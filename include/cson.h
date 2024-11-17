#ifndef CSON_H_
#define CSON_H_

#include "lexer.h"
#include "parser.h"

#define NOT_FOUND_RETURN (-4)
#define INVALID_TYPE_RETURN (-1)
#define OK_RETURN 0

Parser* cson_load(char* filepath);

char* error_explain(int code);
int cson_read_string(Parser* cson, char* key, char** output);
int cson_read_double(Parser* cson, char* key, double* output);
int cson_read_bool(Parser* cson, char* key, bool* output);

void cson_end(Cson* cson);

#endif // CSON_H_
