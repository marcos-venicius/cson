#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/nested_stuff.json");

    if (cson == NULL) {
        return 1;
    }

    char* one;

    int result;

    if ((result = cson_read_string(cson, "one.two.three.[1].test.hello.[0]", &one)) != OK_RETURN) {
        fprintf(stderr, "one: %s\n", error_explain(result));
        return 1;
    }

    printf("one: %s\n", one);

    parser_cleanup(cson->parser);
    free(cson);

    return 0;
}

