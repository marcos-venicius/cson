#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./include/lexer.h"

void invalid_char_error(Cson_Lexer* lexer_cson) {
    lexer_cson->has_error = true;

    const int symbol_size = lexer_cson->cursor - lexer_cson->bot + 1;
    const char* symbol_start_pointer = &lexer_cson->content[lexer_cson->bot];

    fprintf(stderr, "invalid char near \"%.*s\"\n", symbol_size, symbol_start_pointer);
}

char chr(const Cson_Lexer* lexer_cson) {
    if (lexer_cson->cursor < lexer_cson->content_len) {
        return lexer_cson->content[lexer_cson->cursor];
    }

    return '\0';
}

char nchr(const Cson_Lexer* lexer_cson) {
    if (lexer_cson->cursor < lexer_cson->content_len - 1) {
        return lexer_cson->content[lexer_cson->cursor + 1];
    }

    return '\0';
}

bool is_digit(const char c) {
    return c >= '0' && c <= '9';
}

bool is_valid_symbol_char(const char c) {
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

void next(Cson_Lexer* lexer_cson, const int with) {
    if (lexer_cson->cursor < lexer_cson->content_len) {
        lexer_cson->cursor += with;
    }
}

void append_token(Cson_Lexer* lexer_cson, Cson_Token* token) {
    if (lexer_cson->tail == NULL) {
        lexer_cson->root = token;
        lexer_cson->tail = lexer_cson->root;
    } else {
        lexer_cson->tail->next = token;
        lexer_cson->tail = token;
    }
}

int save_token_chunk(Cson_Lexer* lexer_cson, const Cson_Token_Kind kind, const int from, const int to) {
    Cson_Token* token = malloc(sizeof(Cson_Token));

    if (token == NULL) {
        perror("could not allocate memory to save the token");
        return -1;
    }

    token->kind = kind;
    token->value = &lexer_cson->content[from];
    token->value_len = to;
    token->next = NULL;

    if (lexer_cson->tail == NULL) {
        lexer_cson->root = token;
        lexer_cson->tail = lexer_cson->root;
    } else {
        lexer_cson->tail->next = token;
        lexer_cson->tail = token;
    }

    lexer_cson->tokens_len++;

    return token->value_len;
}

int save_token(Cson_Lexer* lexer_cson, const Cson_Token_Kind kind) {
    return save_token_chunk(lexer_cson, kind, lexer_cson->bot, lexer_cson->cursor - lexer_cson->bot + 1);
}

void save_token_advance(Cson_Lexer* lexer_cson, const Cson_Token_Kind kind) {
    const int size = save_token(lexer_cson, kind);

    if (size == -1) {
        return;
    }

    next(lexer_cson, size);
}

void save_string(Cson_Lexer* lexer_cson) {
    next(lexer_cson, 1);

    while (chr(lexer_cson) != '"') {
        next(lexer_cson, 1);
    }

    save_token_chunk(
        lexer_cson,
        STRING_CSON_TOKEN,
        lexer_cson->bot + 1,
        lexer_cson->cursor - lexer_cson->bot - 1);

    next(lexer_cson, 1);
}

void save_number(Cson_Lexer* lexer_cson) {
    while (is_digit(nchr(lexer_cson))) {
        next(lexer_cson, 1);
    }

    if (nchr(lexer_cson) == '.') {
        next(lexer_cson, 1);

        while (is_digit(nchr(lexer_cson))) {
            next(lexer_cson, 1);
        }
    }

    if (nchr(lexer_cson) == '.') {
        invalid_char_error(lexer_cson);
        return;
    }

    save_token(lexer_cson, NUMBER_CSON_TOKEN);

    next(lexer_cson, 1);
}

void save_symbol(Cson_Lexer* lexer_cson, const char* symbol, const Cson_Token_Kind kind) {
    while (is_valid_symbol_char(nchr(lexer_cson))) {
        next(lexer_cson, 1);
    }

    if (strncmp(&lexer_cson->content[lexer_cson->bot], symbol, lexer_cson->cursor - lexer_cson->bot + 1) != 0) {
        invalid_char_error(lexer_cson);
        return;
    }

    save_token(lexer_cson, kind);

    next(lexer_cson, 1);
}

void save_true(Cson_Lexer* lexer_cson) {
    save_symbol(lexer_cson, "true", TRUE_CSON_TOKEN);
}

void save_false(Cson_Lexer* lexer_cson) {
    save_symbol(lexer_cson, "false", FALSE_CSON_TOKEN);
}

void save_null(Cson_Lexer* lexer_cson) {
    save_symbol(lexer_cson, "null", NULL_CSON_TOKEN);
}

void tokenize(Cson_Lexer* lexer_cson) {
    while (!lexer_cson->has_error) {
        lexer_cson->bot = lexer_cson->cursor;

        const char c = chr(lexer_cson);

        if (c == '\0') {
            save_token(lexer_cson, EOF_CSON_TOKEN);
            break;
        }

        switch (c) {
            case '{': save_token_advance(lexer_cson, LBRACE_CSON_TOKEN); break;
            case '}': save_token_advance(lexer_cson, RBRACE_CSON_TOKEN); break;
            case ':': save_token_advance(lexer_cson, COLON_CSON_TOKEN); break;
            case ',': save_token_advance(lexer_cson, COMMA_CSON_TOKEN); break;
            case '0': case '1': case '2':
            case '3': case '4': case '5':
            case '6': case '7': case '8':
            case '9': save_number(lexer_cson); break;
            case 't': save_true(lexer_cson); break;
            case 'f': save_false(lexer_cson); break;
            case 'n': save_null(lexer_cson); break;
            case '"': save_string(lexer_cson); break;
            case ' ':
            case '\n': next(lexer_cson, 1); break;
            default:
                lexer_cson->has_error = true;
                fprintf(stderr, "unrecognized character \"%c\"\n", c);
                return;
        }
    }
}

void lex(Cson_Lexer* cson_lexer) {
    Cson_Token* current = cson_lexer->root;

    cson_lexer->root = NULL;
    cson_lexer->tail = NULL;

    while (current != NULL) {
        if (current->kind == STRING_CSON_TOKEN && current->next->kind == COLON_CSON_TOKEN) {
            current->kind = KEY_CSON_TOKEN;
        }

        append_token(cson_lexer, current);

        current = current->next;
    }
}

void cson_lexer_free(const Cson_Lexer* lexer_cson) {
    Cson_Token* current = lexer_cson->root;

    while (current != NULL) {
        Cson_Token* next = current->next;
        free(current);
        current = next;
    }
}
