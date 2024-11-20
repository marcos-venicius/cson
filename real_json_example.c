#include <stdio.h>
#include <time.h>
#include "cson/include/cson.h"

int main() {
    clock_t start, end;

    start = clock();

    Cson* cson = cson_load("./examples/real_json.json");

    end = clock();

    cson_free(cson);

    printf("Time: %fs\n", (double)(end - start) / (double)CLOCKS_PER_SEC);

    return 0;
}
