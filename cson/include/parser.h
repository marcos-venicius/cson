#ifndef CSON_PARSE_H_
#define CSON_PARSE_H_
#include "./lexer.h"

typedef struct {
  Cson_Token_Kind kind;
  char* key;
  int key_len;
  char* value;
} KeyPair;

typedef struct {
  int size;
  Cson_Token* root;
  KeyPair pairs[];
} Parser;

Parser* parse(Cson_Lexer* cson);

#endif // CSON_PARSE_H_
