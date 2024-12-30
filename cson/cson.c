#include "./include/cson.h"
#include "./include/lexer.h"
#include "./include/io.h"
#include "include/common.h"
#include "include/st_parser.h"
#include "libs/assertf.h"
#include "libs/ll.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

Cson* cson_load(const char* filepath) {
    assert(filepath != NULL && "input should not be null");

    char* content;

    const unsigned long size = read_file_content(filepath, &content);

    if (size == 0) {
        return NULL;
    }

    Cson_Lexer* lexer = malloc(sizeof(Cson_Lexer));

    if (lexer == NULL) {
        free(content);

        perror("could not allocate memory to the lexer struct");

        return NULL;
    }

    lexer->content = content;
    lexer->content_len = size;
    lexer->tokens_len = 0;
    lexer->bot = 0;
    lexer->cursor = 0;
    lexer->has_error = false;
    lexer->root = NULL;
    lexer->tail = NULL;

    tokenize(lexer);
    lex(lexer);

    SyntaxTree *st = init_syntax_tree_parser(lexer);

    syntax_tree_parse(st);

    cson_lexer_free(lexer);

    Cson* cson = malloc(sizeof(Cson));

    if (cson == NULL) {
        return NULL;
    }

    cson->ast = st;
    cson->root = st->root;

    return cson;
}

CsonItem cson_get(const SyntaxTreeNode *node, char *format, ...) {
    const SyntaxTreeNode *current = node;

    if (current == NULL) {
        return (CsonItem){
            .return_code = CRC_NOT_FOUND,
            .node = NULL
        };
    }

    CsonReturnCode return_code;
    bool parsing = true;

    if (strlen(format) % 2 != 0) {
        return (CsonItem){
            .return_code = CRC_INVALID_FORMAT,
            .node = NULL
        };
    }

    char *p;

    va_list ap;
    va_start(ap, format);
    bool matchSymbol = true;

    for (p = format; *p && parsing; p++) {
        switch (*p) {
            case '%':
                if (!matchSymbol) {
                    return_code = CRC_INVALID_FORMAT;
                    parsing = false;
                }
                matchSymbol = false;
                break;
            case 's':
                matchSymbol = true;
                if (current->kind != STNK_OBJECT) {
                    return_code = CRC_INVALID_TYPE;
                    parsing = false;

                    break;
                } else {
                    char *key = va_arg(ap, char*);
                    bool found = false;

                    LLIter iter = ll_iter(current->value.object);

                    while (ll_iter_has(&iter)) {
                        LLIterItem iter_item = ll_iter_consume(&iter);

                        SyntaxTreeNode *node = iter_item.data;

                        if (strncmp(key, node->name, strlen(node->name)) == 0) {
                            return_code = CRC_OK;
                            found = true;
                            current = node;

                            break;
                        }
                    }

                    if (!found) {
                        return_code = CRC_NOT_FOUND;
                        parsing = false;
                    }
                }

                break;
            case 'd':
                matchSymbol = true;
                if (current->kind != STNK_ARRAY) {
                    return_code = CRC_INVALID_TYPE;
                    parsing = false;

                    break;
                } else {
                    size_t index = va_arg(ap, size_t);

                    if (index < 0 || index >= current->value.array->count) {
                        return_code = CRC_INVALID_POS;
                        parsing = false;

                        break;
                    }

                    current = ll_find_by_index(current->value.array, index);
                    return_code = CRC_OK;
                }
                break;
            default:
                return_code = CRC_INVALID_FORMAT;
                fprintf(stderr, "invalid formatting string '%c'. available only: %%s %%d\n", *p);
                parsing = false;
                break;
        }
    }
    va_end(ap);

    if (parsing && return_code == CRC_OK) {
        return (CsonItem){
            .return_code = return_code,
            .node = current
        };
    }

    return (CsonItem){
        .return_code = return_code,
        .node = NULL
    };
}

char *cson_unwrap_string(CsonItem item) {
    return item.node->value.string;
}

bool cson_unwrap_boolean(CsonItem item) {
    return item.node->value.boolean;    
}

double cson_unwrap_number(CsonItem item) {
    return item.node->value.number;
}

static void print_json(SyntaxTreeNode *node, unsigned int padding, unsigned int shift, bool is_last) {
    switch (node->kind) {
        case STNK_OBJECT: {
            if (node->name != NULL) {
                fprintf(stdout, "%*s\"%s\": {", shift, "", node->name);
            } else {
                fprintf(stdout, "%*s{", shift, "");
            }

            if (node->value.object->count > 0) {
                fprintf(stdout, "\n");                

                LLIter iter = ll_iter(node->value.object);

                while (ll_iter_has(&iter)) {
                    LLIterItem item = ll_iter_consume(&iter);
                    SyntaxTreeNode *child = item.data;

                    print_json(child, padding, shift + padding, item.index == node->value.object->count - 1);
                }

                fprintf(stdout, "%*s}", shift, "");
            } else {
                fprintf(stdout, "}");
            }

            if (is_last) {
                fprintf(stdout, "\n");
            } else {
                fprintf(stdout, ",\n");
            }

            break;
        }
        case STNK_ARRAY: {
            if (node->name != NULL) {
                fprintf(stdout, "%*s\"%s\": [", shift, "", node->name);
            } else {
                fprintf(stdout, "%*s[", shift, "");
            }

            if (node->value.array->count > 0) {
                fprintf(stdout, "\n");

                LLIter iter = ll_iter(node->value.object);

                while (ll_iter_has(&iter)) {
                    LLIterItem item = ll_iter_consume(&iter);
                    SyntaxTreeNode *child = item.data;

                    print_json(child, padding, shift + padding, item.index == node->value.object->count - 1);
                }

                fprintf(stdout, "%*s]", shift, "");
            } else {
                fprintf(stdout, "]");
            }

            if (is_last) {
                fprintf(stdout, "\n");
            } else {
                fprintf(stdout, ",\n");
            }
            break;
        }
        case STNK_STRING:
            if (node->name != NULL) {
                fprintf(stdout, "%*s\"%s\": \"%s\"", shift, "", node->name, node->value.string);
            } else {
                fprintf(stdout, "%*s\"%s\"", shift, "", node->value.string);
            }

            if (is_last) {
                fprintf(stdout, "\n");
            } else {
                fprintf(stdout, ",\n");
            }
            break;
        case STNK_NUMBER:
            if (node->name != NULL) {
                fprintf(stdout, "%*s\"%s\": %f", shift, "", node->name, node->value.number);
            } else {
                fprintf(stdout, "%*s%f", shift, "", node->value.number);
            }

            if (is_last) {
                fprintf(stdout, "\n");
            } else {
                fprintf(stdout, ",\n");
            }
            break;
        case STNK_BOOLEAN:
            if (node->name != NULL) {
                fprintf(stdout, "%*s\"%s\": %s", shift, "", node->name, node->value.boolean ? "true" : "false");
            } else {
                fprintf(stdout, "%*s%s", shift, "", node->value.boolean ? "true" : "false");
            }

            if (is_last) {
                fprintf(stdout, "\n");
            } else {
                fprintf(stdout, ",\n");
            }
            break;
        default:
            fprintf(stderr, "error: print not implemented for \"%s\"\n", stnk_display(node->kind));
            break;
    }
}

void cson_format(Cson *cson, unsigned int padding) {
    print_json(cson->root, padding, 0, true);
}

void cson_free(Cson *cson) {
    syntax_tree_free(cson->ast);
    free(cson);
}

const char* return_code_as_cstr(CsonReturnCode code) {
    switch (code) {
        case CRC_OK:
            return "OK";
        case CRC_INVALID_TYPE:
            return "INVALID_TYPE";
        case CRC_INVALID_POS:
            return "INVALID_POS";
        case CRC_NOT_FOUND:
            return "NOT_FOUND";
        case CRC_INVALID_FORMAT:
            return "INVALID_FORMAT";
        default:
            return "UNKOWN_RETURN_CODE";
    }
}
