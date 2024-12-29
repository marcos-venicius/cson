#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/st_parser.h"
#include "include/common.h"
#include "include/lexer.h"
#include "libs/ll.h"

void free_syntax_tree_node(void *data) {
    SyntaxTreeNode *node = data;

    switch (node->kind) {
        case STNK_ARRAY:
            ll_free(node->value.array);
            break;
        case STNK_OBJECT:
            ll_free(node->value.object);
        default:
            break;
    }

    free(node);
}

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
    node->value = (SyntaxTreeNodeValue){
        .object = ll_new(free_syntax_tree_node, NULL), // SyntaxTreeNode[]
    };

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

SyntaxTreeNode *create_syntax_tree_node_array() {
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    node->kind = STNK_ARRAY;
    node->value = (SyntaxTreeNodeValue){
        .array = ll_new(free_syntax_tree_node, NULL), // SyntaxTreeNode[]
    };

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_string(char *name, size_t name_size, char *data, size_t data_size) {
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    char *null_terminated_name = malloc(name_size + 1);

    memcpy(null_terminated_name, name, name_size);

    null_terminated_name[name_size] = '\0';

    node->kind = STNK_STRING;

    if (data_size > 0) {
        char *null_terminated_data = malloc(data_size + 1);

        memcpy(null_terminated_data, data, data_size);

        null_terminated_data[data_size] = '\0';

        node->value.string = null_terminated_data;
    } else {
        node->value.string = NULL;
    }

    node->name = null_terminated_name;

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_bool(char *name, size_t name_size, bool value) {
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    char *null_terminated_name = malloc(name_size + 1);

    memcpy(null_terminated_name, name, name_size);

    null_terminated_name[name_size] = '\0';

    node->kind = STNK_BOOLEAN;
    node->name = null_terminated_name;
    node->value.boolean = value;

    return node;
}

SyntaxTreeNode *create_syntax_tree_node_number(char *name, size_t name_size, char *data, size_t data_size) {
    SyntaxTreeNode *node = malloc(sizeof(SyntaxTreeNode));

    char *null_terminated_name = malloc(name_size + 1);
    char *null_terminated_data = malloc(data_size + 1);
    char* endptr;

    memcpy(null_terminated_name, name, name_size);
    memcpy(null_terminated_data, data, data_size);

    null_terminated_name[name_size] = '\0';

    node->kind = STNK_NUMBER;
    node->value.number = strtod(null_terminated_data, &endptr);
    node->name = null_terminated_name;

    return node;
}

SyntaxTreeNode *syntax_tree_parse_object(SyntaxTree *st, char *name, int name_size) {
    SyntaxTreeNode *rootObject = create_syntax_tree_node_object(name, name_size);

    bool parsing = true;

    while (parsing) {
        Cson_Token *left = expect(next_token(st), KEY_CSON_TOKEN);

        expect(next_token(st), COLON_CSON_TOKEN);

        Cson_Token *right = next_token(st);

        switch (right->kind) {
            case STRING_CSON_TOKEN: {
                SyntaxTreeNode *node = create_syntax_tree_node_string(left->value, left->value_len, right->value, right->value_len);

                ll_add(rootObject->value.object, node, 0);

                break;
            }
            case NUMBER_CSON_TOKEN: {
                SyntaxTreeNode *node = create_syntax_tree_node_number(left->value, left->value_len, right->value, right->value_len);

                ll_add(rootObject->value.object, node, 0);
                break;
            }
            case NULL_CSON_TOKEN: {
                SyntaxTreeNode *node = create_syntax_tree_node_string(left->value, left->value_len, NULL, 0);

                ll_add(rootObject->value.object, node, 0);

                break;
            }
            case TRUE_CSON_TOKEN: {
                SyntaxTreeNode *node = create_syntax_tree_node_bool(left->value, left->value_len, true);

                ll_add(rootObject->value.object, node, 0);

                break;
            }
            case FALSE_CSON_TOKEN: {
                SyntaxTreeNode *node = create_syntax_tree_node_bool(left->value, left->value_len, false);

                ll_add(rootObject->value.object, node, 0);

                break;
            }
            case LBRACE_CSON_TOKEN: {
                SyntaxTreeNode *node = syntax_tree_parse_object(st, left->value, left->value_len);

                ll_add(rootObject->value.object, node, 0);

                break;
            }
            case LSQUARE_CSON_TOKEN:
                break;
            default:
                fprintf(stderr, "unexpected token \"%s\"", tk_kind_display(right->kind));
                exit(1);
        }

        Cson_Token *token = next_token(st);

        if (token == NULL) break;
 
        switch (token->kind) {
            case RBRACE_CSON_TOKEN:
                return rootObject;
            default:
                expect(token, COMMA_CSON_TOKEN);
        }
    }

    return rootObject;
}

SyntaxTreeNode *syntax_tree_parse_array(SyntaxTree *st) {
    printf("parsing array\n");

    return NULL;
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
            node = syntax_tree_parse_array(st);
            break;
        case RSQUARE_CSON_TOKEN:
            return;
        default:
            // TODO: improve error handling    
            fprintf(stderr, "invalid token: \"%s\"\n", tk_kind_display(token->kind));
            exit(1);
    }

    stn_display(node, 0);
}
