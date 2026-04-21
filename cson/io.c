#include "./include/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long read_file_content(const char* filename, char** output) {
    FILE* fptr = fopen(filename, "r");

    if (fptr == NULL) {
        perror("could not open file");
        return 0;
    }

    fseek(fptr, 0, SEEK_END);
    const size_t file_size = ftell(fptr);
    rewind(fptr);

    // +1 due to the null terminator
    *output = malloc((file_size + 1) * sizeof(char));

    if (*output == NULL) {
        perror("could not allocate memory to read the file");
        fclose(fptr);
        return 0;
    }

    const size_t read_size = fread(*output, 1, file_size, fptr);

    if (read_size != file_size) {
        perror("could not read the entire file");
        free(*output);
        fclose(fptr);
        return 0;
    }

    (*output)[file_size] = '\0';

    fclose(fptr);

    return read_size;
}

unsigned long read_from_stdin(char **content) {
    char *line = NULL;
    unsigned long len;
    unsigned long read_size = 0;
    unsigned long content_length = 0;
    char *buffer = NULL;

    while ((read_size = getline(&line, &len, stdin)) != -1) {
        unsigned long previous_length = content_length;
        content_length += read_size;

        // TODO: handle errors correctly
        buffer = realloc(buffer, content_length);

        memcpy(buffer + previous_length, line, read_size);
    }

    if (content != NULL) *content = buffer;

    return content_length;
}

