/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   general_stack.c
 * @author Martin Osvald - xosval03
 * @brief This module implements a general stack
 */


#include "general_stack.h"
#include "errors.h"

stack_general_t *stack_general_init() {
    // constructor
    stack_general_t *stack = (stack_general_t *) malloc(sizeof(struct stack_general));

    //check
    if (stack == NULL) {
        return NULL;
    }
    stack->top = NULL;
    return stack;

}

//return pointer of stack_general_item_t *
stack_general_item_t *stack_general_top(stack_general_t *stack) {
    return stack->top;

}

// push int
int stack_general_push_int(stack_general_t *stack, int data) {
    int *temp = (int *) malloc(sizeof(int)); // alloc if you do not want to loose it
    if (temp == NULL) {
        return ALLOC_ERROR;
    }
    *temp = data;
    return stack_general_push(stack, (void *) temp);
}


// push int
int stack_general_push_char(stack_general_t *stack, char data) {
    char *temp = (char *) malloc(sizeof(char)); // alloc if you do not want to loose it
    if (temp == NULL) {
        return ALLOC_ERROR;
    }
    *temp = data;
    return stack_general_push(stack, (void *) temp);
}



// alloc your item, convert pointer to void and use this function
int stack_general_push(stack_general_t *stack, void *data) {

    stack_general_item_t *new_item = malloc(sizeof(struct stack_general_item));
    if (new_item == NULL) {
        return ALLOC_ERROR;
    }
    new_item->next = NULL;
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
    free(temp->data);
    free(temp);
    temp = NULL;
}

void stack_popNoDataFree(stack_general_t *stack) {
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


int stack_free(stack_general_t *stack) {
    stack_clear(stack);
    free(stack);
    stack = NULL;
    return SUCCESS;


}
