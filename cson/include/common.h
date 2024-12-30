#ifndef CSON_COMMON_H_
#define CSON_COMMON_H_

#include "./lexer.h"
#include "st_parser.h"

const char* tk_kind_display(Cson_Token_Kind kind);
void tk_display(const char *label, Cson_Token *token);
void stn_display(SyntaxTreeNode *node, int level, char *name);
const char* stnk_display(SyntaxTreeNodeKind kind);

#endif
