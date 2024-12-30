#include <stdio.h>
#include "./cson/include/cson.h"
#include <stdlib.h>

int main() {
    Cson* cson = cson_load("./examples/basic.json");

    if (cson == NULL) {
        return 1;
    }

    CsonItem string = cson_get(cson->root, "%s", "string");
    CsonItem integer = cson_get(cson->root, "%s", "integer");
    CsonItem flutuant = cson_get(cson->root, "%s", "float");
    CsonItem null =  cson_get(cson->root, "%s", "null");
    CsonItem booleanTrue = cson_get(cson->root, "%s", "true");
    CsonItem booleanFalse = cson_get(cson->root, "%s", "false");

    if (string.return_code != CRC_OK) {
        fprintf(stderr, "stringField: %s\n", return_code_as_cstr(string.return_code));
        return 1;
    }

    printf("stringField: %s\n", cson_unwrap_string(string));

    if (integer.return_code != CRC_OK) {
        fprintf(stderr, "integerField: %s\n", return_code_as_cstr(integer.return_code));
    } else {
        printf("integerField: %ld\n", cson_unwrap_integer(integer));
    }


    if (flutuant.return_code != CRC_OK) {
        fprintf(stderr, "floatField: %s\n", return_code_as_cstr(flutuant.return_code));
    } else {
        printf("floatField: %.*Lf\n", flutuant.node->precision, cson_unwrap_float(flutuant));
    }


    if (null.return_code != CRC_OK) {
        fprintf(stderr, "nullField: %s\n", return_code_as_cstr(null.return_code));
    } else {
        printf("nullField: %s\n", cson_unwrap_string(null));
    }

    if (booleanTrue.return_code != CRC_OK) {
        fprintf(stderr, "trueField: %s\n", return_code_as_cstr(booleanTrue.return_code));
    } else {
        printf("trueField: %d\n", cson_unwrap_boolean(booleanTrue));
    }


    if (booleanFalse.return_code != CRC_OK) {
        fprintf(stderr, "falseField: %s\n", return_code_as_cstr(booleanFalse.return_code));
    } else {
        printf("falseField: %d\n", cson_unwrap_boolean(booleanFalse));
    }

    cson_free(cson);

    return 0;
}

