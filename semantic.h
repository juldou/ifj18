//
// Created by Július  Marko  on 13/11/2018.
//

#ifndef IFJ_SEMANTIC_H
#define IFJ_SEMANTIC_H

// TODO: move functions doing only symtable stuff to another header file

#include "symtable.h"

void semantic_prepare();
void init_fun_elem_data(elem_data *data); // x
int insert_fun_to_st(char *fun_id); // x
int semantic_check_fun_defined(char *fun_id);
int semantic_add_fun_param(char *fun_id, char *param);

#endif //IFJ_SEMANTIC_H
