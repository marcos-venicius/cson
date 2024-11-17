#include "./include/cson.h"
#include "./include/lexer.h"
#include "./include/parser.h"
#include "./include/io.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

Parser* cson_load(char* filepath) {
    assert(filepath != NULL && "input should not be null");

    char* content;

    int size = read_file_content(filepath, &content);

    if (size == -1) {
        return NULL;
    }

    Cson* cson = malloc(sizeof(Cson));

    if (cson == NULL) {
        free(content);

        perror("could not allocate memory to the cson struct");

        return NULL;
    }

    cson->content = content;
    cson->content_len = size;
    cson->bot = 0;
    cson->cursor = 0;
    cson->has_error = false;
    cson->root = NULL;
    cson->tail = NULL;

    tokenize(cson);

    Parser* parser = parse(cson);

    cson_end(cson);

    return parser;
}

bool is_kind(KeyPair* pair, Cson_Token_Kind kind) {
    return pair->kind == kind;
}

// FIXME: "test" == "test " but it should not occour
int cson_read_string(Parser* cson, char* key, char** output) {
    for (int i = 0; i < cson->size; i++) {
        KeyPair pair = cson->pairs[i];

        if (strncmp(pair.key, key, pair.key_len - 1) == 0) {
            if (is_kind(&pair, NULL_CSON_TOKEN)) {
                return OK_RETURN;
            }

            if (!is_kind(&pair, STRING_CSON_TOKEN)) {
                return INVALID_TYPE_RETURN;
            }

            *output = (char*)malloc(pair.value_len + 1);

            strncpy(*output, pair.value, pair.value_len);

            (*output)[pair.value_len] = '\0';

            return OK_RETURN;
        }
    }

    return NOT_FOUND_RETURN;
}

int cson_read_double(Parser* cson, char* key, double* output) {
    for (int i = 0; i < cson->size; i++) {
        KeyPair pair = cson->pairs[i];

        if (strncmp(pair.key, key, pair.key_len - 1) == 0) {
            if (is_kind(&pair, NULL_CSON_TOKEN)) {
                return OK_RETURN;
            }

            if (!is_kind(&pair, NUMBER_CSON_TOKEN)) {
                return INVALID_TYPE_RETURN;
            }

            char* result = (char*)malloc(pair.value_len + 1);
            char* endptr;

            strncpy(result, pair.value, pair.value_len);

            result[pair.value_len] = '\0';

            (*output) = strtod(result, &endptr);

            return OK_RETURN;
        }
    }

    return NOT_FOUND_RETURN;
}

int cson_read_bool(Parser* cson, char* key, bool* output) {
    for (int i = 0; i < cson->size; i++) {
        KeyPair pair = cson->pairs[i];

        if (strncmp(pair.key, key, pair.key_len - 1) == 0) {
            if (is_kind(&pair, NULL_CSON_TOKEN)) {
                return OK_RETURN;
            }

            if (!is_kind(&pair, TRUE_CSON_TOKEN) && !is_kind(&pair, FALSE_CSON_TOKEN)) {
                return INVALID_TYPE_RETURN;
            }

            (*output) = pair.kind == TRUE_CSON_TOKEN;

            return OK_RETURN;
        }
    }

    return NOT_FOUND_RETURN;
}

void cson_end(Cson* cson) {
    Cson_Token* current = cson->root;

    while (current != NULL) {
        Cson_Token* next = current->next;
        free(current);
        current = next;
    }
}

char* error_explain(int code) {
    switch (code) {
        case NOT_FOUND_RETURN:
            return "not found";
        case INVALID_TYPE_RETURN:
            return "invalid type";
        default:
            return "unexpected error";
    }
}
