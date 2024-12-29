#ifndef CSON_ST_PARSE_H_
#define CSON_ST_PARSE_H_

#include <stdbool.h>
#include "lexer.h"
#include "../libs/ll.h"

typedef struct SyntaxTree SyntaxTree;

typedef enum {
    STNK_OBJECT,
    STNK_ARRAY,
    STNK_STRING,
    STNK_NUMBER,
    STNK_BOOLEAN
} SyntaxTreeNodeKind;

typedef union {
    double number;
    bool boolean;
    char* string;
    // SyntaxTreeNode[]
    LL *array;
    // SyntaxTreeNode[]
    LL *object;
} SyntaxTreeNodeValue;

typedef struct {
    char *name;

    SyntaxTreeNodeKind kind;

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

