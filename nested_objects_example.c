#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_objects.json");

    if (cson == NULL) {
        return 1;
    }

    CsonItem find = cson_get(cson->root, "%s%s%s%s", "one", "two", "three", "find");

    if (find.return_code != CRC_OK) {
        fprintf(stderr, "one: %s\n", return_code_as_cstr(find.return_code));
        return 1;
    }

    printf("one: %s\n", cson_unwrap_string(find));

    CsonItem neighbor = cson_get(cson->root, "%s%s%s", "one", "two", "neighbor");

    if (neighbor.return_code != CRC_OK) {
        fprintf(stderr, "two: %s\n", return_code_as_cstr(neighbor.return_code));
        return 1;
    }

    printf("two: %s\n", cson_unwrap_string(neighbor));

    cson_free(cson);

    return 0;
}

