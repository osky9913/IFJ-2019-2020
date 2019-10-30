/**
 * @file dynamic_string.h
 * @brief A header file for dynamic_string.c
 * @author xosval03, xsedla1h
 */

#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Default string length for initialization */
#define STR_DEFAULT_LEN 20

/* Return codes */
#define SUCCESS 0
#define STR_ALLOC_ERROR 1
#define STR_NULL_PASSED 2

/**
 * @brief This structure represents our dynamic string.
 */
typedef struct String {
    char *array;
    int index;
    int real;
} string_t;


/**
 * @brief Allocates memory for a new string and initializes it.
 * @return Pointer to the new string or NULL if the operation fails.
 */
string_t *string_create_init();

/**
 * @brief Initializes s to the base values for a new string.
 */
int string_init(string_t *s);

/**
 * @brief Appends a character to s.
 */
int string_append_char(string_t *s, char c);

/**
 * @brief Prints out s->array.
 */
void string_print(string_t *s, const char *append);

/**
 * @brief Frees all the memory allocated for s->array and for s itself.
 */
void string_free(string_t *s);

/**
 * @brief Frees the memory allocated for s->array, sets s->index and s->real to zero.
 */
void string_free_array(string_t *s);

/**
 * @brief Reinits the string - clears the character array, sets index to 0.
 */
void string_clear(string_t *s);

/**
 * @brief Allocates memory for a new array of chars, then copies
 * the content of s->array into the new char array.
 * @return Returns a pointer to the new char array or NULL if the operation could
 * not be completed.
 */
char *string_copy_data(string_t *s);

#endif
