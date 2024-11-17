#ifndef CSON_H_
#define CSON_H_

#include "lexer.h"
#include "parser.h"

#define NOT_FOUND_RETURN (-4)
#define INVALID_TYPE_RETURN (-1)
#define OK_RETURN 0

typedef struct {
    Parser* parser;
} Cson;

Cson* cson_load(char* filepath);

char* error_explain(int code);
int cson_read_string(Cson* cson, char* key, char** output);
int cson_read_double(Cson* cson, char* key, double* output);
int cson_read_bool(Cson* cson, char* key, bool* output);

#endif // CSON_H_
