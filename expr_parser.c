#include <stdarg.h>
#include "expr_parser.h"
#include "error.h"
#include "semantic.h"
#include "code_gen.h"

#define SIZE 7
#define SYNTAX_OK 101
#define MATH 0
#define BOOL 1

#define GET_TOKEN() do {if ((token = getToken(value, &line)) == ERR_LEXICAL)\
return ERR_LEXICAL;} while(0)

#define ACCEPT(type) do{ if(token != (type)) return ERR_SYNTAX;\
    GET_TOKEN();} while(0)

extern int token;

/*
 * Conventions of odd variables for generating code
 * %*NUMBER, where NUMBER is global variable which is
 * incremented after each DEFVAR
 */

extern string *value;
int line;

const int prec_table[SIZE][SIZE] = {
//             0    1   2   3   4   5   6
//             + - * / rel  (   )  ID   $
/*  0 + - */ {GR, LE, GR, LE, GR, LE, GR},

/*  1 * / */
             {GR, GR, GR, LE, GR, LE, GR},

/*  2 rel */
             {LE, LE, NO, LE, GR, LE, GR},

/*  3 (   */
             {LE, LE, LE, LE, EQ, LE, NO},

/*  4 )   */
             {GR, GR, GR, NO, GR, NO, GR},

/*  5 id  */
             {GR, GR, GR, NO, GR, NO, GR},

/*  6 $   */
             {LE, LE, LE, LE, NO, LE, NO}
};

int decode(int symbol) {
    switch (symbol) {
        case MINUS:
        case PLUS:
            return 0;
        case MUL:
        case DIV:
            return 1;
        case EQUAL:
        case NOT_EQUAL:
        case LESS:
        case LESS_EQUAL:
        case MORE:
        case MORE_EQUAL:
            return 2;
        case ROUNDL:
            return 3;
        case ROUNDR:
            return 4;
        case ID:
        case NUM_EXP:
        case NUM_FLOAT:
        case NUM_INT:
        case STRING:
        case KEYWORD_NIL:
            return 5;
        case LEX_EOF:
        case LEX_EOL:
        case KEYWORD_DO:
        case KEYWORD_THEN:
            return 6;
        default:
            return -1;
    }
}

bool check_rule(t_stack *stack, int count, ...) {
    va_list vaList;
    va_start(vaList, count);

    int *params = malloc(count * sizeof(int));

    if (params == NULL) {
        va_end(vaList);
        return false;
    }

    for (int i = 0; i < count; ++i) { //give params to array in reverse order
        params[count - 1 - i] = va_arg(vaList, int);
    }

    t_stack_item *tmp = stack->top;

    for (int i = 0; i < count; ++i) {
        if (tmp->symbol != params[i]) {
            free(params);
            va_end(vaList);
            return false;
        }

        if (tmp->next == NULL) {
            free(params);
            va_end(vaList);
            return false;
        }

        tmp = tmp->next;
    }

    if (tmp->symbol != LE) {
        free(params);
        va_end(vaList);
        return false;
    }

    free(params);
    va_end(vaList);
    return true;
}

void pop_rule(t_stack *stack, int count, int symbol) {
    multi_pop(stack, count + 1); //pop rule and symbol '<'
    push(stack, symbol);
}


int rules(t_stack *stack, string prev_value) {
    static unsigned int var_number = 0, label_number = 0;

    if (check_rule(stack, 3, EXPR, PLUS, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "ADDS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "ADDS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "ADDS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "ADDS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/


        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("CONCAT GF@expr_res GF@%%*%s GF@%%*%s", "op1", "op2");
        GEN_INSTR("PUSHS GF@%s", "expr_res");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, MINUS, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2"); //op2
        GEN_INSTR("POPS GF@%%*%s", "op1"); //op1
        GEN_INSTR("PUSHS GF@%%*%s", "op1"); //op1 has to be pushed first
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "SUBS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "SUBS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "SUBS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "SUBS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, MUL, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "MULS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "MULS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "MULS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "MULS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, DIV, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("JUMPIFEQ div_by_zero%d GF@%%*%s int@0", label_number, "op2");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "IDIVS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("JUMPIFEQ div_by_zero%d GF@%%*%s float@0x0p+0", label_number, "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "DIVS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("JUMPIFEQ div_by_zero%d GF@%%*%s float@0x0p+0", label_number, "op2");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "DIVS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("JUMPIFEQ div_by_zero%d GF@%%*%s int@0", label_number, "op2");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "DIVS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/
        GEN_INSTR("LABEL %s%d", "div_by_zero", label_number);
        GEN_INSTR("EXIT int@%d", ERR_ZERO_DIVISION);
        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, LESS, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "LTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "LTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "LTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "LTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/


        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "LTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, MORE, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "GTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "GTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "GTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "GTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/


        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "GTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, LESS_EQUAL, EXPR)) {
        //control types
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/


        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        //duplicate numbers in stack, call '<' then '==' and 'or' results

        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");

        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");

        GEN_INSTR("%s", "LTS");
        GEN_INSTR("POPS GF@%%*%s", "op1"); //result of <
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        GEN_INSTR("%s", "ORS");

        pop_rule(stack, 3, EXPR);

        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, MORE_EQUAL, EXPR)) {
        //control types
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/


        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        //duplicate numbers in stack, call '>' then '==' and 'or' results

        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");

        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");

        GEN_INSTR("%s", "GTS");
        GEN_INSTR("POPS GF@%%*%s", "op1"); //result of >
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        GEN_INSTR("%s", "ORS");

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, NOT_EQUAL, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_nil%d GF@%s string@nil", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@nil", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("%s", "NOTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("%s", "NOTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@nil", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("%s", "NOTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("%s", "NOTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@nil", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("%s", "NOTS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/
        GEN_INSTR("LABEL %s%d", "op_1_nil", label_number);
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ nil_nil%d GF@%s string@nil", label_number, "optype2");

        GEN_INSTR("LABEL %s%d", "nil_nil", label_number);
        GEN_INSTR("PUSHS bool@%s", "false");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "dif_types", label_number);
        GEN_INSTR("PUSHS bool@%s", "true");

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, EXPR, EQUAL, EXPR)) {
        GEN_INSTR("POPS GF@%%*%s", "op2");
        GEN_INSTR("POPS GF@%%*%s", "op1");
        GEN_INSTR("PUSHS GF@%%*%s", "op1");
        //get 2 items from data stack to variables

        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype1", "op1");
        GEN_INSTR("TYPE GF@%s GF@%%*%s", "optype2", "op2");
        //get their types

        GEN_INSTR("JUMPIFEQ op_1_int%d GF@%s string@int", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_float%d GF@%s string@float", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_string%d GF@%s string@string", label_number, "optype1");
        GEN_INSTR("JUMPIFEQ op_1_nil%d GF@%s string@nil", label_number, "optype1");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);
        //check first op

        GEN_INSTR("LABEL %s%d", "op_1_int", label_number);
        GEN_INSTR("JUMPIFEQ int_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ int_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@nil", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "int_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("JUMP %s%d", "success", label_number);


        GEN_INSTR("LABEL %s%d", "int_float", label_number);
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_float", label_number);
        GEN_INSTR("JUMPIFEQ float_float%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ float_int%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@nil", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "float_float", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        GEN_INSTR("LABEL %s%d", "float_int", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "INT2FLOATS");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "op_1_string", label_number);
        GEN_INSTR("JUMPIFEQ str_str%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@nil", label_number, "optype2");
        GEN_INSTR("EXIT int@%d", ERR_SEMANTIC_TYPE);

        GEN_INSTR("LABEL %s%d", "str_str", label_number);
        GEN_INSTR("PUSHS GF@%%*%s", "op2");
        GEN_INSTR("%s", "EQS");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/
        GEN_INSTR("LABEL %s%d", "op_1_nil", label_number);
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@string", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@int", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ dif_types%d GF@%s string@float", label_number, "optype2");
        GEN_INSTR("JUMPIFEQ nil_nil%d GF@%s string@nil", label_number, "optype2");

        GEN_INSTR("LABEL %s%d", "nil_nil", label_number);
        GEN_INSTR("PUSHS bool@%s", "true");
        GEN_INSTR("JUMP %s%d", "success", label_number);

        /****************************************************/

        GEN_INSTR("LABEL %s%d", "dif_types", label_number);
        GEN_INSTR("PUSHS bool@%s", "false");

        GEN_INSTR("LABEL %s%d", "success", label_number);
        label_number++;

        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 1, ID)) {
        pop_rule(stack, 1, EXPR);
        GEN_INSTR("PUSHS LF@%s", prev_value.str);
        return SYNTAX_OK;
    } else if (check_rule(stack, 3, ROUNDL, EXPR, ROUNDR)) {
        pop_rule(stack, 3, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 1, NUM_INT)) {
        pop_rule(stack, 1, EXPR);
        GEN_INSTR("PUSHS int@%s", prev_value.str);
        return SYNTAX_OK;
    } else if (check_rule(stack, 1, NUM_FLOAT)) {
        pop_rule(stack, 1, EXPR);
        char *endptr;
        double tmp = strtod(prev_value.str, &endptr);
        GEN_INSTR("PUSHS float@%a", tmp);
        return SYNTAX_OK;
    } else if (check_rule(stack, 1, STRING)) {
        GEN_INSTR("PUSHS string@%s", prev_value.str);
        pop_rule(stack, 1, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 1, KEYWORD_NIL)) {
        GEN_INSTR("PUSHS nil@%s", "nil");
        pop_rule(stack, 1, EXPR);
        return SYNTAX_OK;
    } else if (check_rule(stack, 1, NUM_EXP)) {
        char *endptr;
        double tmp = strtod(prev_value.str, &endptr);
        GEN_INSTR("PUSHS float@%a", tmp);
        pop_rule(stack, 1, EXPR);
        return SYNTAX_OK;
    } else return ERR_SYNTAX;
}


int expresion(int type, char *fun_id) {
    int retval;
    string prev_value;
    strInit(&prev_value);

    t_stack *stack = malloc(sizeof(t_stack));
    stack_init(stack);

    GEN_INSTR("%s", "CLEARS");
    push(stack, LEX_EOF);

    do {
        if (type == BOOL && (token == ROUNDR || token == KEYWORD_THEN || token == KEYWORD_DO) &&
            top_term(stack)->symbol == LEX_EOF) {
            break;
        }

        switch (prec_table[decode(top_term(stack)->symbol)][decode(token)]) {
            case EQ:
                push(stack, token);
                strCopyString(&prev_value, value);
                GET_TOKEN();
                break;
            case LE:
                insert_after_first_terminal(stack, LE);
                if (token == ID) {
                    if (semantic_check_var_defined(fun_id, value->str) == ERR_SEMANTIC_DEFINITION) {
                        while (stack->top != NULL) pop(stack);
                        free(stack);
                        strFree(&prev_value);
                        return ERR_SEMANTIC_DEFINITION;
                    }
                }
                push(stack, token);
                strCopyString(&prev_value, value);
                GET_TOKEN();
                break;
            case GR:
                retval = rules(stack, prev_value);
                if (retval != SYNTAX_OK) {
                    while (stack->top != NULL) pop(stack);
                    free(stack);
                    strFree(&prev_value);
                    return retval;
                }
                break;
            case NO:
            default:
                while (stack->top != NULL) pop(stack);
                free(stack);
                strFree(&prev_value);
                return ERR_SYNTAX;
        }

    } while (token != LEX_EOL || top_term(stack)->symbol != LEX_EOF);

    while (stack->top != NULL) pop(stack);
    free(stack);
    strFree(&prev_value);
    GEN_INSTR("POPS %s", "GF@expr_res");
    return SYNTAX_OK;

}

int bool_expr(char *fun_id) {
    return expresion(BOOL, fun_id);
}

int math_expr(char *fun_id) {
    return expresion(MATH, fun_id);
}