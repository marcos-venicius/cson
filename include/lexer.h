#ifndef CSON_LEXER_H_
#define CSON_LEXER_H_

#include <stdbool.h>

typedef enum {
	// special chars
	LBRACE_CSON_TOKEN,
	RBRACE_CSON_TOKEN,
	COLON_CSON_TOKEN,
	COMMA_CSON_TOKEN,

	KEY_CSON_TOKEN,

	// types
	STRING_CSON_TOKEN,
	NUMBER_CSON_TOKEN,
	NULL_CSON_TOKEN,
	TRUE_CSON_TOKEN,
	FALSE_CSON_TOKEN,

	EOF_CSON_TOKEN,
} Cson_Token_Kind;


typedef struct Cson_Token Cson_Token;

struct Cson_Token {
	Cson_Token_Kind kind;
	char* value;
	int value_len;
	Cson_Token* next;
};

typedef struct {
	char* content;
	int content_len;
	int cursor;
	int bot;
	bool has_error;
	int tokens_len;
	Cson_Token* root;
	Cson_Token* tail;
} Cson_Lexer;

void cson_lexer_free(const Cson_Lexer* lexer_cson);

void tokenize(Cson_Lexer* lexer_cson);
void lex(Cson_Lexer* cson_lexer);

#endif // CSON_LEXER_H_
