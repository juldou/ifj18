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
    if (gen_instr("%s", ".IFJcode18")) return ERR_INTERNAL;
    if (gen_main() == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_main() {
    gen_instr("JUMP %s", "$$MAIN");
    gen_instr("LABEL %s", "$$MAIN");
    return 0;
}

void code_generate() {
    printList (&instr);
}

int gen_instr(char *string, ...) {
    va_list vaList;
    va_start(vaList, string);
    size_t len = strlen(string);
    char *instruction;
    if ((instruction = AppendToList(&instr, len)) == NULL) return ERR_INTERNAL;
    vsprintf(instruction, string, vaList);
    va_end(vaList);
    return 0;
}

// There will be more complex constructs built from instructions (instr.h)
// like gen while, for, print fun, ...

