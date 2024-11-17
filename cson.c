#include "./cson.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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

int read_file_content(const char* filename, char** output) {
    FILE* fptr = fopen(filename, "r");

    if (fptr == NULL) {
        perror("could not open file");
        return -1;
    }

    fseek(fptr, 0, SEEK_END);
    size_t file_size = ftell(fptr);
    rewind(fptr);

    // +1 due to the null terminator
    char* content = malloc(file_size + 1);

    if (content == NULL) {
        perror("could not allocate memory to read the file");
        fclose(fptr);
        return -1;
    }

    size_t read_size = fread(content, 1, file_size, fptr);

    if (read_size != file_size) {
        perror("could not read the entire file");
        free(content);
        fclose(fptr);
        return -1;
    }

    content[file_size] = '\0';

    fclose(fptr);

    *output = (char*)malloc(read_size * sizeof(char));

    if (*output != NULL) {
        strcpy(*output, content);
    } else {
        perror("Could not allocate memory for the output json");
        free(content);
        return -1;
    }

    return read_size;
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
}

void print_tokens(Cson* cson) {
    Cson_Token* current = cson->root;

    while (current != NULL) {
        printf("%s :: %.*s\n", tk_kind_display(current->kind), current->value_len, current->value);

        current = current->next;
    }
}

void cson_end(Cson* cson) {
    Cson_Token* current = cson->root;

    while (current != NULL) {
        Cson_Token* next = current->next;
        free(current);
        current = next;
    }
}

Cson* cson_load(char* filepath)
{
    assert(filepath != NULL && "input should not be null");

    char* content;

    int size = read_file_content(filepath, &content);

    if (size == -1) {
        return NULL;
    }

    Cson* cson = malloc(sizeof(Cson));

    if (cson == NULL) {
        free(content);

        perror("could not allocate memory to the cson struct");

        return NULL;
    }

    cson->content = content;
    cson->content_len = size;
    cson->bot = 0;
    cson->cursor = 0;
    cson->has_error = false;
    cson->root = NULL;
    cson->tail = NULL;

    tokenize(cson);

    return cson;
}

char* cson_read_string(Cson* cson, char* key) {
    (void)cson;
    (void)key;

    return "";
}

double cson_read_double(Cson* cson, char* key) {
    (void)cson;
    (void)key;

    return 0.f;
}

bool cson_read_bool(Cson* cson, char* key) {
    (void)cson;
    (void)key;

    return false;
}
