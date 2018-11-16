#include "stack.h"
#include "lex.h"

void stack_init(t_stack *stack) {
    stack->top = NULL;
}

void push(t_stack *stack, int symbol) {
    t_stack_item *toBePushed = malloc(sizeof(t_stack_item));

    if (toBePushed == NULL) {
        return;
    }

    toBePushed->next = stack->top;
    toBePushed->symbol = symbol;
    stack->top = toBePushed;
}

void pop(t_stack *stack) {
    if (stack->top != NULL) {
        t_stack_item *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp);
    }
}

void multi_pop(t_stack *stack, int count) {
    for (int i = count; i > 0; --i) {
        pop(stack);
    }
}

bool is_terminal(int token) {
    return (token < MAXTOKEN);
}

t_stack_item *top_term(t_stack *stack) {
    t_stack_item *tmp = stack->top;

    while (tmp) {
        if (is_terminal(tmp->symbol)) {
            return tmp;
        } else {
            tmp = tmp->next;
        }
    }
    return NULL;
}

void insert_after_first_terminal(t_stack *stack, int symbol) {
    t_stack_item *prev = NULL, *tmp = stack->top;

    for (; tmp != NULL; tmp = tmp->next) {
        if (is_terminal(tmp->symbol)) {
            t_stack_item *toBeInserted = malloc(sizeof(t_stack_item));

            if (toBeInserted == NULL) return;

            toBeInserted->symbol = symbol;
            toBeInserted->next = tmp;

            if (prev) {
                prev->next = toBeInserted;
            } else {
                stack->top = toBeInserted;
            }
            return;
        } else { //symbol is not terminal
            prev = tmp;
        }
    }
}


