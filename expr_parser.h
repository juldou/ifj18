#ifndef PROJECT_EXPR_PARSER_H
#define PROJECT_EXPR_PARSER_H

#include "parser.h"
#include "stack.h"
#include "lex.h"
#include "semantic.h"

enum table_symbol {
    NO = MAXTOKEN, LE, EQ, GR, TSMAX
};

enum non_terminal {
    EXPR = TSMAX
};

int bool_expr(char *fun_id);

int math_expr(char *fun_id);

#endif //PROJECT_EXPR_PARSER_H
