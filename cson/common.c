#include "./include/common.h"

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
