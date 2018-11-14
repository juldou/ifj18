#include <zconf.h>
#include "expr_parser.h"
#include "lex.h"


#define SIZE 7

const char prec_table[SIZE][SIZE] = {
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

t_stack *stack;

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
            return 5;
        case LEX_EOF:
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
    }

    if (tmp->symbol != GR) {
        free(params);
        va_end(vaList);
        return false;
    }

    free(params);
    va_end(vaList);
    return true;
}


