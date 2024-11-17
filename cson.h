#ifndef CSON_H_
#define CSON_H_

#include "lexer.h"

Cson* cson_load(char* filepath);

char* cson_read_string(Cson* cson, char* key);
bool cson_read_bool(Cson* cson, char* key);
double cson_read_double(Cson* cson, char* key);

void cson_end(Cson* cson);

#endif // CSON_H_
