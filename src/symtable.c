/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   symtable.c
 * @author Simon Sedlacek, xsedla1h
 * @brief This module implements the table of symbols
 */

#include "symtable.h"

unsigned int symtable_hash(const char *id) {
    unsigned long hash = 0, high;

    while (*id) {
        hash = (hash << 4) + *id++;
        if ((high = hash & 0xF0000000))
            hash ^= high >> 24;
        hash &= ~high;
    }

    return hash % SYMTABLE_SIZE;
}


void symtable_init(symtable_t *table) {
    if (table != NULL) {
        /* Initialize the symtable bucket array */
        for (int i = 0; i < SYMTABLE_SIZE; i++) {
            (*table)[i] = NULL;
        }
    }
}


symbol_t *symtable_search(symtable_t *table, const char *id) {
    /* Get the index */
    symbol_t *target = (*table)[symtable_hash(id)];

    /* Scan through the list */
    while (target != NULL) {
        if (!strcmp(id, target->id))
            return target;

        target = target->next;
    }
    return NULL;
}


symbol_t *symtable_insert(symtable_t *table, const char *id, symbol_type_t type, symbol_attributes attributes) {

    symbol_t *target = symtable_search(table, id);
    
    /* If the target symbol is already in the table, update it's
     * attributes */
    if (target) {
        target->type = type;
        target->attributes = attributes;

    } else {
        /* Otherwise, create a new entry */
        int index = symtable_hash(id);

        symbol_t *new_item = malloc(sizeof(symbol_t));
        if (new_item == NULL)
            return NULL;

        if (!(new_item->id = malloc(strlen(id) + 1))) {
            free(new_item);
            return NULL;
        }

        strcpy(new_item->id, id);
        new_item->type = type;
        new_item->attributes = attributes;

        /* If the new symbol is to be a function, allocate a dependency list for it */
        if (type == STYPE_FUNC) {
            if (!(new_item->attributes.func_att.depends = malloc(sizeof(symbol_t *)
                            * DEPEND_LEN))) {
                free(new_item->id);
                free(new_item);
                return NULL;
            }
            new_item->attributes.func_att.dep_len = 0;
        }

        /* Insert the new item in the table */
        new_item->next = (*table)[index];
        (*table)[index] = new_item;
        return new_item;
    }

    return target;
}


void symtable_delete(symtable_t *table, const char *id) {
    /* Get the pointer to the correct list in the table */
    symbol_t *target = (*table)[symtable_hash(id)];

    symbol_t *previous = NULL;
    while (target != NULL) {

        /* Now scan the list for the desired item */
        if (!strcmp(id, target->id)) {

            /* If the deleted item is at the front of the list */
            if (previous == NULL) {
                (*table)[symtable_hash(id)] = target->next;

            } else {
                previous->next = target->next;
            }

            /* Free the item */
            if (target->type == STYPE_FUNC)
                free(target->attributes.func_att.depends);
            free(target->id);
            free(target);
            return;
        }
        previous = target;
        target = target->next;
    }
}


void symtable_clear_all(symtable_t *table) {
    if (table) {
        for (int i = 0; i < SYMTABLE_SIZE; i++) {

            /* Free every item in the list on the current index */
            while ((*table)[i] != NULL) {

                symbol_t *first = (*table)[i];
                symbol_t *second = first->next;
                (*table)[i] = second;

                if (first->type == STYPE_FUNC) /* free the dependency list */
                    free(first->attributes.func_att.depends);

                free(first->id);
                free(first);
            }
        }
    }
}
