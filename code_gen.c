#include "code_gen.h"
#include "semantic.h"

/* CONVENTIONS:
 * special variables has prefix $
 *      - return value of function is $retval
 *      - variable that holds type of variable some_var is some_var$type
 * function has prefix * (*some_function)
 *
 * FUNCTION CALL:
 * Before function call, vars are pushed to TF (TEMPORARY FRAME) and named
 * %1 for first param, %2 for second param, %3 for third param, etc...
 * so you know exactly how vars are named in function definition for example
 * the first param will be LF@1, second LF@2, etc...
 *      - you can have a look at gen_length() below
 *
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
    GEN_INSTR("%s", "CREATEFRAME");
    GEN_INSTR("LABEL %s", "*MAIN");

    GEN_INSTR("DEFVAR %s", "GF@expr_res");
    GEN_INSTR("DEFVAR %s", "TF@$retval");

    GEN_INSTR("%s", "PUSHFRAME");
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

int find_instr(char* string, ...){
    va_list vaList;
    va_start(vaList, string);
    char instruction[1024];
    vsprintf(instruction, string, vaList);
    va_end(vaList);
    if(!find(&instr, instruction))
        // not found
        return ERR_INTERNAL;
    return 0;
}

int insert_instr_after(char *string, ...){
    va_list vaList;
    va_start(vaList, string);
    char *instruction;
    if ((instruction = PostInsert(&instr)) == NULL) return ERR_INTERNAL;
    vsprintf(instruction, string, vaList);
    va_end(vaList);

    return 0;
}

int gen_fun_header(char *label) {
    GEN_INSTR("JUMP &&%s", label);
    GEN_INSTR("LABEL *%s", label);
    GEN_INSTR("%s", "PUSHFRAME");
    GEN_INSTR("DEFVAR LF@%s", "$retval");
    GEN_INSTR("MOVE LF@%s nil@nil", "$retval");
    return 0;
}

int gen_fun_footer(char* label) {
    GEN_INSTR("%s", "POPFRAME");
    GEN_INSTR("%s", "RETURN");
    GEN_INSTR("LABEL &&%s", label);
    return 0;
}

int gen_builtin_fun(char *fun_id, unsigned params_count) {
    if (is_print_fun(fun_id)) return gen_print(fun_id, params_count); // print is little bit complicated so different gen.

    if (is_fun_defined(fun_id)) return 0;
    set_fun_defined(fun_id);
//    if (strcmp(fun_id, "inputs") == 0) return gen_inputs();
//    if (strcmp(fun_id, "inputi") == 0) return gen_inputi();
//    if (strcmp(fun_id, "inputf") == 0) return gen_inputf();
    if (strcmp(fun_id, "length") == 0) return gen_length();
//    if (strcmp(fun_id, "substr") == 0) return gen_substr();
//    if (strcmp(fun_id, "ord") == 0) return gen_ord();
//    if (strcmp(fun_id, "chr") == 0) return gen_chr();
    return 0;
}

unsigned num_digits(const unsigned n) {
    if (n < 10) return 1;
    return 1 + num_digits(n / 10);
}

int gen_print(char *fun_id ,unsigned params_count) {
    unsigned num_d = num_digits( params_count);
    unsigned fun_id_len = ((unsigned) strlen("print")) + num_d + 1;
    char fun_id_new[fun_id_len];
    snprintf(fun_id_new, fun_id_len, "%s%d", fun_id, (int) params_count);

    /* fun exists, just call */
    if (semantic_token_is_function(fun_id_new)) {
        GEN_INSTR("CALL *%s", fun_id_new);
        return 0;
    }

    if (insert_fun_to_st(fun_id_new, params_count, true, true) == ERR_INTERNAL) return ERR_INTERNAL;

    if (gen_fun_header(fun_id_new) == ERR_INTERNAL) return ERR_INTERNAL;

    for (size_t param = 1; param <= params_count; param++) {
        GEN_INSTR("WRITE %s%d", "LF@%", param);
    }

    if (gen_fun_footer(fun_id_new) == ERR_INTERNAL) return ERR_INTERNAL;

    GEN_INSTR("CALL *%s", fun_id_new);
    return 0;
}

bool is_print_fun(char *fun_id) {
    return strcmp("print", fun_id) == 0;
}

int gen_length() {
    if (gen_fun_header("length") == ERR_INTERNAL) return ERR_INTERNAL;

    /* semantic check */
    GEN_INSTR("DEFVAR %s", "LF@%1$type");
    GEN_INSTR("DEFVAR %s", "LF@%1$tmp");
    GEN_INSTR("TYPE %s %s", "LF@%1$type", "LF@%1");
    GEN_INSTR("MOVE %s LF@%s", "LF@%1$tmp", "%1");

    GEN_INSTR("JUMPIFEQ %s %s %s", "$type$string$", "LF@%1$type", "string@string");
    GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE); // exit if variable type is not string

    GEN_INSTR("LABEL %s", "$type$string$");
    GEN_INSTR("STRLEN %s %s", "LF@$retval", "LF@%1");

    if (gen_fun_footer("length") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}
