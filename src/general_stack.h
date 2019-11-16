/**
 *	@file   general_stack.h
 *	@author Martin Osvald , xosval03
 *	@date
 *	@brief hlava implementujici stack.
 */


#ifndef STACK_GENERAL_H
#define STACK_GENERAL_H

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

bool stack_empty(stack_general_t *stack);

int stack_clear(stack_general_t *stack);

int stack_free(stack_general_t *stack);


#endif

