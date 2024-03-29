/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   symbol_list.c
 * @author Simon Sedlacek, xsedla1h
 * @brief A module implementing a list of symbols used to check
 * recursive function calls
 */

#include "symbol_list.h"

snode_t *list_init(const char *id) {
    /* Allocate memory for the new list */
    snode_t *list = malloc(sizeof(snode_t));
    if (!list) return NULL;

    /* Initialize the list attributes */
    list->symbol_id = id;
    list->next = NULL;
    return list;
}

bool list_contains(snode_t *list, const char *id) {
    snode_t *iterator = list;

    /* Scan through the list for the matching identifier */
    while (iterator) {
        if (!iterator->symbol_id) return false;
        if (!strcmp(id, iterator->symbol_id)) return true;
        iterator = iterator->next;
    }
    return false;
}

int list_insert_symbol(snode_t *list, const char *id) {
    /* Alloc the memory for the new node */
    snode_t *new = malloc(sizeof(snode_t));
    if (!new) return ERROR_INTERNAL;

    new->symbol_id = id;
    new->next = NULL;

    /* Go to the end of the list */
    snode_t *iterator = list;
    while (iterator->next) {
        iterator = iterator->next;
    }

    /* Append the new node */
    iterator->next = new;
    return SUCCESS;
}


void list_free(snode_t *list) {
    if (!list) return;
    snode_t *tmp1 = list;
    snode_t *tmp2;

    do {
        tmp2 = tmp1->next;
        free(tmp1);
        tmp1 = tmp2;
    } while (tmp1);
}
