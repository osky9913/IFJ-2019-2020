//
// Created by root on 10/17/19.
//

#ifndef UNTITLED_DYNAMIC_STRING_H
#define UNTITLED_DYNAMIC_STRING_H

#include <stdlib.h>
#include <stdio.h>

struct tString {
    char *array;
    int index;
    int real;
} t_string;


void string_init(struct tString *s);

void string_append(struct tString *s, char c);

void string_print(struct tString *s);

void string_free(struct tString *s);






#endif //UNTITLED_DYNAMIC_STRING_H
