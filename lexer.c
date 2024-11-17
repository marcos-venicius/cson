#include <stdlib.h>
#include <string.h>
#include "./include/lexer.h"

static const char* tk_kind_display(Cson_Token_Kind kind) {
    switch (kind) {
        case LBRACE_CSON_TOKEN: return "LBRACE";
        case RBRACE_CSON_TOKEN: return "RBRACE";
        case COLON_CSON_TOKEN: return "COLON";
        case COMMA_CSON_TOKEN: return "COMMA";
        case STRING_CSON_TOKEN: return "STRING";
        case NUMBER_CSON_TOKEN: return "NUMBER";
        case NULL_CSON_TOKEN: return "NULL";
        case TRUE_CSON_TOKEN: return "TRUE";
        case FALSE_CSON_TOKEN: return "FALSE";
        case EOF_CSON_TOKEN: return "EOF";
        default: return "unknown";
    }
}

char chr(Cson* cson) {
    if (cson->cursor < cson->content_len) {
        return cson->content[cson->cursor];
    }

    return '\0';
}

char nchr(Cson* cson) {
    if (cson->cursor < cson->content_len - 1) {
        return cson->content[cson->cursor + 1];
    }

    return '\0';
}

void next(Cson* cson, int with) {
    if (cson->cursor < cson->content_len) {
        cson->cursor += with;
    }
}

int save_token(Cson* cson, Cson_Token_Kind kind) {
    Cson_Token* token = (Cson_Token*)malloc(sizeof(Cson_Token));

    if (token == NULL) {
        perror("could not allocate memory to save the token");
        return -1;
    }

    token->kind = kind;
    token->value = &cson->content[cson->bot];
    token->value_len = cson->cursor - cson->bot + 1;
    token->next = NULL;

    if (cson->tail == NULL) {
        cson->root = token;
        cson->tail = cson->root;
    } else {
        cson->tail->next = token;
        cson->tail = token;
    }

    return token->value_len;
}

void save_token_advance(Cson* cson, Cson_Token_Kind kind) {
    int size = save_token(cson, kind);

    if (size == -1) {
        return;
    }

    next(cson, size);
}

void save_string(Cson* cson) {
    next(cson, 1);

    while (chr(cson) != '"') {
        next(cson, 1);
    }

    save_token(cson, STRING_CSON_TOKEN);

    next(cson, 1);
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_valid_symbol_char(char c) {
    switch (c) {
        case 't':
        case 'r':
        case 'e':
        case 'f':
        case 'a':
        case 's':
        case 'n':
        case 'u':
        case 'l':
            return true;
        default:
            return false;
    }
}

void save_number(Cson* cson) {
    while (is_digit(nchr(cson))) {
        next(cson, 1);
    }

    if (nchr(cson) == '.') {
        next(cson, 1);

        while (is_digit(nchr(cson))) {
            next(cson, 1);
        }
    }

    if (nchr(cson) == '.') {
        cson->has_error = true;
        fprintf(stderr, "invalid number near \"%.*s\"\n", cson->cursor - cson->bot + 1, &cson->content[cson->bot]);
        return;
    }

    save_token(cson, STRING_CSON_TOKEN);

    next(cson, 1);
}

void save_symbol(Cson* cson, char* symbol, Cson_Token_Kind kind) {
    while (is_valid_symbol_char(nchr(cson))) {
        next(cson, 1);
    }

    if (strncmp(&cson->content[cson->bot], symbol, cson->cursor - cson->bot + 1) != 0) {
        fprintf(stderr, "invalid symbol %.*s", cson->cursor - cson->bot + 1, &cson->content[cson->bot]);
        cson->has_error = true;
        return;
    }

    save_token(cson, kind);

    next(cson, 1);
}

void save_true(Cson* cson) {
    save_symbol(cson, "true", TRUE_CSON_TOKEN);
}

void save_false(Cson* cson) {
    save_symbol(cson, "false", FALSE_CSON_TOKEN);
}

void save_null(Cson* cson) {
    save_symbol(cson, "null", NULL_CSON_TOKEN);
}

void print_tokens(Cson* cson) {
    Cson_Token* current = cson->root;

    while (current != NULL) {
        printf("%s :: %.*s\n", tk_kind_display(current->kind), current->value_len, current->value);

        current = current->next;
    }
}

void tokenize(Cson* cson) {
    while (!cson->has_error) {
        cson->bot = cson->cursor;

        char c = chr(cson);

        if (c == '\0') {
            save_token(cson, EOF_CSON_TOKEN);
            break;
        }

        switch (c) {
            case '{': save_token_advance(cson, LBRACE_CSON_TOKEN); break;
            case '}': save_token_advance(cson, RBRACE_CSON_TOKEN); break;
            case ':': save_token_advance(cson, COLON_CSON_TOKEN); break;
            case ',': save_token_advance(cson, COMMA_CSON_TOKEN); break;
            case '0': case '1': case '2':
            case '3': case '4': case '5':
            case '6': case '7': case '8':
            case '9': save_number(cson); break;
            case 't': save_true(cson); break;
            case 'f': save_false(cson); break;
            case 'n': save_null(cson); break;
            case '"': save_string(cson); break;
            case ' ':
            case '\n': next(cson, 1); break;
            default:
                cson->has_error = true;
                fprintf(stderr, "unrecognized character \"%c\"\n", c);
                return;
        }
    }

    print_tokens(cson);
}
