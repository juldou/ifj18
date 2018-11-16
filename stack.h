#ifndef IFJ_STACK_H
#define IFJ_STACK_H

#include <stdbool.h>

typedef struct stack_item {
    int symbol;
    struct stack_item *next;
} t_stack_item;

typedef struct {
    t_stack_item *top;
} t_stack;

void stack_init(t_stack *stack);

void push(t_stack *stack, int symbol);

void pop(t_stack *stack);

bool is_terminal(int token);

t_stack_item *top_term(t_stack *stack);

void multi_pop(t_stack *stack, int count);

void insert_after_first_terminal(t_stack *stack, int symbol);

#endif //IFJ_STACK_H
