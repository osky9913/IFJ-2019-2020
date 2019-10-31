/**
 * @file scanner.c
 * @brief This module 
 * @author xziska03
 */
#include "indent_stack.h"
#include <stdio.h>
#include <stdlib.h>

indent_stack_t* indent_stack_init()
{
	indent_stack_t* s = malloc(sizeof(indent_stack_t));
	if(s == NULL) {
        fprintf(stderr, "indent_Stack ERROR: Could not allocate memory for a indent_stack\n");
        return NULL;
    }
    s->array = calloc(STACK_DEFAULT_LEN, 1);
    if( s->array == NULL){
    	fprintf(stderr, "indent_Stack ERROR: Could not allocate memory for an integer array.\n");
    	return NULL;
    }
    s->array[0] = 0;
	s->top = 1;
	s->size = STACK_DEFAULT_LEN;
	return s;
}

int indent_stackEmpty (indent_stack_t* s ){
	return s->top == -1 ? 1 : 0;
}

int indent_stack_top (indent_stack_t* s){

	if(indent_stackEmpty(s)){
		return -1;
	}
	return s->array[s->top-1];
}

int indent_stack_pop (indent_stack_t* s ) {

	if(indent_stackEmpty(s))
		return STACK_ERROR;

	s->array[s->top--] = '\0';
	return 0;
}

int indent_stack_push (indent_stack_t* s, int c) {

    if(s->top+2 >= s->size){
        s->size += 2;
        int *tmp = realloc(s->array, s->size);
        if(!tmp){
        	fprintf(stderr, "indent_Stack error: Could not push another item onto the indent_stack!\n");
        	return STACK_ALLOC_ERROR;
        }
        s->array = tmp;
    }

	s->array[s->top++] = c;
	return 0;
}

void indent_stack_free(indent_stack_t* s){
	if(s){
		free(s->array);
		free(s);
	}
	else{
		fprintf(stderr, "String WARNING: A pointer passed to indent_stack_free was NULL.\n");
	}

}
