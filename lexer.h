#ifndef CSON_LEXER_H_
#define CSON_LEXER_H_

#include <stdio.h>
#include <stdbool.h>

typedef enum {
	// special chars
	LBRACE_CSON_TOKEN,
	RBRACE_CSON_TOKEN,
	COLON_CSON_TOKEN,
	COMMA_CSON_TOKEN,

	// types
	STRING_CSON_TOKEN,
	NUMBER_CSON_TOKEN,
	NULL_CSON_TOKEN,
	TRUE_CSON_TOKEN,
	FALSE_CSON_TOKEN,

  // eof
	EOF_CSON_TOKEN,
} Cson_Token_Kind;

typedef struct {
  Cson_Token_Kind kind;
  char* value;
  int value_len;
  struct Cson_Token* next;
} Cson_Token;

typedef struct {
	char* content;
  int content_len;
  int cursor;
  int bot;
  bool has_error;
  Cson_Token* root;
  Cson_Token* tail;
} Cson;

typedef struct {
  Cson_Token_Kind kind;
  char* key;
  char* value;
  int value_len;
} KeyPair;

typedef struct {
  int size;
  KeyPair pairs[];
} KeyPairs;


void tokenize(Cson* cson);

#endif // CSON_LEXER_H_
