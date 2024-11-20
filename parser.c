#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "./include/common.h"
#include "./include/parser.h"
#include "include/lexer.h"

int parse_json(Parser* parser, char* prefix);

void add_pair(Parser* parser, KeyPair* pair) {
    parser->pairs[parser->size] = *pair;
    parser->size++;
    
    free(pair);
}

char* pop_nested_key(char* key) {
    int cursor = (int)strlen(key) - 1;

    while (cursor > 0 && key[cursor] != '.' && key[cursor] != ']') {
        cursor--;
    }

    if (cursor == 0) {
        return "";
    }

    if (key[cursor] == ']') {
        while (cursor > 0 && key[cursor] != '.') {
            cursor--;
        }
    }

    if (cursor == 0) {
        return "";
    }

    cursor++;

    char* new_key = malloc(cursor * sizeof(char));
    
    if (new_key == NULL) {
        fprintf(stderr, "could not allocate memory to the pop nested key new_key\n");
        exit(1);
    }

    snprintf(new_key, cursor, "%s", key); 

    free(key);

    return new_key;
}

char* nested_key(
    const char* obj_one_key,
    const size_t obj_one_key_size,
    const char* separator,
    char* obj_two_key,
    const int obj_two_key_size) {
    int join_string_size = 0;

    if (obj_one_key_size > 0) {
        join_string_size = (int)strlen(separator);
    }

    const int final_size = (int)obj_one_key_size + join_string_size + obj_two_key_size + 1;

    char* result = malloc(final_size * sizeof(char));

    if (obj_one_key_size == 0) {
        snprintf(result, final_size, "%s", obj_two_key);
    } else if (obj_two_key_size == 0) {
        snprintf(result, final_size, "%s%s", obj_one_key, separator);
    } else {
        snprintf(result, final_size, "%s%s%s", obj_one_key, separator, obj_two_key);
    }

    return result;
}

char* nested_object_key(const char* obj_one_key, const size_t obj_one_key_size, char* obj_two_key, const int obj_two_key_size) {
    return nested_key(obj_one_key, obj_one_key_size, ".", obj_two_key, obj_two_key_size);
}

char* nested_array_key(int index, const char* obj_one_key, const size_t obj_one_key_size, char* obj_two_key, const int obj_two_key_size) {
    // TODO: I don't think we will have arrays with postions greater than 8 digits
    // but, it should be improve, maybe
    char key[12];

    if (obj_one_key_size == 0) {
        sprintf(key, ".[%d]", index);
    } else {
        sprintf(key, ".[%d].", index);
    }

    return nested_key(obj_one_key, obj_one_key_size, key, obj_two_key, obj_two_key_size);
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

// This function expects any number of tokens (Cson_Token_Kind), but it should have a -1 at the end
bool is(const Cson_Token* token, ...) {
    if (token == NULL) {
        return false;
    }

    int tokens_count = TOKENS_COUNT;
    va_list args;
    va_start(args, token);

    Cson_Token_Kind kind = va_arg(args, Cson_Token_Kind);

    while ((int)kind != -1) {
        assert(kind >= 0 && "This token kind does not exists");
        assert(kind <= EOF_CSON_TOKEN && "This token kind does not exists");

        if (token->kind == kind) {
            va_end(args);
            return true;
        }

        --tokens_count;

        assert(tokens_count >= 0 && "invalid number of tokens passed to \"is\" function or missing -1 at the end of the arguments");

        kind = va_arg(args, Cson_Token_Kind);
    }

    va_end(args);

    return false;
}

int parse_array(Parser* parser, char* prefix) {
    int opening_square_brackets = 1;
    int current_array_index = 0;

    Cson_Token* next = next_token(parser);

    while (opening_square_brackets > 0) {
        if (is(next, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN, -1)) {
            KeyPair* pair = malloc(sizeof(KeyPair));

            char* key = nested_array_key(current_array_index, prefix, strlen(prefix), NULL, 0);
            char* value = malloc(next->value_len + 1);

            snprintf(value, next->value_len + 1, "%s", next->value);

            pair->key = key;
            pair->key_len = (int)strlen(key);

            pair->value = value;

            pair->kind = next->kind;

            add_pair(parser, pair);
        } else if (is(next, COMMA_CSON_TOKEN, -1)) {
            current_array_index++;
        } else if (is(next, RSQUARE_CSON_TOKEN, -1)) {
            opening_square_brackets--;
        }

        next = next_token(parser);
    }

    return 0;
}

int parse_json(Parser* parser, char* prefix) {
    const Cson_Token* left = next_token(parser);

    // TODO: add rsquare
    if (is(left, RBRACE_CSON_TOKEN, EOF_CSON_TOKEN, -1)) {
        char* key = pop_nested_key(prefix);

        return parse_json(parser, key);
    }

    if (is(left, COMMA_CSON_TOKEN, -1)) {
        return parse_json(parser, prefix);
    }

    if (!is(left, KEY_CSON_TOKEN, -1)) {
        unexpected_token_error(left, KEY_CSON_TOKEN);
        return -1;
    }

    const Cson_Token* middle = next_token(parser);

    if (!is(middle, COLON_CSON_TOKEN, -1)) {
        unexpected_token_error(middle, COLON_CSON_TOKEN);
        return -1;
    }

    const Cson_Token* right = next_token(parser);

    if (is(right, LBRACE_CSON_TOKEN, -1)) {
        char* key = nested_object_key(prefix, strlen(prefix), left->value, left->value_len);

        return parse_json(parser, key);
    }

    if (is(right, LSQUARE_CSON_TOKEN, -1)) {
        char* key = nested_object_key(prefix, strlen(prefix), left->value, left->value_len);

        return parse_array(parser, key);
    }

    if (!is(right, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN, -1)) {
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

#if DEBUG
        printf("%s %.*s %s\n", tk_kind_display(pair->kind), pair->key_len, pair->key, pair->value);
#endif

    add_pair(parser, pair);

    if (parser->root->kind == EOF_CSON_TOKEN) return 0;

    return parse_json(parser, prefix);
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

    if (is(current, EOF_CSON_TOKEN, -1)) {
        return parser;
    }

    if (!is(current, LBRACE_CSON_TOKEN, -1)) {
        unexpected_token_error(current, LBRACE_CSON_TOKEN);
        return NULL;
    }

    if (is(current, RBRACE_CSON_TOKEN, -1)) {
        return parser;
    }

    if (parse_json(parser, "") == -1) {
        exit(1);
    }

    return parser;
}

void parser_cleanup(Parser* parser) {
    free(parser);
}
