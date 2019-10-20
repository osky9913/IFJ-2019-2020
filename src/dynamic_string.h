//
// Created by root on 10/17/19.
//

#ifndef UNTITLED_DYNAMIC_STRING_H
#define UNTITLED_DYNAMIC_STRING_H

#include <stdlib.h>
#include <stdio.h>

typedef struct{
    char *array;
    int index;
    int real;
} t_string;


void string_init(t_string *s);

void string_append(t_string *s, char c);

void string_print(t_string *s);

void string_free(t_string *s);

#endif //UNTITLED_DYNAMIC_STRING_H
