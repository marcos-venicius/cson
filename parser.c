#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "./include/parser.h"
#include "./include/common.h"

void unexpected_token_error(const Cson_Token* token, const Cson_Token_Kind kind) {
    if (token == NULL) {
        fprintf(stderr, "missing expected \"%s\"\n", tk_kind_display(kind));
    } else {
        fprintf(stderr, "invalid \"%.*s\" expected \"%s\"\n", token->value_len, token->value, tk_kind_display(kind));
    }
}

Parser* init_parser(const Cson_Lexer* lexer_cson) {
    Parser* parser = malloc(sizeof(Parser) + lexer_cson->tokens_len * sizeof(KeyPair));

    if (parser == NULL) {
        perror("could not allocate memory to init the parser");

        return NULL;
    }

    parser->size = 0;
    parser->root = lexer_cson->root;

    return parser;
}

Cson_Token* next_token(Parser* parser) {
    if (parser->root->kind == EOF_CSON_TOKEN) {
        return NULL;
    }

    parser->root = parser->root->next;

    return parser->root;
}

bool is(const Cson_Token* token, const int size, ...) {
    if (token == NULL) {
        return false;
    }

    va_list args;
    va_start(args, size);

    for (int i = 0; i < size; i++) {
        const Cson_Token_Kind kind = va_arg(args, Cson_Token_Kind);

        if (token->kind == kind) {
            va_end(args);
            return true;
        }
    }

    va_end(args);

    return false;
}

int parse_expression(Parser* parser, KeyPair** pair) {
    const Cson_Token* left = next_token(parser);

    if (!is(left, 1, STRING_CSON_TOKEN)) {
        unexpected_token_error(left, STRING_CSON_TOKEN);
        return -1;
    }

    const Cson_Token* middle = next_token(parser);

    if (!is(middle, 1, COLON_CSON_TOKEN)) {
        unexpected_token_error(middle, COLON_CSON_TOKEN);
        return -1;
    }

    const Cson_Token* right = next_token(parser);

    if (!is(right, 5, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN)) {
        unexpected_token_error(right, STRING_CSON_TOKEN);
        return -1;
    }

    *pair = (KeyPair*)malloc(sizeof(KeyPair));

    if (*pair == NULL) {
        fprintf(stderr, "could not allocate memory for key/pair");

        return -1;
    }

    (*pair)->key = left->value;
    (*pair)->key_len = left->value_len;

    (*pair)->value = right->value;
    (*pair)->value_len = right->value_len;

    (*pair)->kind = right->kind;

    next_token(parser);

    return 0;
}

Parser* parse(Cson_Lexer* lexer_cson) {
#if DEBUG
    printf("\n\nParsing:\n\n");
#endif

    Parser* parser = init_parser(lexer_cson);

    if (parser == NULL) {
        return NULL;
    }

    const Cson_Token* current = lexer_cson->root;

    if (is(current, 1, EOF_CSON_TOKEN)) {
        return parser;
    }

    if (!is(current, 1, LBRACE_CSON_TOKEN)) {
        unexpected_token_error(current, LBRACE_CSON_TOKEN);
        return NULL;
    }

    if (is(current, 1, RBRACE_CSON_TOKEN)) {
        return parser;
    }

    while (!is(parser->root, 1, EOF_CSON_TOKEN)) {
        if (is(parser->root, 1, RBRACE_CSON_TOKEN) || is(parser->root->next, 1, RBRACE_CSON_TOKEN)) {
            next_token(parser);
            continue;
        }

        KeyPair* pair;
        
        const int result = parse_expression(parser, &pair);

        if (result == -1) {
            exit(1);
        }

        parser->pairs[parser->size] = *pair;
        parser->size++;

#if DEBUG
        printf("%s %.*s %.*s\n", tk_kind_display(pair->kind), pair->key_len, pair->key, pair->value_len, pair->value);
#endif
    }

    return parser;
}
