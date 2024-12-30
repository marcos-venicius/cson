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

    printf("stringField: %s\n", string.node->value.string);

    if (integer.return_code != CRC_OK) {
        fprintf(stderr, "integerField: %s\n", return_code_as_cstr(integer.return_code));
    } else {
        printf("integerField: %f\n", integer.node->value.number);
    }


    if (flutuant.return_code != CRC_OK) {
        fprintf(stderr, "floatField: %s\n", return_code_as_cstr(flutuant.return_code));
    } else {
        printf("floatField: %f\n", flutuant.node->value.number);
    }


    if (null.return_code != CRC_OK) {
        fprintf(stderr, "nullField: %s\n", return_code_as_cstr(null.return_code));
    } else {
        printf("nullField: %s\n", null.node->value.string);
    }

    if (booleanTrue.return_code != CRC_OK) {
        fprintf(stderr, "trueField: %s\n", return_code_as_cstr(booleanTrue.return_code));
    } else {
        printf("trueField: %d\n", booleanTrue.node->value.boolean);
    }


    if (booleanFalse.return_code != CRC_OK) {
        fprintf(stderr, "falseField: %s\n", return_code_as_cstr(booleanFalse.return_code));
    } else {
        printf("falseField: %d\n", booleanFalse.node->value.boolean);
    }

    cson_free(cson);

    return 0;
}

