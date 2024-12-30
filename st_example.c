#include "cson/include/common.h"
#include "cson/include/cson.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void usage(FILE *stream, char *program_name) {
    fprintf(stream, "Usage: %s <json-file> [OPTIONS]\n", program_name);
    fprintf(stream, "    -p                print the syntax tree\n");
    fprintf(stream, "    -g                get property value from json (not implemented yet)\n");
    fprintf(stream, "\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage(stderr, argv[0]);
        return 1;
    }

    clock_t start, end;

    start = clock();

    Cson *cson = cson_load(argv[1]);

    if (cson == NULL) {
        return 1;
    }

    end = clock();

    if (argc >= 3 && strncmp("-p", argv[2], 2) == 0) {
        stn_display(cson->root, 0, NULL);
    } else if (argc >= 3 && strncmp("-g", argv[2], 2) == 0) {
        usage(stderr, argv[0]);
        cson_free(cson);
        return 1;
    }

    cson_free(cson);

    printf("Time to parse: %fs\n", (double)(end - start) / (double)CLOCKS_PER_SEC);

    return 0;
}
