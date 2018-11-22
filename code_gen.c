#include "code_gen.h"

/* CONVENTIONS:
 * special variables has prefix $
 *      - return value of function is $retval
 * function has prefix * (*some_function)
 */

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

int gen_fun_header(char *label) {
    GEN_INSTR("LABEL %s", label);
    GEN_INSTR("%s", "PUSHFRAME");
    GEN_INSTR("DEFVAR LF@%s", "$retval");
    GEN_INSTR("MOVE LF@%s nil@nil", "$retval");
    return 0;
}

int gen_fun_footer() {
    GEN_INSTR("%s", "POPFRAME");
    GEN_INSTR("%s", "RETURN");
    return 0;
}

int gen_builtin_fun(char *fun_id) {
    if (strcmp(fun_id, "print") == 0) return gen_print();
    return 0;
}

int gen_print() {
    if (gen_fun_header("*print") == ERR_INTERNAL) return ERR_INTERNAL;

    // TODO:

    if (gen_fun_footer() == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}
