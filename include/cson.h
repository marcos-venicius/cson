#ifndef CSON_H_
#define CSON_H_

#include "lexer.h"
#include "parser.h"

Parser* cson_load(char* filepath);

char* cson_read_string(Parser* cson, char* key);
bool cson_read_bool(Parser* cson, char* key);
double cson_read_double(Parser* cson, char* key);

void cson_end(Cson* cson);

#endif // CSON_H_
