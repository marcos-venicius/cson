#ifndef CSON_LEXER_H_
#define CSON_LEXER_H_

#include <stdbool.h>

typedef enum {
	// special chars
	LBRACE_CSON_TOKEN = 0, // this is required to be able to know the amount of tokens we have just by seeing the EOF_CSON_TOKEN
	RBRACE_CSON_TOKEN,
	LSQUARE_CSON_TOKEN,
	RSQUARE_CSON_TOKEN,
	COLON_CSON_TOKEN,
	COMMA_CSON_TOKEN,

	KEY_CSON_TOKEN,

	// types
	STRING_CSON_TOKEN,
	NUMBER_CSON_TOKEN,
	NULL_CSON_TOKEN,
	TRUE_CSON_TOKEN,
	FALSE_CSON_TOKEN,

  // this eof token should be the last one
	EOF_CSON_TOKEN
} Cson_Token_Kind;

#define TOKENS_COUNT EOF_CSON_TOKEN

typedef struct Cson_Token Cson_Token;

struct Cson_Token {
	Cson_Token_Kind kind;
	char* value;
	int value_len;
	Cson_Token* next;
};

typedef struct {
	char* content;
	unsigned long content_len;
	unsigned long cursor;
	int bot;
	bool has_error;
	int tokens_len;
	Cson_Token* root;
	Cson_Token* tail;
} Cson_Lexer;

void cson_lexer_free(Cson_Lexer* lexer_cson);

void tokenize(Cson_Lexer* lexer_cson);
void lex(Cson_Lexer* cson_lexer);

#endif // CSON_LEXER_H_
