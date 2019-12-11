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

/**
 * returning allocated pointer of stack general or NULL
 * @param nothing
 */
stack_general_t *stack_general_init();


/**
 * @brief use this function for get top from your stack
 * @param stack
 * @return  pointer to item
 */
struct stack_general_item *stack_general_top(stack_general_t *stack);


/**
 * @brief use this function to push on top of stack your data, the data should be allocated already
 * @param stack and  data  what you want to push
 * @return success or alloc error
 */
int stack_general_push(stack_general_t *stack, void *data);



/**
 * @brief use this function to push on top of stack your data,
 * @param stack and  data  what you want to push
 * @return success or alloc error
 */
int stack_general_push_int(stack_general_t *stack, int data);


/**
 * @brief use this function to push on top of stack your data,
 * @param stack and  data  what you want to push
 * @return success or alloc error
 */
int stack_general_push_char(stack_general_t *stack, char data);


/**
 * @brief pop and free item with free data,
 * @param stack
 * @return void
 */
void stack_pop(stack_general_t *stack);
/**
 * [Pops item from the top of the stack but does not free memory of item's data
 * @param stack [Stack from which will be item poped]
 */
void stack_popNoDataFree(stack_general_t *stack);

/**
 * @brief returns true if stack is empty,
 * @param stack
 * @return bool
 */
bool stack_empty(stack_general_t *stack);


/**
 * @brief stack clear free every item,
 * @param stack
 * @return int
 */
int stack_clear(stack_general_t *stack);


/**
 * @brief stack free calls stack clear and free on your allocated stack,
 * @param stack
 * @return int
 */
int stack_free(stack_general_t *stack);


#endif

