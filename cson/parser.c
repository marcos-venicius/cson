#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "./include/common.h"
#include "./include/parser.h"
#include "include/lexer.h"

void parse_array(Parser* parser, char* prefix);
void parse_object(Parser* parser, char* prefix);

void add_pair(Parser* parser, KeyPair* pair) {
    parser->pairs[parser->size] = *pair;
    parser->size++;
    
    free(pair);
}


char* unescape_sequence(char* string) {
    int len = (int)strlen(string);
    int string_i = 0;
    int scape_i = 0;
    char* unescaped = malloc(len * sizeof(char));

    while (string_i < len) {
        if (string[string_i] == '\\') {
            switch (string[string_i + 1]) {
                case '"':
                    unescaped[scape_i] = '"';
                    break;
                case '\\':
                    unescaped[scape_i] = '\\';
                    break;
                case 'n':
                    unescaped[scape_i] = '\n';
                    break;
                case 't':
                    unescaped[scape_i] = '\t';
                    break;
                case 'b':
                    unescaped[scape_i] = '\t';
                    break;
                case 'r':
                    unescaped[scape_i] = '\r';
                    break;
                case 'f':
                    unescaped[scape_i] = '\f';
                    break;
                default:
                    fprintf(stderr, "invalid espace string \"%c%c\"\n", string[string_i], string[string_i + 1]);
                    exit(1);
            }
            string_i++;
        } else {
            unescaped[scape_i] = string[string_i];
        }

        string_i++;
        scape_i++;
    }

    free(string);

    return unescaped;
}

KeyPair* new_pair(char* key, char* value, Cson_Token_Kind kind) {
    KeyPair* pair = malloc(sizeof(KeyPair));

    if (pair == NULL) {
        fprintf(stderr, "could not allocate memory for key/pair");

        exit(1);
    }

    pair->key = key;
    pair->key_len = (int)strlen(key);

    if (kind == STRING_CSON_TOKEN) {
        pair->value = unescape_sequence(value);
    } else {
        pair->value = value;
    }

    pair->kind = kind;

    return pair;
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

char* nested_object_key(const char* obj_one_key, const size_t obj_one_key_size, char* obj_two_key, const int obj_two_key_size) {
    char* separator = ".";

    const int join_string_size = (int)strlen(separator);

    int final_size = (int)obj_one_key_size + join_string_size + obj_two_key_size;

    if (obj_one_key_size != 0) {
        final_size++;
    }

    char* result = malloc(final_size * sizeof(char));

    if (obj_one_key_size == 0 && obj_two_key_size == 0) {
        snprintf(result, final_size, "%s", separator);
    } else if (obj_one_key_size == 0) {
        snprintf(result, final_size, "%s", obj_two_key);
    } else if (obj_two_key_size == 0) {
        snprintf(result, final_size, "%s%s", obj_one_key, separator);
    } else {
        snprintf(result, final_size, "%s%s%s", obj_one_key, separator, obj_two_key);
    }

    return result;
}

char* nested_array_key(int index, const char* obj_one_key, const size_t obj_one_key_size, char* obj_two_key, const int obj_two_key_size) {
    // TODO: I don't think we will have arrays with postions greater than 8 digits
    // but, it should be improve, maybe
    char key[12];

    if (obj_one_key_size == 0 && obj_two_key_size == 0) {
        sprintf(key, "[%d]", index);
    } else if (obj_one_key_size == 0) {
        sprintf(key, "[%d]", index);
    } else if (obj_two_key_size == 0) {
        sprintf(key, ".[%d]", index);
    } else {
        sprintf(key, ".[%d].", index);
    }

    const int join_string_size = (int)strlen(key) + 1;

    int final_size = (int)obj_one_key_size + join_string_size + obj_two_key_size;

    char* result = malloc(final_size * sizeof(char));

    if (obj_one_key_size == 0 && obj_two_key_size == 0) {
        snprintf(result, final_size, "%s", key);
    } else if (obj_one_key_size == 0) {
        snprintf(result, final_size, "%s", obj_two_key);
    } else if (obj_two_key_size == 0) {
        snprintf(result, final_size, "%s%s", obj_one_key, key);
    } else {
        snprintf(result, final_size, "%s%s%s", obj_one_key, key, obj_two_key);
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

void parse_object(Parser* parser, char* prefix) {
    while (true) {
        Cson_Token* left = next_token(parser);

        if (is(left, RBRACE_CSON_TOKEN, -1)) {
            break;
        }

        if (is(left, COMMA_CSON_TOKEN, -1)) {
            continue;
        }

        if (!is(left, KEY_CSON_TOKEN, -1)) {
            fprintf(stderr, "1 unexpected token of type %s\n", tk_kind_display(left->kind));

            exit(1);
        }

        Cson_Token* middle = next_token(parser);

        if (!is(middle, COLON_CSON_TOKEN, -1)) {
            fprintf(stderr, "2 unexpected token of type %s\n", tk_kind_display(middle->kind));

            exit(1);
        }

        Cson_Token* right = next_token(parser);

        if (is(right, LBRACE_CSON_TOKEN, -1)) {
            char* key = nested_object_key(prefix, strlen(prefix), left->value, left->value_len);

            parse_object(parser, key);
            continue;
        }

        if (is(right, LSQUARE_CSON_TOKEN, -1)) {
            char* key = nested_object_key(prefix, strlen(prefix), left->value, left->value_len);

            parse_array(parser, key);
            continue;
        }
        
        if (!is(right, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN, -1)) {
            fprintf(stderr, "3 unexpected token of type %s\n", tk_kind_display(right->kind));

            exit(1);
        }

        const size_t prefix_len = strlen(prefix);
        char* key = nested_object_key(prefix, prefix_len, left->value, left->value_len);
        char* value = malloc(right->value_len + 1);

        snprintf(value, right->value_len + 1, "%s", right->value);

        KeyPair* pair = new_pair(key, value, right->kind);

        add_pair(parser, pair);
    }
}

void parse_array(Parser* parser, char* prefix) {
    int current_array_index = 0;

    while (true) {
        Cson_Token* next = next_token(parser);

        if (is(next, STRING_CSON_TOKEN, NUMBER_CSON_TOKEN, NULL_CSON_TOKEN, FALSE_CSON_TOKEN, TRUE_CSON_TOKEN, -1)) {
            char* key = nested_array_key(current_array_index, prefix, strlen(prefix), NULL, 0);
            char* value = malloc(next->value_len + 1);

            snprintf(value, next->value_len + 1, "%s", next->value);

            KeyPair* pair = new_pair(key, value, next->kind);

            add_pair(parser, pair);
        } else if (is(next, COMMA_CSON_TOKEN, -1)) {
            current_array_index++;
        } else if (is(next, RSQUARE_CSON_TOKEN, -1)) {
            break;
        } else if (is(next, LBRACE_CSON_TOKEN, -1)) {
            char* key = nested_array_key(current_array_index, prefix, strlen(prefix), NULL, 0);
            parse_object(parser, key);
        } else if (is(next, LSQUARE_CSON_TOKEN, -1)) {
            char* key = nested_array_key(current_array_index, prefix, strlen(prefix), NULL, 0);
            parse_array(parser, key);
        }
    }
}

Parser* parse(Cson_Lexer* lexer_cson) {
    Parser* parser = init_parser(lexer_cson);

    if (parser == NULL) {
        fprintf(stderr, "could not initialize the parser");
        exit(1);
    }

    bool parsing = true;

    while (parser->root->kind != EOF_CSON_TOKEN && parsing) {
        switch (parser->root->kind) {
            case LBRACE_CSON_TOKEN:
                parse_object(parser, "");
                break;
            case LSQUARE_CSON_TOKEN:
                parse_array(parser, "");
                break;
            case RBRACE_CSON_TOKEN:
            case RSQUARE_CSON_TOKEN:
                parsing = false;
                break;
            default:
                fprintf(stderr, "unhandled token kind %s\n", tk_kind_display(parser->root->kind));
                exit(1);
        }
    }

    return parser;
}
