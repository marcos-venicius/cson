#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_objects.json");

    if (cson == NULL) {
        return 1;
    }

    char* one;
    char* two;

    int result;

    if ((result = cson_read_string(cson, "one.two.three.find", &one)) != OK_RETURN) {
        fprintf(stderr, "one: %s\n", error_explain(result));
        return 1;
    }

    printf("one: %s\n", one);

    if ((result = cson_read_string(cson, "one.two.neighbor", &two)) != OK_RETURN) {
        fprintf(stderr, "two: %s\n", error_explain(result));
        return 1;
    }

    printf("two: %s\n", two);

    cson_free(cson);

    return 0;
}

