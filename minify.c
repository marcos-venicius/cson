#include "./cson/include/cson.h"
#include <stdio.h>

void usage(FILE *stream, char *program_name, char *error) {
    fprintf(stream, "Usage: %s <input-file-name> <output-file-name>\n", program_name);
    fprintf(stream, "error: %s\n", error);
    fprintf(stream, "\n");
}

char *shift(int *argc, char ***argv) {
    if ((*argc)-- == 0) return NULL;

    return *((*argv)++);
}

int main(int argc, char **argv) {
    char *program_name = shift(&argc, &argv);

    char *input_filepath = shift(&argc, &argv);

    if (input_filepath == NULL) {
        usage(stderr, program_name, "please, specify the input json file");
        return 1;
    }

    char *output_filepath = shift(&argc, &argv);

    if (output_filepath == NULL) {
        usage(stderr, program_name, "please, specify the output file name");

        return 1;
    }

    printf("Minifying %s as %s\n", input_filepath, output_filepath);

    Cson *cson = cson_load(input_filepath);

    if (cson == NULL) return 1;

    FILE* output = fopen(output_filepath, "w");

    minify_json(output, cson->root, true);

    fclose(output);

    return 0;
}
