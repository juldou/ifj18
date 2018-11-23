//
// Created by janko on 11/5/18.
//

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

#define SYNTAX_OK 101

int assign(char *fun_id);
int expr();
int fun_params(char *fun_id);
int params(char *fun_id, char *called_from_fun, unsigned *par_count);
int fun_declr();
int fun_call(char *fun_id, char *called_from_fun);
int stat_list(char *fun_id);

int program();

int parse();
#endif //IFJ_PARSER_H
