/**
 * School project to subject IFJ (Formal Languages and Compilers)
 * Compiler implementation of imperative language IFJ18
 *
 * Interface of module for parsing expressions and conditions
 *
 * Author: Ján Vavro
 * Login: xvavro05
 */


#ifndef PROJECT_EXPR_PARSER_H
#define PROJECT_EXPR_PARSER_H

#include "parser.h"
#include "stack.h"
#include "lex.h"

/**
 * enum that represents items in precedence table
 */
enum table_symbol {
    NO = MAXTOKEN, LE, EQ, GR, TSMAX
};

/**
 * non-terminal used in precedence analysis
 */
enum non_terminal {
    EXPR = TSMAX
};

int bool_expr(char *fun_id);

int math_expr(char *fun_id);

#endif //PROJECT_EXPR_PARSER_H
