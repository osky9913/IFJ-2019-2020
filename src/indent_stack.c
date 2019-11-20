/**
 * @file indent_stack.c
 * @brief This module implements simple stack of integers
 * @author xziska03
 */
#include "indent_stack.h"
#include <stdio.h>
#include <stdlib.h>

indent_stack_t* indent_stack_init()
{
	indent_stack_t* s = malloc(sizeof(struct indent_stack));
	if(s == NULL) {
        fprintf(stderr, "Indent stack ERROR: Could not allocate memory for a indent_stack\n");
        return NULL;
    }
    s->array = calloc(STACK_DEFAULT_LEN, sizeof(int));
    if( s->array == NULL){
    	fprintf(stderr, "Indent stack ERROR: Could not allocate memory for an integer array.\n");
    	return NULL;
    }
    s->array[0] = 0;
    s->top = 0;
	s->size = STACK_DEFAULT_LEN;
	return s;
}

int indent_stack_empty (indent_stack_t* s ){
	return s->top == -1 ? 1 : 0;
}

int indent_stack_top (indent_stack_t* s){

	if(indent_stack_empty(s)){
		return -1;
	}
    return s->array[s->top];
}

int indent_stack_pop (indent_stack_t* s ) {

	if(indent_stack_empty(s))
		return -1;

    s->array[s->top--] = 0;
	return 0;
}

int indent_stack_push (indent_stack_t* s, int c) {

    if (s->top + 1 >= s->size) {

        int *tmp = realloc(s->array, s->size * 2);
        if(!tmp){
        	fprintf(stderr, "Indent stack error: Could not push another item onto the indent_stack!\n");
        	return STACK_ALLOC_ERROR;
        }
        s->size *= 2;
        s->array = tmp;
    }
    s->array[++(s->top)] = c;
	return 0;
}

void indent_stack_print(indent_stack_t* s, int dent, int pop_indent){
    printf("\nINDENT - |%d| POP INDENT - |%d| S->TOP - |%d| INDENT STACK TOP - |%d|\n", dent, pop_indent, s->top, indent_stack_top(s));
    printf("STACK PRINT\n-------------\n");
    for (int i = 0; i <= s->top; i++)
    {
        printf("pos|%d| value |%d|\n", i, s->array[i]);
    }
    printf("--------------\nSTACK PRINT\n\n");
}

void indent_stack_free(indent_stack_t* s){
	if(s){
		free(s->array);
		free(s);
	}
	else{
		fprintf(stderr, "Stack WARNING: A pointer passed to indent_stack_free was NULL.\n");
	}

}
