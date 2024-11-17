#include "./include/cson.h"
#include "./include/lexer.h"
#include "./include/io.h"
#include <stdlib.h>
#include <assert.h>

Cson* cson_load(char* filepath) {
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

    return cson;
}

char* cson_read_string(Cson* cson, char* key) {
    (void)cson;
    (void)key;

    return "";
}

double cson_read_double(Cson* cson, char* key) {
    (void)cson;
    (void)key;

    return 0.f;
}

bool cson_read_bool(Cson* cson, char* key) {
    (void)cson;
    (void)key;

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
