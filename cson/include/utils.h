#ifndef CSON_UTILS_H_
#define CSON_UTILS_H_

#include <stddef.h>
#include <stdbool.h>

char* scape_sequence(char* string);
bool cmp_sized_string(const char *a, const char *b, size_t as, size_t bs);

#endif // !CSON_UTILS_H_
