#ifndef CSON_H_
#define CSON_H_

#include "./lexer.h"
#include "./st_parser.h"
#include <stdio.h>

typedef enum {
    CRC_NOT_FOUND = 404,
    CRC_INVALID_TYPE = 4151,
    CRC_INVALID_POS = 4152,
    CRC_INVALID_FORMAT = 400,
    CRC_OK = 200
} CsonReturnCode;

typedef struct {
    SyntaxTree *ast;
    SyntaxTreeNode *root;
} Cson;

typedef struct {
    const SyntaxTreeNode *const node;
    CsonReturnCode return_code;
} CsonItem;

Cson *cson_load(const char *filepath);

// Usage like: create_path(cson, "%s%s%d%s", "foo", "bar", 0, "bazz");
// This will match: { foo: { bar: [ { bazz: 10 } ] } }
CsonItem cson_get(const SyntaxTreeNode *node, char *format, ...);
void cson_format(Cson *cson, unsigned int padding);
void minify_json(FILE *stream, SyntaxTreeNode *node, bool is_last);

char *cson_unwrap_string(CsonItem item);
bool cson_unwrap_boolean(CsonItem item);
long double cson_unwrap_float(CsonItem item);
long cson_unwrap_integer(CsonItem item);

const char* return_code_as_cstr(CsonReturnCode code);

void cson_free(Cson* cson);

#endif // CSON_H_
