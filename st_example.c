#include "cson/include/cson.h"
#include <stdio.h>

int main() {
    Cson *cson = cson_load("./examples/nested_stuff.json");

    CsonItem item = cson_get(cson->root, "%s%s%s%d", "one", "two", "three", 0);

    if (item.return_code != CRC_OK) {
        fprintf(stderr, "error: %s\n", return_code_as_cstr(item.return_code));
    } else {
        printf("value: %f\n", item.node->value.number);
    }

    CsonItem objectThree = cson_get(cson->root, "%s%s%s", "one", "two", "three");
    CsonItem message = cson_get(objectThree.node, "%d%s%s%d", 1, "test", "hello", 0);

    printf("message: %s\n", message.node->value.string);

    cson_free(cson);
}
