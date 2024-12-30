#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_objects.json");

    if (cson == NULL) {
        return 1;
    }

    CsonItem result;

    if ((result = cson_get(cson->root, "%s%s%s%s", "one", "two", "three", "find")).return_code != CRC_OK) {
        fprintf(stderr, "one: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("one: %s\n", result.node->value.string);

    if ((result = cson_get(cson->root, "%s%s%s", "one", "two", "neighbor")).return_code != CRC_OK) {
        fprintf(stderr, "two: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("two: %s\n", result.node->value.string);

    cson_free(cson);

    return 0;
}

