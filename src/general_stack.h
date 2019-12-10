/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   general_stack.h
 * @author Martin Osvald - xosval03
 * @brief A header file for the general stack module
 */


#ifndef __STACK_GENERAL_H__
#define __STACK_GENERAL_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct stack_general_item {
    void *data;
    struct stack_general_item *next;
} stack_general_item_t;


typedef struct stack_general {
    stack_general_item_t *top;
} stack_general_t;


stack_general_t *stack_general_init();

struct stack_general_item *stack_general_top(stack_general_t *stack);

int stack_general_push(stack_general_t *stack, void *data);

int stack_general_push_int(stack_general_t *stack, int data);

int stack_general_push_char(stack_general_t *stack, char data);


void stack_pop(stack_general_t *stack);
/**
 * [Pops item from the top of the stack but does not free memory of item's data
 * @param stack [Stack from which will be item poped]
 */
void stack_popNoDataFree(stack_general_t *stack);

bool stack_empty(stack_general_t *stack);

int stack_clear(stack_general_t *stack);

int stack_free(stack_general_t *stack);


#endif

