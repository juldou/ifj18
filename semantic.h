//
// Created by Július  Marko  on 13/11/2018.
//

#ifndef IFJ_SEMANTIC_H
#define IFJ_SEMANTIC_H

#include "symtable.h"

int semantic_prepare();

int semantic_clean();

int add_builtin_funcs_to_st();

void init_fun_elem_data(elem_data *data, size_t params_count, bool defined, bool builtin);

int insert_var_to_st(char *var_id, char *fun_id, bool defined);

int insert_fun_to_st(char *fun_id, size_t params_count, bool defined, bool builtin);

int semantic_add_fun_param(char *fun_id, char *param);

int semantic_check_var_defined(char *fun_id, char *var_id);

int semantic_check_fun_defined(char *fun_id);

int semantic_check_fun_call_params(char *fun_id, size_t params_count);

int set_fun_defined(char *fun_id);

bool function_parameter_exists(st_elem *elem, char *param);

bool is_function(char *fun_id);

bool is_fun_builtin(char *fun_id);

bool is_fun_defined(char *fun_id);

bool is_print_fun(char *fun_id);

bool is_main_scope(char *fun_id);

bool is_variable(char *var_id, char *fun_id);

bool all_ids_defined();

#endif //IFJ_SEMANTIC_H
