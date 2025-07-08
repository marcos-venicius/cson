#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_stuff.json");

    if (cson == NULL) {
        return 1;
    }

    CsonItem result = cson_get(cson->root, "%s%s%s%d%s%s%d", "one", "two", "three", 1, "test", "hello", 0);

    if (result.return_code != CRC_OK) {
        fprintf(stderr, "one: %s\n", return_code_as_cstr(result.return_code));
        return 1;
    }

    printf("one: %s\n", cson_unwrap_string(result));

    cson_free(cson);

    return 0;
}

