#ifndef IFJ_CODE_GEN_H
#define IFJ_CODE_GEN_H

#include <stdio.h>
#include "list.h"

FILE *fp;

int code_gen_prepare();
int code_gen_clean();
int gen_header();
int gen_main();
int gen_instr(char *c, ...);
void code_generate();
void print_code(tList *L);

#endif //IFJ_CODE_GEN_H
