#include "./include/cson.h"
#include "./include/lexer.h"
#include "./include/io.h"
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

    CsonItem item = { .node = NULL };
    bool parsing = true;

    char *p;

    va_list ap;
    va_start(ap, format);
    for (p = format; *p && parsing; p++) {
        switch (*p) {
            case '%':
                continue;
            case 's':
                if (current->kind != STNK_OBJECT) {
                    item.node = NULL;
                    item.return_code = CRC_INVALID_TYPE;
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
                            found = true;
                            current = node;

                            break;
                        }
                    }

                    if (!found) {
                        item.return_code = CRC_NOT_FOUND;
                        parsing = false;
                    }
                }

                break;
            case 'd':
                if (current->kind != STNK_ARRAY) {
                    item.node = NULL;
                    item.return_code = CRC_INVALID_TYPE;
                    parsing = false;

                    break;
                } else {
                    size_t index = va_arg(ap, size_t);

                    if (index < 0 || index >= current->value.array->count) {
                        item.node = NULL;
                        item.return_code = CRC_INVALID_POS;
                        parsing = false;

                        break;
                    }

                    current = ll_find_by_index(current->value.array, index);
                }
                break;
            default:
                item.return_code = CRC_INVALID_FORMAT;
                fprintf(stderr, "invalid formatting string '%c'. available only: %%s %%d\n", *p);
                parsing = false;
                break;
        }
    }
    va_end(ap);

    if (parsing) {
        item.node = current;
        item.return_code = CRC_OK;
    }

    return item;
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
