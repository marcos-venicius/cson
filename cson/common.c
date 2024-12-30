#include <stdio.h>
#include "./include/common.h"
#include "include/st_parser.h"
#include "libs/ll.h"

const char* stnk_display(SyntaxTreeNodeKind kind) {
    switch (kind) {
        case STNK_OBJECT:
            return "STNK_OBJECT";
        case STNK_ARRAY:
            return "STNK_ARRAY";
        case STNK_STRING:
            return "STNK_STRING";
        case STNK_NUMBER:
            return "STNK_NUMBER";
        case STNK_BOOLEAN:
            return "STNK_BOOLEAN";
        default:
            return "UNKNOWN";
    }
}

const char* tk_kind_display(const Cson_Token_Kind kind) {
    switch (kind) {
        case LBRACE_CSON_TOKEN: return "LBRACE";
        case RBRACE_CSON_TOKEN: return "RBRACE";
        case LSQUARE_CSON_TOKEN: return "LSQUARE";
        case RSQUARE_CSON_TOKEN: return "RSQUARE";
        case COLON_CSON_TOKEN: return "COLON";
        case COMMA_CSON_TOKEN: return "COMMA";
        case KEY_CSON_TOKEN: return "KEY";
        case STRING_CSON_TOKEN: return "STRING";
        case NUMBER_CSON_TOKEN: return "NUMBER";
        case NULL_CSON_TOKEN: return "NULL";
        case TRUE_CSON_TOKEN: return "TRUE";
        case FALSE_CSON_TOKEN: return "FALSE";
        case EOF_CSON_TOKEN: return "EOF";
        default: return "unknown";
    }
}

void tk_display(const char *label, Cson_Token *token) {
    const char *kind = tk_kind_display(token->kind);

    if (label == NULL) {
        printf("Token(kind=%s, value=\"%.*s\")\n", kind, token->value_len, token->value);
    } else {
        printf("Token(label=\"%s\", kind=%s, value=\"%.*s\")\n", label, kind, token->value_len, token->value);
    }
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}

void stn_display(SyntaxTreeNode *node, int level, char *name) {
    switch (node->kind) {
        case STNK_OBJECT: {
            printf("%*sSyntaxTreeNode {\n", level, "");

            if (node->name != NULL) {
                printf("%*sname = %s,\n", level + 4, "", node->name);
            } else if (name != NULL) {
                printf("%*sindex = %s,\n", level + 4, "", name);
            }

            printf("%*skind = %s,\n", level + 4, "", stnk_display(node->kind));
            printf("%*svalue = SyntaxTreeNodeValue {\n", level + 4, "");

            LLIter iter = ll_iter(node->value.object);

            while (ll_iter_has(&iter)) {
                LLIterItem item = ll_iter_consume(&iter);
                SyntaxTreeNode *child = item.data;

                stn_display(child, level + 8, NULL);
            }

            printf("%*s}\n", level + 4, "");
            printf("%*s}\n", level, "");
            break;
        }
        case STNK_STRING:
            printf("%*sSyntaxTreeNode {\n", level, "");
            if (node->name != NULL) {
                printf("%*sname = %s,\n", level + 4, "", node->name);
            } else if (name != NULL) {
                printf("%*sindex = %s,\n", level + 4, "", name);
            }
            printf("%*skind = %s,\n", level + 4, "", stnk_display(node->kind));
            printf("%*svalue = SyntaxTreeNodeValue {\n", level + 4, "");
            printf("%*sstring = \"%s\"\n", level + 8, "", node->value.string);
            printf("%*s}\n", level + 4, "");
            printf("%*s}\n", level, "");
            break;
        case STNK_ARRAY: {
            printf("%*sSyntaxTreeNode {\n", level, "");

            if (node->name != NULL) {
                printf("%*sname = %s,\n", level + 4, "", node->name);
            } else if (name != NULL) {
                printf("%*sindex = %s,\n", level + 4, "", name);
            }

            printf("%*skind = %s,\n", level + 4, "", stnk_display(node->kind));
            printf("%*svalue = SyntaxTreeNodeValue [\n", level + 4, "");

            LLIter iter = ll_iter(node->value.array);

            while (ll_iter_has(&iter)) {
                LLIterItem item = ll_iter_consume(&iter);
                SyntaxTreeNode *child = item.data;

                char index_buf[20];

                snprintf(index_buf, sizeof(index_buf), "%ld", item.index);

                stn_display(child, level + 8, index_buf);
            }

            printf("%*s]\n", level + 4, "");
            printf("%*s}\n", level, "");
            break;
        }
        case STNK_NUMBER:
            printf("%*sSyntaxTreeNode {\n", level, "");
            if (node->name != NULL) {
                printf("%*sname = %s,\n", level + 4, "", node->name);
            } else if (name != NULL) {
                printf("%*sindex = %s,\n", level + 4, "", name);
            }
            printf("%*skind = %s,\n", level + 4, "", stnk_display(node->kind));
            printf("%*svalue = SyntaxTreeNodeValue {\n", level + 4, "");
            printf("%*snumber = %f\n", level + 8, "", node->value.number);
            printf("%*s}\n", level + 4, "");
            printf("%*s}\n", level, "");
            break;
        case STNK_BOOLEAN:
            printf("%*sSyntaxTreeNode {\n", level, "");
            if (node->name != NULL) {
                printf("%*sname = %s,\n", level + 4, "", node->name);
            } else if (name != NULL) {
                printf("%*sindex = %s,\n", level + 4, "", name);
            }
            printf("%*skind = %s,\n", level + 4, "", stnk_display(node->kind));
            printf("%*svalue = SyntaxTreeNodeValue {\n", level + 4, "");
            printf("%*sboolean = %s\n", level + 8, "", node->value.boolean ? "true" : "false");
            printf("%*s}\n", level + 4, "");
            printf("%*s}\n", level, "");
            break;
    }
}
