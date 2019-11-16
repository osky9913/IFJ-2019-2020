/**
 * @file indent_stack.h
 * @brief This is header file of indent_stack.c
 * @author xziska03
 */
#ifndef _INDENT_STACK_H_
#define _INDENT_STACK_H_

/*default stack length*/
#define STACK_DEFAULT_LEN 10

/* error/return codes */
#define STACK_ALLOC_ERROR 201
#define STACK_ERROR 202

/* Zásobník integerov*/
typedef struct indent_stack{
	int *array;                            
	int top; 								
	int size;                               
} indent_stack_t;

void indent_stack_print(indent_stack_t* s, int dent, int pop_indent);


/**
 * [Allocates memory for indent_stack structure, initializes it]
 * @return [returns created indent_stack]
 */
indent_stack_t* indent_stack_init();

/**
 * [Finds out if indent_stack is empty]
 * @param  s [indent_stack]
 * @return   [returns 1 if indent_stack is empty, else 0]
 */
int indent_stack_empty ( indent_stack_t* s );

/**
 * [Looks whats on top of the indent_stack and returns it]
 * @param  s [indent_stack]
 * @return   [returns actual item on top of indent_stack]
 */
int indent_stack_top ( indent_stack_t* s);

/**
 * [Removes item on top of the indent_stack]
 * @param s [indent_stack]
 */
int indent_stack_pop ( indent_stack_t* s );

/**
 * [Pushes item on top of the indent_stack, if there is not enough space, reallocates size]
 * @param s [indent_stack]
 * @param c [to be pushed integer]
 */
int indent_stack_push ( indent_stack_t* s, int c );

/**
 * [Frees allocated memory for indent_stack]
 * @param s [indent_stack]
 */
void indent_stack_free(indent_stack_t* s);

#endif
