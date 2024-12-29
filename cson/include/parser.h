#ifndef CSON_PARSE_H_
#define CSON_PARSE_H_
#include "./lexer.h"
#include "../libs/ll.h"

typedef union {
    double number;
    bool boolean;
    char* string;
} KeyPairValue;

typedef struct {
    Cson_Token_Kind kind;
    char* key;
    int key_len;

    KeyPairValue as;
} KeyPair;

typedef struct {
    int size;
    Cson_Token* root;
    KeyPair pairs[];
} Parser;

Parser* parse(Cson_Lexer* cson);

#endif // CSON_PARSE_H_
