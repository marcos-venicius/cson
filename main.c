#include "./include/cson.h"

int main() {
    Parser* cson = cson_load("./examples/basic.json");

    if (cson == NULL) {
        return 1;
    }

    char* string_d = cson_read_string(cson, "string");
    double float_d = cson_read_double(cson, "float");
    double integer_d = cson_read_double(cson, "integer");
    bool boolean_true_d = cson_read_bool(cson, "booleanTrue");
    bool boolean_false_d = cson_read_bool(cson, "booleanFalse");
    char* null_d = cson_read_string(cson, "null");
    char* testing = cson_read_string(cson, "testing");

    printf("%s\n", string_d);
    printf("%lf\n", float_d);
    printf("%lf\n", integer_d);
    printf("%d\n", boolean_true_d);
    printf("%d\n", boolean_false_d);
    printf("%s\n", testing);

    if (null_d == NULL) {
        printf("NULL\n");
    } else {
        printf("%s\n", null_d);
    }

    return 0;
}

