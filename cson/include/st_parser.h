#ifndef CSON_ST_PARSE_H_
#define CSON_ST_PARSE_H_

#include <stdbool.h>
#include "./lexer.h"
#include "../libs/ll.h"

typedef struct SyntaxTree SyntaxTree;

typedef enum {
    STNK_OBJECT,
    STNK_ARRAY,
    STNK_STRING,
    STNK_FLOAT,
    STNK_INTEGER,
    STNK_BOOLEAN
} SyntaxTreeNodeKind;

typedef union {
    long double as_float;
    long as_integer;
    bool as_bool;
    char* as_string;
    // SyntaxTreeNode[]
    LL *as_array;
    // SyntaxTreeNode[]
    LL *as_object;
} SyntaxTreeNodeValue;

typedef struct {
    char *name;

    SyntaxTreeNodeKind kind;

    int precision; // only for floats. Should have a better way todo this.

    SyntaxTreeNodeValue value;
} SyntaxTreeNode;

struct SyntaxTree {
    Cson_Token *token;

    SyntaxTreeNode *root;
};

SyntaxTree *init_syntax_tree_parser(Cson_Lexer *lexer);
void syntax_tree_parse(SyntaxTree *st);
void syntax_tree_free(SyntaxTree *st);

#endif

