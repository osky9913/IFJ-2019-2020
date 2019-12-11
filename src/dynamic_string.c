/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file dynamic_string.c
 * @author Martin Osvald - xosval03, Simon Sedlacek - xsedla1h
 * @brief This module implements all the necessary operations needed for manipulating
 * our dynamic string implementation.
 */


#include "dynamic_string.h"

string_t *string_create_init(void) {
    /* Allocate memory for the string_t structure */
    string_t *s = malloc(sizeof(string_t));
    if (s == NULL) {
        fprintf(stderr, "String ERROR: Could not allocate memory for a string.\n");
        return NULL;
    }

    /* Now init the new string */
    if (string_init(s) == STR_ALLOC_ERROR) {
        string_free(s);
        return NULL;
    }
    
    return s;
}

int string_init(string_t *s) {
    /* Set the default string attributes */
    s->index = 0;
    s->real = STR_DEFAULT_LEN;
    /* Allocate memory for the string array */
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
        /* Free the whole structure */
        string_free_array(s);
        free(s);
    } else {
        fprintf(stderr, "String WARNING: A pointer passed to string_free was NULL.\n");
    }
}

void string_free_array(string_t *s) {
    if (s) {
        /* Reset the string to it's original state before initialization */
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
        /* Fill the string array with zeroes and reset the index */
        for(int i = 0; i < s->index; i++)
            s->array[i] = '\0';
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

    /* Create a new array for the copied data - we need a deep copy */
    char *new = calloc(s->index + 1, 1);
    if (new == NULL) {
        fprintf(stderr, "String ERROR: Could not allocate memory for\
                the copy of the string.\n");
        return NULL;
    }
    strcpy(new, s->array);
    return new;
}

int str_find_char(string_t* s, char c) {
    int charFound = 0;
    
    /* Scan the string to find a character in it */
    for(int i = 0; i < s->index; i++){
        if(s->array[i] == c){
            charFound++;
        }
    }
    return charFound;
}

void create_unique_variable(string_t *result, int *unique, char* base_name) {

    /* Create a temporary string to convert the uniqueue parameter to a string */
    char number_string[30] = {0};
    sprintf(number_string, "%d", *unique);

    result->index = 0;
    string_append(result, base_name);
    string_append(result, number_string);
    *unique = *unique + 1;
}


void create_unique_label(string_t *result, int *unique, char *base_name) {

    /* Create a temporary string to convert the uniqueue parameter to a string */
    char number_string[30] = {0};
    sprintf(number_string, "%d", *unique);

    result->index = 0;
    string_append(result, base_name);
    string_append(result, number_string);
}


void string_append(string_t *s1, const char *s2) {
    unsigned len = strlen(s2);

    /* Append s2 to s1 char by char */
    for (unsigned i = 0; i < len; i++) {
        if (string_append_char(s1, s2[i]) == STR_ALLOC_ERROR)
            exit(ERROR_INTERNAL);
    }
}
