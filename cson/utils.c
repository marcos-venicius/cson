#include "./include/utils.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

char* scape_sequence(char* string) {
    if (string == NULL) return NULL;

    size_t len = strlen(string);
    size_t scape_i = 0;
    char* scaped = calloc(sizeof(char), len);
    size_t current_size = len;

    for (size_t i = 0; i < len; ++i) {
        if (scape_i >= current_size) {
            current_size = current_size + 5;

            scaped = realloc(scaped, current_size * sizeof(char));
            
            if (scaped == NULL) {
                fprintf(stderr, "Could not allocate enough memory to scape the string: %s\n", strerror(errno));
                exit(1);
            }
        }

        switch (string[i]) {
            case '\n':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = 'n';
                break;
            case '\\':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = '\\';
                break;
            case '\t':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = 't';
                break;
            case '\b':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = 'b';
                break;
            case '\r':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = 'r';
                break;
            case '\f':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = 'f';
                break;
            case '\"':
                scaped[scape_i++] = '\\';
                scaped[scape_i++] = '"';
                break;
            default:
                scaped[scape_i++] = string[i];
                break;
        }
    }

    free(string);

    return scaped;
}

bool cmp_sized_string(const char *a, const char *b, size_t as, size_t bs)
{
    if (as != bs) return false;

    for (size_t i = 0; i < as; ++i) if (a[i] != b[i]) return false;

    return true;
}
