#include <stdlib.h>
#include <stdarg.h>
#include "./include/parser.h"
#include "./include/common.h"

void unexpected_token_error(Cson_Token* token, Cson_Token_Kind kind) {
    if (token == NULL) {
        fprintf(stderr, "missing expected \"%s\"\n", tk_kind_display(kind));
    } else {
        fprintf(stderr, "invalid \"%.*s\" expected \"%s\"\n", token->value_len, token->value, tk_kind_display(kind));
    }
}

Parser* init_parser(Cson_Lexer* cson) {
    Parser* parser = (Parser*)malloc(sizeof(Parser) + cson->tokens_len * sizeof(KeyPair));

    if (parser == NULL) {
        perror("could not allocate memory to init the parser");

        return NULL;
    }

    parser->size = 0;
    parser->root = cson->root;

    return parser;
}

Cson_Token* next_token(Parser* parser) {
    if (parser->root == NULL) {
        return NULL;
    }

    parser->root = parser->root->next;

    return parser->root;
}

bool is(Cson_Token* token, int size, ...) {
    if (token == NULL) {
        return false;
    }

    va_list args;
    va_start(args, size);

    for (int i = 0; i < size; i++) {
        Cson_Token_Kind kind = va_arg(args, Cson_Token_Kind);

        if (token->kind == kind) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

KeyPair* parse_expression(Parser* parser) {
    if (is(parser->root, 2, EOF_CSON_TOKEN, RBRACE_CSON_TOKEN)) {
        return NULL;
    }

    Cson_Token* left = next_token(parser);

    if (!is(left, 1, STRING_CSON_TOKEN)) {
        unexpected_token_error(left, STRING_CSON_TOKEN);
        return NULL;
    }

    Cson_Token* middle = next_token(parser);

    if (!is(middle, 1, COLON_CSON_TOKEN)) {
        unexpected_token_error(middle, COLON_CSON_TOKEN);
        return NULL;
    }

    Cson_Token* right = next_token(parser);

    if (!is(right, 5, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN)) {
        unexpected_token_error(right, STRING_CSON_TOKEN);
        return NULL;
    }

    KeyPair* pair = (KeyPair*)malloc(sizeof(KeyPair));

    if (pair == NULL) {
        fprintf(stderr, "could not allocate memory for key/pair");

        return NULL;
    }

    pair->key = left->value;
    pair->key_len = left->value_len;

    pair->value = right->value;
    pair->value_len = right->value_len;

    pair->kind = right->kind;

    next_token(parser);

    return pair;
}

Parser* parse(Cson_Lexer* cson) {
#if DEBUG
    printf("\n\nParsing:\n\n");
#endif

    Parser* parser = init_parser(cson);

    Cson_Token* current = cson->root;

    if (is(current, 1, EOF_CSON_TOKEN)) {
        return parser;
    }

    if (!is(current, 1, LBRACE_CSON_TOKEN)) {
        fprintf(stderr, "Unexpected \"%.*s\"\n", current->value_len, current->value);
        return parser;
    }

    if (is(current, 1, RBRACE_CSON_TOKEN)) {
        return parser;
    }

    while (!is(parser->root, 1, EOF_CSON_TOKEN)) {
        KeyPair* pair = parse_expression(parser);

        if (pair == NULL) {
            break;
        }

        parser->pairs[parser->size] = *pair;
        parser->size++;

#if DEBUG
        printf("%s %.*s %.*s\n", tk_kind_display(pair->kind), pair->key_len, pair->key, pair->value_len, pair->value);
#endif
    }

    return parser;
}
