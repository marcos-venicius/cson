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

char* cson_read_string(Parser* cson, char* key) {
    // TODO: check types
    // TODO: improve code structure and remove parser from here
    for (int i = 0; i < cson->size; i++) {
        KeyPair pair = cson->pairs[i];

        if (strncmp(pair.key, key, pair.key_len - 1) == 0) {
            char* result = (char*)malloc(pair.value_len + 1);

            strncpy(result, pair.value, pair.value_len);

            result[pair.value_len] = '\0';

            return result;
        }
    }

    return NULL;
}

double cson_read_double(Parser* cson, char* key) {
    for (int i = 0; i < cson->size; i++) {
        KeyPair pair = cson->pairs[i];

        if (strncmp(pair.key, key, pair.key_len - 1) == 0) {
            char* result = (char*)malloc(pair.value_len + 1);
            char* endptr;

            strncpy(result, pair.value, pair.value_len);

            result[pair.value_len] = '\0';

            // TODO: check if everything is right, checking type
            return strtod(result, &endptr);
        }
    }

    return 0.f;
}

bool cson_read_bool(Parser* cson, char* key) {
    for (int i = 0; i < cson->size; i++) {
        KeyPair pair = cson->pairs[i];

        if (strncmp(pair.key, key, pair.key_len - 1) == 0) {
            // TODO: check if the pair kind is true or false
            return pair.kind == TRUE_CSON_TOKEN;
        }
    }

    return false;
}

void cson_end(Cson* cson) {
    Cson_Token* current = cson->root;

    while (current != NULL) {
        Cson_Token* next = current->next;
        free(current);
        current = next;
    }
}
