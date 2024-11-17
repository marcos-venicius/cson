#include <stdlib.h>
#include <stdarg.h>
#include "./include/parser.h"

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

Parser* init_parser(Cson* cson) {
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
    Cson_Token* left = next_token(parser);

    if (left == NULL || is(parser->root, 1, EOF_CSON_TOKEN)) {
        return NULL;
    }

    if (!is(left, 1, STRING_CSON_TOKEN)) {
        fprintf(stderr, "1 Unexpected \"%.*s\"\n", left->value_len, left->value);
        return NULL;
    }

    Cson_Token* middle = next_token(parser);

    if (!is(middle, 1, COLON_CSON_TOKEN)) {
        fprintf(stderr, "2 Unexpected \"%.*s\"\n", middle->value_len, middle->value);
        return NULL;
    }

    Cson_Token* right = next_token(parser);

    if (!is(right, 5, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN)) {
        fprintf(stderr, "3 Unexpected \"%.*s\"\n", right->value_len, right->value);
        return NULL;
    }

    KeyPair* pair = (KeyPair*)malloc(sizeof(KeyPair));

    pair->key = left->value;
    pair->key_len = left->value_len;

    pair->value = right->value;
    pair->value_len = right->value_len;

    pair->kind = right->kind;

    next_token(parser);

    return pair;
}

Parser* parse(Cson* cson) {
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

    while (parser->root != NULL && parser->root->kind != EOF_CSON_TOKEN) {
        KeyPair* pair = parse_expression(parser);

        if (pair == NULL) {
            break;
        }

        parser->pairs[parser->size] = *pair;
        parser->size++;

        //printf("%s %.*s %.*s\n", tk_kind_display(pair->kind), pair->key_len, pair->key, pair->value_len, pair->value);
    }

    return parser;
}
