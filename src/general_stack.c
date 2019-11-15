#include "general_stack.h"


stack_general_t *stack_general_init() {
    stack_general_t *stack = (stack_general_t *) malloc(sizeof(struct stack_general));
    if (stack == NULL) {
        fprintf(stderr, "stack_general alloc failed.\n");
        return NULL;
    }
    stack->top = NULL;
    return stack;

}


void *stack_general_top(stack_general_t *stack) {

    return (void *) stack->top;
}


void stack_general_push(stack_general_t *stack, void *data) {

    stack_general_item_t *new_item = (stack_general_item_t *) malloc(sizeof(struct stack_general_item));
    if (new_item == NULL) {
        fprintf(stderr, " stack_general_push alloc failed.\n");
    }

    if (stack->top == NULL) {
        stack->top = new_item;
        return;
    }

    stack_general_item_t *temp = stack->top;

    stack->top = new_item;
    new_item->next = temp;
    return;

}

void stack_pop(stack_general_t *stack) {
    if (stack_empty(stack)) {
        return;
    }

    stack_general_item_t *temp = stack->top;
    stack->top = temp->next;
    free(temp);
    temp = NULL;

}

bool stack_empty(stack_general_t *stack) {

    if (stack->top == NULL) {
        return true;
    } else {
        return false;
    }


}