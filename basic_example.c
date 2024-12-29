#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/basic.json");

    if (cson == NULL) {
        return 1;
    }

    CsonItem result;

    if ((result = cson_get(cson->root, "%s", "string")).return_code != CRC_OK) {
        fprintf(stderr, "stringField: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("stringField: %s\n", result.node->value.string);

    if ((result = cson_get(cson->root, "%s", "integer")).return_code != CRC_OK) {
        fprintf(stderr, "integerField: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("integerField: %f\n", result.node->value.number);

    if ((result = cson_get(cson->root, "%s", "float")).return_code != CRC_OK) {
        fprintf(stderr, "floatField: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("floatField: %f\n", result.node->value.number);

    if ((result = cson_get(cson->root, "%s", "null")).return_code != CRC_OK) {
        fprintf(stderr, "nullField: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("nullField: %s\n", result.node->value.string);

    if ((result = cson_get(cson->root, "%s", "true")).return_code != CRC_OK) {
        fprintf(stderr, "trueField: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("trueField: %d\n", result.node->value.boolean);

    if ((result = cson_get(cson->root, "%s", "false")).return_code != CRC_OK) {
        fprintf(stderr, "falseField: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("falseField: %d\n", result.node->value.boolean);

    cson_free(cson);

    return 0;
}

