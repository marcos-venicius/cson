#include "./cson/include/cson.h"
#include "./cson/include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(FILE *stream, const char *program_name, const char *error) {
    fprintf(stream, "Usage: %s [minify|format] <input-file> [flags]\n", program_name);
    fprintf(stream, "\n");
    fprintf(stream, "  minify\n");
    fprintf(stream, "    -o        output file (optional) when omitted, output to the stdout\n");
    fprintf(stream, "  format\n");
    fprintf(stream, "    -t        amount of spaces as tab (optional) (default is 2)\n");

    if (error != NULL) {
        fprintf(stream, "\nerror: %s\n", error);
    }

    fprintf(stream, "\n");
}

char *shift(int *argc, char ***argv) {
    if ((*argc)-- == 0) return NULL;

    return *((*argv)++);
}

typedef enum {
    C_MINIFY = 1,
    C_FORMAT
} Command;

int minify_command(const char *program_name, const char *input_filepath, int argc, char **argv) {
    char *flag = shift(&argc, &argv);
    char *output_filepath = NULL;

    if (flag != NULL) {
        if (!cmp_sized_string(flag, "-o", strlen(flag), 2)) {
            usage(stderr, program_name, "unrecognized flag");

            return 1;
        }

        output_filepath = shift(&argc, &argv);

        if (output_filepath == NULL) {
            usage(stderr, program_name, "please, specify the output file name");

            return 1;
        }
    }

    Cson *cson = cson_load(input_filepath);

    if (cson == NULL) return 1;

    if (output_filepath != NULL) {
        FILE* output = fopen(output_filepath, "w");

        minify_json(output, cson->root, true);

        fclose(output);
    } else {
        minify_json(stdout, cson->root, true);
    }

    return 0;
}

int format_command(const char *program_name, const char *input_filepath, int argc, char **argv) {
    const char *flag = shift(&argc, &argv);
    size_t tab_size = 2;

    if (flag != NULL) {
        if (!cmp_sized_string(flag, "-t", strlen(flag), 2)) {
            usage(stderr, program_name, "unrecognized flag");

            return 1;
        }

        char *tab_size_string = shift(&argc, &argv);

        if (tab_size_string == NULL) {
            usage(stderr, program_name, "missing tab size");

            return 1;
        }

        tab_size = strtoul(tab_size_string, NULL, 10);
    }

    if (tab_size > 16) {
        usage(stderr, program_name, "I could be wrong, but... isn't more-than-16-space-tab-size weird?");

        return 1;
    }

    Cson *cson = cson_load(input_filepath);

    if (cson == NULL) return 1;

    cson_format(cson, tab_size);

    return 0;
}

int main(int argc, char **argv) {
    char *program_name = shift(&argc, &argv);

    char *command_string = shift(&argc, &argv);

    if (command_string == NULL) {
        usage(stderr, program_name, "please specify one of the available commands");
        return 1;
    }

    size_t command_string_size = strlen(command_string);

    Command command;

    if (cmp_sized_string(command_string, "minify", command_string_size, 6)) {
        command = C_MINIFY;
    } else if (cmp_sized_string(command_string, "format", command_string_size, 6)) {
        command = C_FORMAT;
    } else {
        usage(stderr, program_name, "invalid command! please specify one of the available commands");

        return 1;
    }

    char *input_filepath = shift(&argc, &argv);

    if (input_filepath == NULL) {
        usage(stderr, program_name, "please specify the path to the json file");
        return 1;
    }

    switch (command) {
        case C_MINIFY: return minify_command(program_name, input_filepath, argc, argv);
        case C_FORMAT: return format_command(program_name, input_filepath, argc, argv);
    }
}
