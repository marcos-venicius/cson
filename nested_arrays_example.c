#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_arrays.json");

    if (cson == NULL) {
        return 1;
    }

    char* one;
    char* two;

    int result;

    if ((result = cson_read_string(cson, "[0].[0].[0]", &one)) != OK_RETURN) {
        fprintf(stderr, "one: %s\n", error_explain(result));
        return 1;
    }

    printf("one: %s\n", one);

    if ((result = cson_read_string(cson, "[0].[1]", &two)) != OK_RETURN) {
        fprintf(stderr, "two: %s\n", error_explain(result));
        return 1;
    }

    printf("two: %s\n", two);


    parser_cleanup(cson->parser);
    free(cson);

    return 0;
}

