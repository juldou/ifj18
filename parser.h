//
// Created by janko on 11/5/18.
//

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

#define SYNTAX_OK 101

int assign();
int expr();
int fun_params(char *func_id);
int params() ;
int fun_declr();
int fun_call();
int stat_list();

int program();

int parse();
#endif //IFJ_PARSER_H