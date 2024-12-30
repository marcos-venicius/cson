#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/st_parser.h"
#include "include/common.h"
#include "include/lexer.h"
#include "libs/ll.h"

SyntaxTreeNode *syntax_tree_parse_array(SyntaxTree *st, char *name, size_t name_size);
SyntaxTreeNode *syntax_tree_parse_object(SyntaxTree *st, char *name, size_t name_size);

static Cson_Token *next_token(SyntaxTree *st) {
    if (st->token->kind == EOF_CSON_TOKEN) return NULL;

    st->token = st->token->next;

    return st->token;
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
    node->value.object = ll_new(NULL, NULL); // SyntaxTreeNode[]

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
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    node->kind = STNK_ARRAY;
    node->value.array = ll_new(NULL, NULL); // SyntaxTreeNode[]

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
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    node->kind = STNK_STRING;

    if (data_size > 0) {
        char *null_terminated_data = malloc(data_size + 1);

        memcpy(null_terminated_data, data, data_size);

        null_terminated_data[data_size] = '\0';

        node->value.string = null_terminated_data;
    } else {
        node->value.string = NULL;
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
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    node->kind = STNK_BOOLEAN;
    node->value.boolean = value;

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

SyntaxTreeNode *create_syntax_tree_node_number(char *name, size_t name_size, char *data, size_t data_size) {
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    char *null_terminated_data = malloc(data_size + 1);
    char* endptr;

    memcpy(null_terminated_data, data, data_size);

    node->kind = STNK_NUMBER;
    node->value.number = strtod(null_terminated_data, &endptr);

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
            case NUMBER_CSON_TOKEN: {
                node = create_syntax_tree_node_number(left->value, left->value_len, right->value, right->value_len);

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

        ll_add(object->value.object, node, 0);

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

        switch (next->kind) {
            case STRING_CSON_TOKEN: {
                node = create_syntax_tree_node_string(NULL, 0, next->value, next->value_len);

                break;
            }
            case NUMBER_CSON_TOKEN: {
                node = create_syntax_tree_node_number(NULL, 0, next->value, next->value_len);

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

        ll_add(array->value.array, node, 0);

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
            iter = ll_iter(list->value.array);
            break;
        case STNK_OBJECT:
            iter = ll_iter(list->value.object);
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
                if (node->name != NULL) free(node->name);
                break;
            case STNK_NUMBER:
                if (node->name != NULL) free(node->name);
                break;
            case STNK_STRING:
                if (node->name != NULL) free(node->name);
                if (node->value.string != NULL) free(node->value.string);
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
            ll_free(list->value.array);
            break;
        case STNK_OBJECT:
            ll_free(list->value.object);
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
