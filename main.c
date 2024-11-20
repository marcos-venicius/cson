#include <stdio.h>
#include "./include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/basic.json");

    if (cson == NULL) {
        return 1;
    }

    char* output;
    char* lorem;
    double pi;
    bool active;
    int result;

    if ((result = cson_read_string(cson, "url", &output)) != OK_RETURN) {
        fprintf(stderr, "url: %s\n", error_explain(result));
        return 1;
    }

    printf("Nested field: %s\n", output);

    if ((result = cson_read_string(cson, "lorem ipsum", &lorem)) != OK_RETURN) {
        fprintf(stderr, "lorem ipsum: %s\n", error_explain(result));
        return 1;
    }

    printf("lorem ipsum: %s\n", lorem);

    if ((result = cson_read_double(cson, "pi", &pi)) != OK_RETURN) {
        fprintf(stderr, "pi: %s\n", error_explain(result));
        return 1;
    }

    printf("pi: %f\n", pi);

    if ((result = cson_read_bool(cson, "active", &active)) != OK_RETURN) {
        fprintf(stderr, "active: %s\n", error_explain(result));
        return 1;
    }

    printf("active: %d\n", active);

    parser_cleanup(cson->parser);
    free(cson);

    return 0;
}

