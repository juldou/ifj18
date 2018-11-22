#include "code_gen.h"


tList instr;

int code_gen_prepare() {
    fp = fopen("testPrg.ifj", "w");
    if(fp == NULL) return ERR_INTERNAL;
    InitList(&instr);
    return 0;
}

int code_gen_clean() {
    if (fclose(fp) == EOF) return ERR_INTERNAL;
    DisposeList(&instr);
    return 0;
}

int gen_header() {
    GEN_INSTR("%s", ".IFJcode18");
    if (gen_main() == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_main() {
    GEN_INSTR("JUMP %s", "$$MAIN");
    GEN_INSTR("LABEL %s", "$$MAIN");
    GEN_INSTR("%s", "CREATEFRAME");
    return 0;
}

void code_generate() {
    printList (&instr);
}

int gen_instr(char *string, ...) {
    va_list vaList;

//    size_t str_len = 0;
//    for (int i = 0; i < params_count; ++i) {
//        str_len += strlen(va_arg(vaList, char *));
//    }
//    str_len -= (params_count - 1) * 2;

    va_start(vaList, string);
//    size_t len = strlen(string);
    char *instruction;
    if ((instruction = AppendToList(&instr)) == NULL) return ERR_INTERNAL;
    vsprintf(instruction, string, vaList);
    va_end(vaList);
    return 0;
}

// There will be more complex constructs built from instructions (instr.h)
// like gen while, for, print fun, ...

