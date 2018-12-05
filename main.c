#include "parser.h"
#include <stdio.h>
#include "error.h"

int main() {
    int res = parse();
    if (res == SYNTAX_OK) return 0;
    else return res;
}

char *values[] = {
        "KEYWORD_DEF",
        "KEYWORD_DO",
        "KEYWORD_ELSE",
        "KEYWORD_END",
        "KEYWORD_IF",
        "KEYWORD_NOT",
        "KEYWORD_NIL",
        "KEYWORD_THEN",
        "KEYWORD_WHILE",
        "ID",
        "IDF",
        "NUM_INT",
        "NUM_FLOAT",
        "NUM_EXP",
        "STRING",
        "LEX_EOL",
        "COMMA",
        "ROUNDL",
        "ROUNDR",
        "ASSIGN",
        "PLUS",
        "MINUS",
        "MUL",
        "DIV",
        "LESS",
        "MORE",
        "LESS_EQUAL",
        "MORE_EQUAL",
        "EQUAL",
        "NOT_EQUAL",

        "START",
        "LEX_EOF",
        "NUM",
        "COMMENT",
        "BLOCK_COMMENT",
        "IDENTIF",
        "INPUTS",
        "INPUTF",
        "INPUTI",
        "PRINT",
        "ORD",
        "CHR",
        "SUBSTR",
        "LENGTH",
        "MAXTOKEN"

};


// int main() {
//     int a;
//     string *value;
//     value = malloc(sizeof(string));
//     if (value == NULL) return ERR_INTERNAL;
//     if (strInit(value) == STR_ERROR) return ERR_INTERNAL;
//     int line = 0;
//
//     do {
//         a = getToken(value, &line);
//         fprintf(stdout,"Value: %15s line: %5d type: %s\n", value->str, line, values[a-200]);
////         if (a == LEX_EOF) {
////             break;
////         }
//
//     } while (a != KEYWORD_NIL);
//     return 0;
// }