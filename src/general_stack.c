/**
 *	@file   general_stack.c
 *	@author Martin Osvald , xosval03
 *	@date
 *	@brief Modul implementujici stack.
 */


#include "general_stack.h"
#include "errors.h"

stack_general_t *stack_general_init() {
    stack_general_t *stack = (stack_general_t *) malloc(sizeof(struct stack_general));
    if (stack == NULL) {
        return NULL;
    }
    stack->top = NULL;
    return stack;

}


void *stack_general_top(stack_general_t *stack) {

    return (void *) stack->top;
}


int stack_general_push(stack_general_t *stack, void *data) {

    stack_general_item_t *new_item = (stack_general_item_t *) malloc(sizeof(struct stack_general_item));
    if (new_item == NULL) {
        return ALLOC_ERROR;
    }

    new_item->data = data;

    if (stack->top == NULL) {
        stack->top = new_item;
        return SUCCESS;
    }

    stack_general_item_t *temp = stack->top;

    stack->top = new_item;
    new_item->next = temp;
    return SUCCESS;

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


int stack_clear(stack_general_t *stack) {
    if (stack == NULL) {
        return NULL_PASSED;
    }

    while (!stack_empty(stack)) {
        stack_pop(stack);
    }
    stack->top = NULL;
    return SUCCESS;
}
