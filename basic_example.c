#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/basic.json");

    if (cson == NULL) {
        return 1;
    }

    char* stringField;
    char* nullField;
    double integerField;
    double floatField;
    bool trueField;
    bool falseField;

    int result;

    if ((result = cson_read_string(cson, "string", &stringField)) != OK_RETURN) {
        fprintf(stderr, "stringField: %s\n", error_explain(result));
        return 1;
    }

    printf("stringField: %s\n", stringField);

    if ((result = cson_read_double(cson, "integer", &integerField)) != OK_RETURN) {
        fprintf(stderr, "integerField: %s\n", error_explain(result));
        return 1;
    }

    printf("integerField: %f\n", integerField);

    if ((result = cson_read_double(cson, "float", &floatField)) != OK_RETURN) {
        fprintf(stderr, "floatField: %s\n", error_explain(result));
        return 1;
    }

    printf("floatField: %f\n", floatField);

    if ((result = cson_read_string(cson, "null", &nullField)) != OK_RETURN) {
        fprintf(stderr, "nullField: %s\n", error_explain(result));
        return 1;
    }

    printf("nullField: %s\n", nullField);

    if ((result = cson_read_bool(cson, "true", &trueField)) != OK_RETURN) {
        fprintf(stderr, "trueField: %s\n", error_explain(result));
        return 1;
    }

    printf("trueField: %d\n", trueField);

    if ((result = cson_read_bool(cson, "false", &falseField)) != OK_RETURN) {
        fprintf(stderr, "falseField: %s\n", error_explain(result));
        return 1;
    }

    printf("falseField: %d\n", falseField);

    cson_free(cson);

    return 0;
}

