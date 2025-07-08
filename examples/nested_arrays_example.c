#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_arrays.json");

    if (cson == NULL) {
        return 1;
    }

    CsonItem one = cson_get(cson->root, "%d%d%d", 0, 0, 0);

    if (one.return_code != CRC_OK) {
        fprintf(stderr, "one: %s\n", return_code_as_cstr(one.return_code));
        return 1;
    }

    printf("one: %s\n", cson_unwrap_string(one));

    CsonItem two = cson_get(cson->root, "%d%d", 0, 2);

    if (two.return_code != CRC_OK) {
        fprintf(stderr, "two: %s\n", return_code_as_cstr(two.return_code));
        return 1;
    }

    printf("two: %s\n", cson_unwrap_string(two));

    cson_free(cson);

    return 0;
}

