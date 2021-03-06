/**
* School project to subject IFJ (Formal Languages and Compilers)
* Compiler implementation of imperative language IFJ18
*
* Module for recursive descent parser
*
* Author: Jan Zauska  Julius Marko  Jan Vavro
* Login:  xzausk00    xmarko17      xvavro05
*/

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

#include "lex.h"

#define SYNTAX_OK 101

#define GET_TOKEN() do {if ((token = getToken(value)) == ERR_LEXICAL)\
return ERR_LEXICAL;} while(0)

#define ACCEPT(type) do{ if(token != (type)) return ERR_SYNTAX;\
    GET_TOKEN();} while(0)

#define IS_VALID_PARAM (token == ID || token == NUM_INT || token == NUM_FLOAT || token == NUM_EXP || token == STRING)

int assign(char *fun_id);

int fun_params(char *fun_id);

int params(char *fun_id, char *called_from_fun, unsigned *par_count);

int fun_declr();

int fun_call(char *fun_id, char *called_from_fun);

int stat_list(char *fun_id);

int program();

int parse();

#endif //IFJ_PARSER_H
