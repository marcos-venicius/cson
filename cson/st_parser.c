#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/st_parser.h"
#include "./include/common.h"
#include "./include/lexer.h"
#include "./libs/ll.h"

SyntaxTreeNode *syntax_tree_parse_array(SyntaxTree *st, char *name, size_t name_size);
SyntaxTreeNode *syntax_tree_parse_object(SyntaxTree *st, char *name, size_t name_size);

static Cson_Token *next_token(SyntaxTree *st) {
    if (st->token->kind == EOF_CSON_TOKEN) return NULL;

    st->token = st->token->next;

    return st->token;
}

static char* unescape_sequence(char* string) {
    int len = (int)strlen(string);
    int string_i = 0;
    int scape_i = 0;
    char* unescaped = calloc(sizeof(char), len);

    while (string_i < len) {
        if (string[string_i] == '\\') {
            switch (string[string_i + 1]) {
                case '"':
                    unescaped[scape_i] = '"';
                    break;
                case '\\':
                    unescaped[scape_i] = '\\';
                    break;
                case 'n':
                    unescaped[scape_i] = '\n';
                    break;
                case 't':
                    unescaped[scape_i] = '\t';
                    break;
                case 'b':
                    unescaped[scape_i] = '\t';
                    break;
                case 'r':
                    unescaped[scape_i] = '\r';
                    break;
                case 'f':
                    unescaped[scape_i] = '\f';
                    break;
                default:
                    fprintf(stderr, "invalid escape string \"%c%c\"\n", string[string_i], string[string_i + 1]);
                    exit(1);
            }
            string_i++;
        } else {
            unescaped[scape_i] = string[string_i];
        }
        string_i++;
        scape_i++;
    }

    free(string);

    return unescaped;
}

Cson_Token *expect(Cson_Token *token, Cson_Token_Kind kind) {
    if (token == NULL || token->kind != kind) {
        fprintf(stderr, "unexpected token \"%s\"\n", tk_kind_display(token->kind));
        exit(1);
    }

    return token;
}

SyntaxTree *init_syntax_tree_parser(Cson_Lexer *lexer) {
    SyntaxTree *syntaxTree = calloc(1, sizeof(SyntaxTree));

    if (syntaxTree == NULL) {
        return NULL;
    }

    syntaxTree->token = lexer->root;

    return syntaxTree;
}

SyntaxTreeNode *create_syntax_tree_node_object(char *name, size_t name_size) {
    SyntaxTreeNode *node = calloc(1, sizeof(SyntaxTreeNode));

    node->kind = STNK_OBJECT;
    node->value.as_object = ll_new(NULL, NULL); // SyntaxTreeNode[]

    if (name_size > 0) {
        char *null_terminated_name = malloc(name_size + 1);

        memcpy(null_terminated_name, name, name_size);

        null_terminated_name[name_size] = '\0';

        node->name = null_terminated_name;
    } else {
        node->name = NULL;
    }

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_array(char *name, size_t name_size) {
    SyntaxTreeNode *node = calloc(1, sizeof(SyntaxTreeNode));

    node->kind = STNK_ARRAY;
    node->value.as_array = ll_new(NULL, NULL); // SyntaxTreeNode[]

    if (name_size > 0) {
        char *null_terminated_name = malloc(name_size + 1);

        memcpy(null_terminated_name, name, name_size);

        null_terminated_name[name_size] = '\0';

        node->name = null_terminated_name;
    } else {
        node->name = NULL;
    }
 
    return node;
}

SyntaxTreeNode *create_syntax_tree_node_string(char *name, size_t name_size, char *data, size_t data_size) {
    SyntaxTreeNode *node = calloc(1, sizeof(SyntaxTreeNode));

    node->kind = STNK_STRING;

    if (data_size > 0) {
        char *null_terminated_data = malloc(data_size + 1);

        memcpy(null_terminated_data, data, data_size);

        null_terminated_data[data_size] = '\0';

        node->value.as_string = unescape_sequence(null_terminated_data);
    } else {
        node->value.as_string = NULL;
    }

    if (name_size > 0) {
        char *null_terminated_name = malloc(name_size + 1);

        memcpy(null_terminated_name, name, name_size);

        null_terminated_name[name_size] = '\0';
        node->name = null_terminated_name;
    } else {
        node->name = NULL;
    }

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_bool(char *name, size_t name_size, bool value) {
    SyntaxTreeNode *node = calloc(1, sizeof(SyntaxTreeNode));

    node->kind = STNK_BOOLEAN;
    node->value.as_bool = value;

    if (name_size > 0) {
        char *null_terminated_name = malloc(name_size + 1);
        memcpy(null_terminated_name, name, name_size);

        null_terminated_name[name_size] = '\0';
        node->name = null_terminated_name;
    } else {
        node->name = NULL;
    }

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_float(char *name, size_t name_size, char *data, size_t data_size) {
    SyntaxTreeNode *node = calloc(1, sizeof(SyntaxTreeNode));

    char *null_terminated_data = malloc(data_size + 1);
    char* endptr;

    memcpy(null_terminated_data, data, data_size);

    int precision = 0;
    bool measuring = false;

    for (size_t i = 0; i < data_size; ++i) {
        if (measuring) {
            ++precision;
            continue;
        }

        if (null_terminated_data[i] == '.') {
            measuring = true;
        }
    }

    node->kind = STNK_FLOAT;
    node->value.as_float = strtold(null_terminated_data, &endptr);
    node->precision = precision;

    free(null_terminated_data);

    if (name_size > 0) {
        char *null_terminated_name = malloc(name_size + 1);
        memcpy(null_terminated_name, name, name_size);
        
        null_terminated_name[name_size] = '\0';
        node->name = null_terminated_name;
    } else {
        node->name = NULL;
    }

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_integer(char *name, size_t name_size, char *data, size_t data_size) {
    SyntaxTreeNode *node = calloc(1, sizeof(SyntaxTreeNode));

    char *null_terminated_data = malloc(data_size + 1);

    memcpy(null_terminated_data, data, data_size);

    node->kind = STNK_INTEGER;
    node->value.as_integer = strtol(null_terminated_data, NULL, 10);

    free(null_terminated_data);

    if (name_size > 0) {
        char *null_terminated_name = malloc(name_size + 1);
        memcpy(null_terminated_name, name, name_size);
        
        null_terminated_name[name_size] = '\0';
        node->name = null_terminated_name;
    } else {
        node->name = NULL;
    }

    return node;
}

SyntaxTreeNode *syntax_tree_parse_object(SyntaxTree *st, char *name, size_t name_size) {
    SyntaxTreeNode *object = create_syntax_tree_node_object(name, name_size);

    bool parsing = true;

    while (parsing) {
        SyntaxTreeNode *node;

        Cson_Token *left = next_token(st);

        if (left->kind == RBRACE_CSON_TOKEN) {
            break;
        }

        expect(left, KEY_CSON_TOKEN);

        expect(next_token(st), COLON_CSON_TOKEN);

        Cson_Token *right = next_token(st);

        switch (right->kind) {
            case STRING_CSON_TOKEN: {
                node = create_syntax_tree_node_string(left->value, left->value_len, right->value, right->value_len);

                break;
            }
            case FLOAT_CSON_TOKEN: {
                node = create_syntax_tree_node_float(left->value, left->value_len, right->value, right->value_len);

                break;
            }
            case INTEGER_CSON_TOKEN: {
                node = create_syntax_tree_node_integer(left->value, left->value_len, right->value, right->value_len);

                break;
            }
            case NULL_CSON_TOKEN: {
                node = create_syntax_tree_node_string(left->value, left->value_len, NULL, 0);

                break;
            }
            case TRUE_CSON_TOKEN: {
                node = create_syntax_tree_node_bool(left->value, left->value_len, true);

                break;
            }
            case FALSE_CSON_TOKEN: {
                node = create_syntax_tree_node_bool(left->value, left->value_len, false);

                break;
            }
            case LBRACE_CSON_TOKEN: {
                node = syntax_tree_parse_object(st, left->value, left->value_len);

                break;
            }
            case LSQUARE_CSON_TOKEN: {
                node = syntax_tree_parse_array(st, left->value, left->value_len);

                break;
            }
            default:
                fprintf(stderr, "unexpected token \"%s\"", tk_kind_display(right->kind));
                exit(1);
        }

        ll_add(object->value.as_object, node, 0);

        Cson_Token *token = next_token(st);

        if (token == NULL) break;
 
        switch (token->kind) {
            case RBRACE_CSON_TOKEN:
                return object;
            default:
                expect(token, COMMA_CSON_TOKEN);
        }
    }

    return object;
}

SyntaxTreeNode *syntax_tree_parse_array(SyntaxTree *st, char *name, size_t name_size) {
    SyntaxTreeNode *array = create_syntax_tree_node_array(name, name_size);

    bool parsing = true;

    while (parsing) {
        SyntaxTreeNode *node;

        Cson_Token *next = next_token(st);

        if (next->kind == RSQUARE_CSON_TOKEN) {
            break;
        }

        switch (next->kind) {
            case STRING_CSON_TOKEN: {
                node = create_syntax_tree_node_string(NULL, 0, next->value, next->value_len);

                break;
            }
            case FLOAT_CSON_TOKEN: {
                node = create_syntax_tree_node_float(NULL, 0, next->value, next->value_len);

                break;
            }
            case INTEGER_CSON_TOKEN: {
                node = create_syntax_tree_node_integer(NULL, 0, next->value, next->value_len);

                break;
            }
            case NULL_CSON_TOKEN: {
                node = create_syntax_tree_node_string(NULL, 0, NULL, 0);

                break;
            }
            case TRUE_CSON_TOKEN: {
                node = create_syntax_tree_node_bool(NULL, 0, true);

                break;
            }
            case FALSE_CSON_TOKEN: {
                node = create_syntax_tree_node_bool(NULL, 0, false);

                break;
            }
            case LBRACE_CSON_TOKEN: {
                node = syntax_tree_parse_object(st, NULL, 0);

                break;
            }
            case LSQUARE_CSON_TOKEN: {
                node = syntax_tree_parse_array(st, NULL, 0);

                break;
            }
            default:
                fprintf(stderr, "unexpected token \"%s\"", tk_kind_display(next->kind));
                exit(1);
        }

        ll_add(array->value.as_array, node, 0);

        Cson_Token *token = next_token(st);

        if (token == NULL) break;

        switch (token->kind) {
            case RSQUARE_CSON_TOKEN:
                return array;
            default:
                expect(token, COMMA_CSON_TOKEN);
        }
    }

    return array;
}

void syntax_tree_parse(SyntaxTree *st) {
    Cson_Token *token = st->token;

    SyntaxTreeNode *node;

    switch (token->kind) {
        case LBRACE_CSON_TOKEN:
            node = syntax_tree_parse_object(st, NULL, 0);
            break;
        case RBRACE_CSON_TOKEN:
            return;
        case LSQUARE_CSON_TOKEN:
            node = syntax_tree_parse_array(st, NULL, 0);
            break;
        case RSQUARE_CSON_TOKEN:
            return;
        default:
            // TODO: improve error handling    
            fprintf(stderr, "invalid token: \"%s\"\n", tk_kind_display(token->kind));
            exit(1);
    }

    st->root = node;
}

void syntax_tree_free_list(SyntaxTreeNode *list) {
    if (list->name != NULL) {
        free(list->name);
    }

    LLIter iter;
    
    switch (list->kind) {
        case STNK_ARRAY:
            iter = ll_iter(list->value.as_array);
            break;
        case STNK_OBJECT:
            iter = ll_iter(list->value.as_object);
            break;
        default:
            fprintf(stderr, "Unexpected kind when freeing memory\n");
            exit(1);
    }

    while (ll_iter_has(&iter)) {
        LLIterItem item = ll_iter_consume(&iter);

        SyntaxTreeNode *node = item.data;

        switch (node->kind) {
            case STNK_BOOLEAN:
            case STNK_FLOAT:
            case STNK_INTEGER:
                if (node->name != NULL) free(node->name);
                break;
            case STNK_STRING:
                if (node->name != NULL) free(node->name);
                if (node->value.as_string != NULL) free(node->value.as_string);
                break;
            case STNK_OBJECT:
            case STNK_ARRAY:
                syntax_tree_free_list(node);
                break;
            default:
                break;
        }
    }   

    switch (list->kind) {
        case STNK_ARRAY:
            ll_free(list->value.as_array);
            break;
        case STNK_OBJECT:
            ll_free(list->value.as_object);
            break;
        default:
            fprintf(stderr, "Unexpected kind when freeing memory\n");
            exit(1);
    }
}

void syntax_tree_free(SyntaxTree *st) {
    SyntaxTreeNode *current = st->root;

    syntax_tree_free_list(current);

    free(current);
    free(st);
}
