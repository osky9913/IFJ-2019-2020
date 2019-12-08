/**
 *	@file   symbol_list.h
 *	@author Simon Sedlacek, xsedla1h
 *	@brief A module implementing a list of symbols. This module is used solely
 *	for avoiding infinite recursion when checking the dependencies of functions.
 */

#ifndef __SYMBOL_LIST_H__
#define __SYMBOL_LIST_H__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "symtable.h"
#include "errors.h"

typedef struct symbol_node {
    const char *symbol_id;
    struct symbol_node *next;
} snode_t;

snode_t *list_init(const char *id);

int list_insert_symbol(snode_t *list, const char *id);

bool list_contains(snode_t *list, const char *id);

void list_free(snode_t *list);

#endif
