#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./include/lexer.h"
#include "include/common.h"

void error_message(Cson_Lexer *lexer, const char *const error, const char *const description, ...) {
    va_list args;
    va_start(args, description);

    int context_size;
    int max_context_size = 50;

    char *context = lexer->content + lexer->cursor;

    for (context_size = 0; context_size < max_context_size && lexer->cursor + context_size < lexer->content_len; ++context_size, ++context) {
        if (*context == '\n') {
            --context_size;
            break;
        }
    }

    char *line = lexer->content + lexer->cursor - (lexer->col - 1);
    int beginning_of_the_token = lexer->cursor - lexer->bot - lexer->col + 1;
    int col = lexer->col - lexer->cursor + lexer->bot;

    fprintf(stderr, "%s:%d:%d %s\n\n", lexer->filename, lexer->line, col, error);
    fprintf(stderr, "%.*s\n", lexer->col + context_size, line);
    fprintf(stderr, "%*.s^\n", beginning_of_the_token, "");
    fprintf(stderr, "%*.s", beginning_of_the_token, "");
    vfprintf(stderr, description, args);
    fprintf(stderr, "\n");

    va_end(args);
}

Cson_Lexer *new_cson_lexer(const char *filename, char *content, const unsigned long content_size) {
    Cson_Lexer* lexer = malloc(sizeof(Cson_Lexer));

    if (lexer == NULL) {
        fprintf(stderr, "could not allocate memory for the lexer due to: %s\n", strerror(errno));
        exit(1);
    }

    lexer->filename = filename;
    lexer->line = 1;
    lexer->col = 1;

    lexer->content = content;
    lexer->content_len = content_size;
    lexer->tokens_len = 0;
    lexer->bot = 0;
    lexer->cursor = 0;
    lexer->has_error = false;
    lexer->root = NULL;
    lexer->tail = NULL;

    return lexer;
}

char chr(Cson_Lexer *lexer_cson) {
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
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void next(Cson_Lexer* lexer_cson, const int with) {
    if (lexer_cson->cursor < lexer_cson->content_len) {
        if (lexer_cson->content[lexer_cson->cursor] == '\n') {
            ++lexer_cson->line;
            lexer_cson->col = 1;
        } else {
            lexer_cson->col += with;
        }

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
        fprintf(stderr, "ERROR: could not allocate memory for the token kind %s due to: %s\n", tk_kind_display(kind), strerror(errno));
        exit(1);
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

    lexer_cson->tokens_len++;

    next(lexer_cson, size);
}

void save_string(Cson_Lexer* lexer_cson) {
    next(lexer_cson, 1);

    while (chr(lexer_cson) != '"') {
        if (chr(lexer_cson) == '\\')  {
            switch (nchr(lexer_cson)) {
                case '"':
                case '\\':
                case 'n':
                case 't':
                case 'b':
                case 'r':
                case 'f':
                    next(lexer_cson, 1);
                    break;
                default:
                    error_message(lexer_cson, "invalid escape string", "\"\\%c\" is not a valid scape string", nchr(lexer_cson));
                    exit(1);
            }
        }

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

    bool isFloating = false;

    if (nchr(lexer_cson) == '.') {
        next(lexer_cson, 1);

        while (is_digit(nchr(lexer_cson))) {
            next(lexer_cson, 1);
        }

        isFloating = true;
    }

    if (isFloating) {
        save_token(lexer_cson, FLOAT_CSON_TOKEN);
    } else {
        save_token(lexer_cson, INTEGER_CSON_TOKEN);
    }

    next(lexer_cson, 1);
}

void save_symbol(Cson_Lexer* lexer_cson) {
    while (is_valid_symbol_char(nchr(lexer_cson))) {
        next(lexer_cson, 1);
    }

    if (strncmp(&lexer_cson->content[lexer_cson->bot], "true", 4) == 0) {
        save_token(lexer_cson, TRUE_CSON_TOKEN);
    } else if (strncmp(&lexer_cson->content[lexer_cson->bot], "false", 5) == 0) {
        save_token(lexer_cson, FALSE_CSON_TOKEN);
    } else if (strncmp(&lexer_cson->content[lexer_cson->bot], "null", 4) == 0) {
        save_token(lexer_cson, NULL_CSON_TOKEN);
    } else {
        error_message(lexer_cson, "invalid symbol", "\"%.*s\" is not a valid symbol", lexer_cson->cursor - lexer_cson->bot + 1, lexer_cson->content + lexer_cson->bot);
        exit(1);
    }

    next(lexer_cson, 1);
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
            case '[': save_token_advance(lexer_cson, LSQUARE_CSON_TOKEN); break;
            case ']': save_token_advance(lexer_cson, RSQUARE_CSON_TOKEN); break;
            case ':': save_token_advance(lexer_cson, COLON_CSON_TOKEN); break;
            case ',': save_token_advance(lexer_cson, COMMA_CSON_TOKEN); break;
            case '0': case '1': case '2':
            case '3': case '4': case '5':
            case '6': case '7': case '8':
            case '9': save_number(lexer_cson); break;
            case '"': save_string(lexer_cson); break;
            case 'a'...'z':
            case 'A'...'Z':
                save_symbol(lexer_cson); break;
            case ' ':
            case '\n': next(lexer_cson, 1); break;
            default:
                lexer_cson->has_error = true;
                error_message(lexer_cson, "unrecognized character", "unrecognized character '%c'", c);
                exit(1);
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

void cson_lexer_free(Cson_Lexer* lexer_cson) {
    Cson_Token* current = lexer_cson->root;

    while (current != NULL) {
        Cson_Token* next = current->next;
        free(current);
        current = next;
    }

    free(lexer_cson->content);
    free(lexer_cson);
}
