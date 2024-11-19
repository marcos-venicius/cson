#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "./include/parser.h"
#include "./include/common.h"
#include "include/lexer.h"

char* nested_object_key(const char* obj_one_key, const size_t obj_one_key_size, const char* obj_two_key, const int obj_two_key_size) {
    int join_string_size = 1;

    if (obj_one_key_size == 0) {
        join_string_size = 0;
    }

    const int final_size = (int)obj_one_key_size + join_string_size + obj_two_key_size + 1;

    char* result = malloc(final_size);

    if (obj_one_key_size == 0) {
        snprintf(result, final_size, "%s", obj_two_key);
    } else {
        snprintf(result, final_size, "%s.%s", obj_one_key, obj_two_key);
    }

    return result;
}

void unexpected_token_error(const Cson_Token* token, const Cson_Token_Kind kind) {
    if (token == NULL) {
        fprintf(stderr, "missing expected \"%s\"\n", tk_kind_display(kind));
    } else {
        fprintf(stderr, "invalid \"%.*s\" expected \"%s\"\n", token->value_len, token->value, tk_kind_display(kind));
    }
}

Parser* init_parser(const Cson_Lexer* lexer_cson) {
    const size_t parser_size = sizeof(Parser) + lexer_cson->tokens_len * sizeof(KeyPair);

    Parser* parser = malloc(parser_size);

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

int parse_expression(Parser* parser, const char* prefix) {
    const Cson_Token* left = next_token(parser);

    if (is(left, 2, RBRACE_CSON_TOKEN, EOF_CSON_TOKEN)) {
        next_token(parser);
        return 0;
    }

    if (!is(left, 1, KEY_CSON_TOKEN)) {
        unexpected_token_error(left, KEY_CSON_TOKEN);
        return -1;
    }

    const Cson_Token* middle = next_token(parser);

    if (!is(middle, 1, COLON_CSON_TOKEN)) {
        unexpected_token_error(middle, COLON_CSON_TOKEN);
        return -1;
    }

    const Cson_Token* right = next_token(parser);

    if (is(right, 1, LBRACE_CSON_TOKEN)) {
        const char* key = nested_object_key(prefix, strlen(prefix), left->value, left->value_len);

        return parse_expression(parser, key);
    }

    if (!is(right, 5, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN)) {
        unexpected_token_error(right, STRING_CSON_TOKEN);
        return -1;
    }

    KeyPair* pair = malloc(sizeof(KeyPair));

    if (pair == NULL) {
        fprintf(stderr, "could not allocate memory for key/pair");

        return -1;
    }

    const size_t prefix_len = strlen(prefix);

    char* key = nested_object_key(prefix, prefix_len, left->value, left->value_len);
    char* value = malloc(right->value_len + 1);

    snprintf(value, right->value_len + 1, "%s", right->value);

    pair->key = key;
    pair->key_len = (int)strlen(key);

    pair->value = value;

    pair->kind = right->kind;

    next_token(parser);

    parser->pairs[parser->size] = *pair;
    parser->size++;

    if (parser->root->kind == EOF_CSON_TOKEN) return 0;

    return parse_expression(parser, prefix);
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

    const int result = parse_expression(parser, "");

    if (result == -1) {
        exit(1);
    }

#if DEBUG
        printf("%s %.*s %s\n", tk_kind_display(pair->kind), pair->key_len, pair->key, pair->value);
#endif

    return parser;
}
