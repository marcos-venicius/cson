#include "./io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file_content(const char* filename, char** output) {
    FILE* fptr = fopen(filename, "r");

    if (fptr == NULL) {
        perror("could not open file");
        return -1;
    }

    fseek(fptr, 0, SEEK_END);
    size_t file_size = ftell(fptr);
    rewind(fptr);

    // +1 due to the null terminator
    char* content = malloc(file_size + 1);

    if (content == NULL) {
        perror("could not allocate memory to read the file");
        fclose(fptr);
        return -1;
    }

    size_t read_size = fread(content, 1, file_size, fptr);

    if (read_size != file_size) {
        perror("could not read the entire file");
        free(content);
        fclose(fptr);
        return -1;
    }

    content[file_size] = '\0';

    fclose(fptr);

    *output = (char*)malloc(read_size * sizeof(char));

    if (*output != NULL) {
        strcpy(*output, content);
    } else {
        perror("Could not allocate memory for the output json");
        free(content);
        return -1;
    }

    return read_size;
}
