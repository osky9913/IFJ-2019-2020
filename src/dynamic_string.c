//
// Created by root on 10/17/19.
//
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_string.h"

int str_find_char(t_string* s, char c){
    int charFound = 0;
    for(int i = 0; i < s->index; i++){
        if(s->array[i] == c){
            charFound++;
        }
    }
    if(charFound >= 1)
        return charFound;
    else
        return 0;
} 

void string_clear(t_string *s){
    s->array[0] = '\0';
    s->index = 0;
}

void string_init(t_string *s) {
    s->index = 0;
    s->real = 100;
    s->array = calloc(s->real, 1);
    if (s->array == NULL) {
        fprintf(stderr, "Allocation problem");
        exit(1);
    }
}


void string_append(t_string *s, char c) {

    if (s->array == NULL) {
        fprintf(stderr, "Parameter NULL");
        exit(1);
    }

    if (s->index + 2 >= s->real) {
        s->real = s->real * 2;
        s->array = realloc(s->array, s->real);
        if (s->array == NULL) {
            fprintf(stderr, "Allocation problem");
            exit(1);
        }
    }
    s->array[s->index++] = c;
}

void string_free(t_string *s) {

    if (s->array == NULL) {
        fprintf(stderr, "Parameter NULL");
        exit(1);
    }

    free(s->array);
}

void string_print(t_string *s) {
    for (int i = 0; i < s->index; i++) {
        printf("%c", s->array[i]);
    }
}
