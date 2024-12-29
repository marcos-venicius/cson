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

SyntaxTree *parse_syntax_tree(Cson_Lexer *cson);
SyntaxTree *init_syntax_tree_parser(Cson_Lexer *lexer);
void syntax_tree_parse(SyntaxTree *st);

#endif

/**

SyntaxTree {
    root = SyntaxTreeNode {
        kind = OBJECT,
        value = SyntaxTreeNodeValue {
            object = [
                SyntaxTreeNode {
                    name = "nome",
                    kind = NUMBER,
                    value = SyntaxTreeNodeValue {
                        number = 1
                    }
                },
                SyntaxTreeNode {
                    name = "test",
                    kind = ARRAY,
                    value = SyntaxTreeNodeValue {
                        array = [
                            SyntaxTreeNode {
                                kind = NUMBER,
                                value = SyntaxTreeNodeValue {
                                    number = 1
                                }
                            },
                            SyntaxTreeNode {
                                kind = NUMBER,
                                value = SyntaxTreeNodeValue {
                                    number = 2
                                }
                            },
                            SyntaxTreeNode = {
                                kind = OBJECT,
                                value = SyntaxTreeNodeValue {
                                    object = [
                                        SyntaxTreeNode {
                                            kind = NUMBER,
                                            value = SyntaxTreeNodeValue {
                                                number = 23
                                            }
                                        }
                                    ]
                                }
                            }
                        ]
                    }
                }
            ]
        }
    }
}

*/
