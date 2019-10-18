//
// Created by root on 10/17/19.
//

#include "dynamic_string.h"


void string_init(struct tString *s) {
    s->index = 0;
    s->real = 100;
    s->array = calloc(s->real, 1);
    if (s->array == NULL) {
        fprintf(stderr, "Allocation problem");
        exit(1);
    }
}


void string_append(struct tString *s, char c) {

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


void string_free(struct tString *s) {

    if (s->array == NULL) {
        fprintf(stderr, "Parameter NULL");
        exit(1);
    }

    free(s->array);
}


void string_print(struct tString *s) {
    for (int i = 0; i < s->index; i++) {
        printf("%c", s->array[i]);
    }
}