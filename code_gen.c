#include "code_gen.h"
#include "semantic.h"
#include "parser.h"

/* CONVENTIONS:
 * special LOCAL FRAME variables has prefix $
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
    InitList(&instr);
    return 0;
}

int code_gen_clean() {
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
    GEN_INSTR("DEFVAR %s", "GF@optype1");
    GEN_INSTR("DEFVAR %s", "GF@optype2");
    GEN_INSTR("DEFVAR GF@%%*%s", "op1");
    GEN_INSTR("DEFVAR GF@%%*%s", "op2");

    GEN_INSTR("DEFVAR %s", "TF@$retval");

    GEN_INSTR("%s", "PUSHFRAME");
    return 0;
}

void code_generate() {
    printList(&instr);
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

int find_instr(char *string, ...) {
    va_list vaList;
    va_start(vaList, string);
    char instruction[INSTR_LENGTH];
    vsprintf(instruction, string, vaList);
    va_end(vaList);
    if (!find(&instr, instruction))
        // not found
        return ERR_INTERNAL;
    return 0;
}

int insert_instr_after(char *string, ...) {
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

int gen_fun_footer(char *label) {
    GEN_INSTR("%s", "POPFRAME");
    GEN_INSTR("%s", "RETURN");
    GEN_INSTR("LABEL &&%s", label);
    return 0;
}

int gen_builtin_fun(char *fun_id, unsigned params_count) {
    if (is_print_fun(fun_id))
        return gen_print(fun_id, params_count); // print is little bit complicated so different gen.

    if (is_fun_defined(fun_id)) return 0;
    set_fun_defined(fun_id);
    if (strcmp(fun_id, "inputs") == 0) return gen_inputs();
    if (strcmp(fun_id, "inputi") == 0) return gen_inputi();
    if (strcmp(fun_id, "inputf") == 0) return gen_inputf();
    if (strcmp(fun_id, "length") == 0) return gen_length();
    if (strcmp(fun_id, "substr") == 0) return gen_substr();
    if (strcmp(fun_id, "ord") == 0) return gen_ord();
    if (strcmp(fun_id, "chr") == 0) return gen_chr();
    return 0;
}


bool is_print_fun(char *fun_id) {
    return strcmp("print", fun_id) == 0;
}


/* generates label for jump if semantic check fails - 1 label for whole fun def */
int gen_semantic_type_check_header(char *fun_id) {
    GEN_INSTR("JUMP %s%s", "$SEMANTIC$CHECK$TYPE$", fun_id, "$END$LABEL$");
    GEN_INSTR("LABEL %s%s", "$SEMANTIC$CHECK$TYPE$FAIL$", fun_id);
    GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
    GEN_INSTR("LABEL %s%s", "$SEMANTIC$CHECK$TYPE$", fun_id, "$END$LABEL$");
    return 0;
}

/* generates type check with given fun_id (to have unique labels for function) */
int gen_semantic_type_check(char *fun_id, char *frame_var, char *desired_type) {
    static int counter_params = 0;
    GEN_INSTR("DEFVAR %s%s", frame_var, "$type");
    GEN_INSTR("DEFVAR %s%s", frame_var, "$tmp");
    GEN_INSTR("TYPE %s%s %s", frame_var, "$type", frame_var);

    // if float and should be int -> do implicit conversion to int
    if (strcmp(desired_type, "int") == 0) {
        GEN_INSTR("JUMPIFEQ %s%s%d %s%s %s", "$IMPLICIT$FLOAT$CONVERTION$", fun_id, counter_params, frame_var, "$type", "string@float");
        GEN_INSTR("JUMP %s%s%d", "$DO$SEMANTIC$CHECK$", fun_id, counter_params);  // skip this conversion

        GEN_INSTR("LABEL %s%s%d", "$IMPLICIT$FLOAT$CONVERTION$", fun_id, counter_params);
        GEN_INSTR("FLOAT2INT %s %s", frame_var, frame_var);
        GEN_INSTR("JUMP %s%s%d", "$SEMANTIC$CHECK$TYPE$END$", fun_id, counter_params);
    }

    GEN_INSTR("LABEL %s%s%d", "$DO$SEMANTIC$CHECK$", fun_id, counter_params);
    GEN_INSTR("MOVE %s%s %s", frame_var, "$tmp", frame_var);
    GEN_INSTR("JUMPIFNEQ %s%s %s%s %s%s", "$SEMANTIC$CHECK$TYPE$FAIL$", fun_id, frame_var, "$type", "string@",
              desired_type);

    GEN_INSTR("LABEL %s%s%d", "$SEMANTIC$CHECK$TYPE$END$", fun_id, counter_params);
    counter_params++;

    return 0;
}

int gen_inputs() {
    if (gen_fun_header("inputs") == ERR_INTERNAL) return ERR_INTERNAL;

    GEN_INSTR("DEFVAR %s", "LF@$line");
    GEN_INSTR("READ %s %s", "LF@$line", "string");
    GEN_INSTR("MOVE %s %s", "LF@$retval", "LF@$line");

    if (gen_fun_footer("inputs") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_inputi() {
    if (gen_fun_header("inputi") == ERR_INTERNAL) return ERR_INTERNAL;

    GEN_INSTR("DEFVAR %s", "LF@$input_int");
    GEN_INSTR("READ %s %s", "LF@$input_int", "int");
    GEN_INSTR("MOVE %s %s", "LF@$retval", "LF@$input_int");

    if (gen_fun_footer("inputi") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_inputf() {
    if (gen_fun_header("inputf") == ERR_INTERNAL) return ERR_INTERNAL;

    GEN_INSTR("DEFVAR %s", "LF@$input_float");
    GEN_INSTR("READ %s %s", "LF@$input_float", "float");
    GEN_INSTR("MOVE %s %s", "LF@$retval", "LF@$input_float");

    if (gen_fun_footer("inputf") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

unsigned num_digits(const unsigned n) {
    if (n < 10) return 1;
    return 1 + num_digits(n / 10);
}

int gen_print(char *fun_id, unsigned params_count) {
    unsigned num_d = num_digits(params_count);
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

int gen_length() {
    if (gen_fun_header("length") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check_header("length") == ERR_INTERNAL) return ERR_INTERNAL;

    if (gen_semantic_type_check("length", "LF@%1", "string") == ERR_INTERNAL) return ERR_INTERNAL;

    GEN_INSTR("LABEL %s", "$type$string$length$");
    GEN_INSTR("STRLEN %s %s", "LF@$retval", "LF@%1");

    if (gen_fun_footer("length") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_substr() {
    if (gen_fun_header("substr") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check_header("substr") == ERR_INTERNAL) return ERR_INTERNAL;

    if (gen_semantic_type_check("substr", "LF@%1", "string") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check("substr", "LF@%2", "int") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check("substr", "LF@%3", "int") == ERR_INTERNAL) return ERR_INTERNAL;

    /* length() fun call */
    GEN_INSTR("%s", "CREATEFRAME");
    GEN_INSTR("DEFVAR TF@%s", "%1");
    GEN_INSTR("MOVE %s %s", "TF@%1", "LF@%1");
    GEN_INSTR("CALL *%s", "length");
    if (!is_fun_defined("length")) gen_builtin_fun("length", 1);
    GEN_INSTR("MOVE %s %s ", "LF@$retval", "TF@$retval");
    GEN_INSTR("DEFVAR %s", "LF@$LENGTH");
    GEN_INSTR("MOVE %s %s", "LF@$LENGTH", "LF@$retval");

    /* check bounds of second param: <0, length(s)> */
    GEN_INSTR("DEFVAR %s", "LF@$greater$than$minus$one$");
    GEN_INSTR("DEFVAR %s", "LF@$less$than$length$plus$one$");
    GEN_INSTR("GT %s %s %s", "LF@$greater$than$minus$one$", "LF@%2", "int@-1");
    GEN_INSTR("LT %s %s %s", "LF@$less$than$length$plus$one$", "LF@%2", "LF@$retval");
    GEN_INSTR("DEFVAR %s", "LF@$check$passed$");
    GEN_INSTR("AND %s %s %s", "LF@$check$passed$", "LF@$greater$than$minus$one$", "LF@$less$than$length$plus$one$");
    GEN_INSTR("MOVE LF@%s nil@nil", "$retval");  // clear $retval because of call length() above
    GEN_INSTR("JUMPIFEQ %s %s %s", "$fail$substr$", "LF@$check$passed$", "bool@false"); // index out of bounds
    /* check third param: >= 0 */
    GEN_INSTR("GT %s %s %s", "LF@$greater$than$minus$one$", "LF@%3", "int@-1");
    GEN_INSTR("JUMPIFEQ %s %s %s", "$fail$substr$end$", "LF@%3", "int@0");
    GEN_INSTR("JUMPIFEQ %s %s %s", "$fail$substr$", "LF@$greater$than$minus$one$", "bool@false"); // index out of bounds

    /* substring processing */
    GEN_INSTR("DEFVAR %s", "LF@$substring");
    GEN_INSTR("DEFVAR %s", "LF@$substring$length$");
    GEN_INSTR("DEFVAR %s", "LF@$length$minus$index$");
    GEN_INSTR("DEFVAR %s", "LF@$n$greater$than$expected$");

    /* check of length(s) - i */
    GEN_INSTR("MOVE %s %s", "LF@$length$minus$index$", "LF@$LENGTH");
    GEN_INSTR("SUB %s %s %s", "LF@$length$minus$index$", "LF@$LENGTH", "LF@%2");
    GEN_INSTR("GT %s %s %s", "LF@$n$greater$than$expected$", "LF@%3", "LF@$length$minus$index$");
    GEN_INSTR("DEFVAR %s", "LF@$iterations");
    GEN_INSTR("JUMPIFEQ %s %s %s", "$label$n$greater$than$expected$", "LF@$n$greater$than$expected$", "bool@true");

    /* iterations = n */
    GEN_INSTR("MOVE %s %s", "LF@$iterations", "LF@%3");

    /* iterations = length(s) - i */
    GEN_INSTR("JUMP %s", "$label$n$greater$than$expected$end$");
    GEN_INSTR("LABEL %s", "$label$n$greater$than$expected$");
    GEN_INSTR("MOVE %s %s", "LF@$iterations", "LF@$length$minus$index$");
    GEN_INSTR("LABEL %s", "$label$n$greater$than$expected$end$");

    /* vars used in while cycle */
    GEN_INSTR("DEFVAR %s", "LF@$temp_char");
    GEN_INSTR("DEFVAR %s", "LF@$pos_input_str");
    GEN_INSTR("DEFVAR %s", "LF@$pos_output_str");
    GEN_INSTR("MOVE %s %s", "LF@$pos_input_str", "LF@%2");
    GEN_INSTR("MOVE %s %s", "LF@$pos_output_str", "int@0");
    GEN_INSTR("MOVE %s %s", "LF@$substring", "string@"); // init to empty string

    /* while cycle */
    GEN_INSTR("LABEL %s", "$while$substr$");
    GEN_INSTR("GETCHAR %s %s %s", "LF@$temp_char", "LF@%1", "LF@$pos_input_str");
    GEN_INSTR("CONCAT %s %s %s", "LF@$substring", "LF@$substring", "LF@$temp_char");
    GEN_INSTR("ADD %s %s %s", "LF@$pos_input_str", "int@1", "LF@$pos_input_str");
    GEN_INSTR("ADD %s %s %s", "LF@$pos_output_str", "int@1", "LF@$pos_output_str");
    GEN_INSTR("SUB %s %s %s", "LF@$iterations", "LF@$iterations", "int@1");
    GEN_INSTR("JUMPIFNEQ %s %s %s", "$while$substr$", "LF@$iterations", "int@0");

    GEN_INSTR("MOVE %s %s", "LF@$retval", "LF@$substring");

    GEN_INSTR("JUMP %s", "$fail$substr$end$");
    GEN_INSTR("LABEL %s", "$fail$substr$");
    GEN_INSTR("MOVE LF@%s nil@nil", "$retval");
    GEN_INSTR("LABEL %s", "$fail$substr$end$");

    if (gen_fun_footer("substr") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_ord() {
    if (gen_fun_header("ord") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check_header("ord") == ERR_INTERNAL) return ERR_INTERNAL;

    if (gen_semantic_type_check("ord", "LF@%1", "string") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check("ord", "LF@%2", "int") == ERR_INTERNAL) return ERR_INTERNAL;

    /* length() fun call */
    GEN_INSTR("%s", "CREATEFRAME");
    GEN_INSTR("DEFVAR TF@%s", "%1");
    GEN_INSTR("MOVE %s %s", "TF@%1", "LF@%1");
    GEN_INSTR("CALL *%s", "length");
    if (!is_fun_defined("length")) gen_builtin_fun("length", 1);
    GEN_INSTR("MOVE %s %s ", "LF@$retval", "TF@$retval");

    GEN_INSTR("DEFVAR %s", "LF@$greater$than$minus$one$");
    GEN_INSTR("DEFVAR %s", "LF@$less$than$length$");
    GEN_INSTR("GT %s %s %s", "LF@$greater$than$minus$one$", "LF@%2", "int@-1");
    GEN_INSTR("LT %s %s %s", "LF@$less$than$length$", "LF@%2", "LF@$retval");
    GEN_INSTR("DEFVAR %s", "LF@$check$passed$");
    GEN_INSTR("AND %s %s %s", "LF@$check$passed$", "LF@$greater$than$minus$one$", "LF@$less$than$length$");
    GEN_INSTR("MOVE LF@%s nil@nil", "$retval");  // clear $retval because of call length() above
    GEN_INSTR("JUMPIFEQ %s %s %s", "$fail$ord$", "LF@$check$passed$", "bool@false"); // index out of bounds
    /* compute the ord value */
    GEN_INSTR("STRI2INT %s %s %s", "LF@$retval", "LF@%1", "LF@%2");

    GEN_INSTR("LABEL %s", "$fail$ord$");  // $retval will be nil@nil because unchanged from fun_header

    if (gen_fun_footer("ord") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int gen_chr() {
    if (gen_fun_header("chr") == ERR_INTERNAL) return ERR_INTERNAL;
    if (gen_semantic_type_check_header("chr") == ERR_INTERNAL) return ERR_INTERNAL;

    if (gen_semantic_type_check("chr", "LF@%1", "int") == ERR_INTERNAL) return ERR_INTERNAL;

    /* compute the chr value */
    GEN_INSTR("INT2CHAR %s %s", "LF@$retval", "LF@%1");

    if (gen_fun_footer("chr") == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}
