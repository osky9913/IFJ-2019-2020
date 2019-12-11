/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   symbol_list.h
 * @author Simon Sedlacek, xsedla1h
 * @brief A module implementing a list of symbols. This module is used solely
 * for avoiding infinite recursion when checking the dependencies of functions.
 */

#ifndef __SYMBOL_LIST_H__
#define __SYMBOL_LIST_H__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symtable.h"
#include "errors.h"

/**
 * @brief This is the structure representing the list of symbols.
 */
typedef struct symbol_node {
    const char *symbol_id;
    struct symbol_node *next;
} snode_t;

/**
 * @biref Inits a new list node and returns a pointer to it.
 */
snode_t *list_init(const char *id);

/**
 * @brief Tries to insert a new list node to the list.
 */
int list_insert_symbol(snode_t *list, const char *id);

/**
 * @brief This function checks a list for an identifier,
 * if the identifier is present in the list, the function
 * returns true.
 */
bool list_contains(snode_t *list, const char *id);

/**
 * @brief Frees the symbol list.
 */
void list_free(snode_t *list);

#endif
