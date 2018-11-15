#ifndef PROJECT_EXPR_PARSER_H
#define PROJECT_EXPR_PARSER_H

#include "parser.h"
#include "stack.h"
#include "lex.h"

enum table_symbol {
    NO = MAXTOKEN, LE, EQ, GR, TSMAX
};

enum non_terminal {
    EXPR = TSMAX
};

int bool_expr();

int math_expr();

#endif //PROJECT_EXPR_PARSER_H
