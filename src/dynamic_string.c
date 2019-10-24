/**
 * @file dynamic_string.c
 * @brief This module implements all the necessary operations needed for manipulating
 * our dynamic string implementation.
 * @author xosval03, xsedla1h
 */

#include "dynamic_string.h"

string_t *string_create_init() {
    string_t *s = malloc(sizeof(string_t));
    if (s == NULL) {
        fprintf(stderr, "String ERROR: Could not allocate memory for a string.\n");
        return NULL;
    }

    if (string_init(s) == STR_ALLOC_ERROR) {
        string_free(s);
        return NULL;
    }
    
    return s;
}

int string_init(string_t *s) {
    s->index = 0;
    s->real = STR_DEFAULT_LEN;
    s->array = calloc(STR_DEFAULT_LEN, 1);
    if (s->array == NULL) {
        fprintf(stderr, "String ERROR: Could not init string.\n");
        return STR_ALLOC_ERROR;
    }
    return SUCCESS;
}


int string_append_char(string_t *s, char c) {
    if (s == NULL) {
        fprintf(stderr, "String WARNING: A pointer passed to string_append was NULL.\n");
        return STR_NULL_PASSED;
    }

    if (s->index + 2 >= s->real) {
        s->real = s->real * 2;
        char *tmp = realloc(s->array, s->real);
        if (!tmp) {
            fprintf(stderr, "String ERROR: Could not append char to string.\n");
            return STR_ALLOC_ERROR;
        }
        s->array = tmp;
    }
    sprintf(&(s->array[s->index++]), "%c%c", c, '\0');
    return SUCCESS;
}



void string_print(string_t *s, const char *append) {
    if (s == NULL) {
        fprintf(stderr, "String WARNING: A pointer passed to string_print was NULL.\n");
        return;
    }
    printf("%s%s", s->array, append);
}

void string_free(string_t *s) {
    if (s) {
        free(s->array);
        free(s);
    } else {
        fprintf(stderr, "String WARNING: A pointer passed to string_free was NULL.\n");
    }
}

void string_free_array(string_t *s) {
    if (s) {
        free(s->array);
        s->real = 0;
        s->index = 0;
    } else {
        fprintf(stderr, "String WARNING: A pointer passed to\
                string_free_array was NULL.\n");
    }
}

void string_clear(string_t *s) {
    if (s) {
        s->array[0] = '\0';
        s->index = 0;
    } else {
        fprintf(stderr, "String WARNING: A pointer passed to string_clear was NULL.\n");
    }
}

char *string_copy_data(string_t *s) {
    if (s == NULL) {
        fprintf(stderr, "String WARNING: A pointer passed to string_clear was NULL.\n");
        return NULL;
    }

    char *new = calloc(s->index + 1, 1);
    if (new == NULL) {
        fprintf(stderr, "String ERROR: Could not allocate memory for\
                the copy of the string.\n");
        return NULL;
    }
    strcpy(new, s->array);
    return new;
}
