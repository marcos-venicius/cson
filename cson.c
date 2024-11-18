#include "./include/cson.h"
#include "./include/lexer.h"
#include "./include/parser.h"
#include "./include/io.h"
#include "./include/common.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void print_tokens(const Cson_Lexer* lexer_cson) {
    const Cson_Token* current = lexer_cson->root;

    while (current != NULL) {
        printf("%s :: %.*s\n", tk_kind_display(current->kind), current->value_len, current->value);

        current = current->next;
    }
}

Cson* cson_load(const char* filepath) {
    assert(filepath != NULL && "input should not be null");

    char* content;

    const int size = read_file_content(filepath, &content);

    if (size == -1) {
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
    lexer->bot = 0;
    lexer->cursor = 0;
    lexer->has_error = false;
    lexer->root = NULL;
    lexer->tail = NULL;

    tokenize(lexer);
    lex(lexer);

#ifdef DEBUG
    print_tokens(lexer);
#endif

    Parser* parser = parse(lexer);

    cson_lexer_free(lexer);

    if (parser == NULL) {
        return NULL;
    }

    Cson* cson = malloc(sizeof(Cson));

    if (cson == NULL) {
        return NULL;
    }

    cson->parser = parser;

    return cson;
}

bool is_kind(const KeyPair* pair, const Cson_Token_Kind kind) {
    return pair->kind == kind;
}

bool match_keys(const KeyPair* pair, const char* key) {
    return strncmp(pair->key, key, pair->key_len) == 0 && strlen(key) == (size_t)pair->key_len;
}

int cson_read_string(const Cson* cson, const char* key, char** output) {
    for (int i = 0; i < cson->parser->size; i++) {
        KeyPair pair = cson->parser->pairs[i];

        if (match_keys(&pair, key)) {
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

int cson_read_double(const Cson* cson, const char* key, double* output) {
    for (int i = 0; i < cson->parser->size; i++) {
        KeyPair pair = cson->parser->pairs[i];

        if (match_keys(&pair, key)) {
            if (is_kind(&pair, NULL_CSON_TOKEN)) {
                return OK_RETURN;
            }

            if (!is_kind(&pair, NUMBER_CSON_TOKEN)) {
                return INVALID_TYPE_RETURN;
            }

            char* result = malloc(pair.value_len + 1);
            char* endptr;

            strncpy(result, pair.value, pair.value_len);

            result[pair.value_len] = '\0';

            *output = strtod(result, &endptr);

            free(result);

            return OK_RETURN;
        }
    }

    return NOT_FOUND_RETURN;
}

int cson_read_bool(const Cson* cson, const char* key, bool* output) {
    for (int i = 0; i < cson->parser->size; i++) {
        KeyPair pair = cson->parser->pairs[i];

        if (match_keys(&pair, key)) {
            if (is_kind(&pair, NULL_CSON_TOKEN)) {
                return OK_RETURN;
            }

            if (!is_kind(&pair, TRUE_CSON_TOKEN) && !is_kind(&pair, FALSE_CSON_TOKEN)) {
                return INVALID_TYPE_RETURN;
            }

            *output = pair.kind == TRUE_CSON_TOKEN;

            return OK_RETURN;
        }
    }

    return NOT_FOUND_RETURN;
}

char* error_explain(const int code) {
    switch (code) {
        case NOT_FOUND_RETURN:
            return "not found";
        case INVALID_TYPE_RETURN:
            return "invalid type";
        default:
            return "unexpected error";
    }
}
