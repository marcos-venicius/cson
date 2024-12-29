#include "cson/include/io.h"
#include "cson/include/st_parser.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* content;

    const unsigned long size = read_file_content("./examples/basic.json", &content);

    if (size == 0) {
        return 0;
    }

    Cson_Lexer* lexer = malloc(sizeof(Cson_Lexer));

    if (lexer == NULL) {
        free(content);

        perror("could not allocate memory to the lexer struct");

        return 1;
    }

    lexer->content = content;
    lexer->content_len = size;
    lexer->tokens_len = 0;
    lexer->bot = 0;
    lexer->cursor = 0;
    lexer->has_error = false;
    lexer->root = NULL;
    lexer->tail = NULL;

    tokenize(lexer);
    lex(lexer);

    SyntaxTree *st = init_syntax_tree_parser(lexer);

    syntax_tree_parse(st);

    cson_lexer_free(lexer);
}
