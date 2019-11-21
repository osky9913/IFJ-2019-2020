/**
 *	@file   symtable.c
 *	@author Simon Sedlacek, Martin Osvald
 *	@date
 *	@brief Modul implementujici tabulku symbolu.
 *	@note	TODO: revidovat funkce, struktury
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
        for (int i = 0; i < SYMTABLE_SIZE; i++) {
            (*table)[i] = NULL;
        }
    }
}


symbol_t *symtable_search(symtable_t *table, const char *id) {
    symbol_t *target = (*table)[symtable_hash(id)];

    while (target != NULL) {

        if (!strcmp(id, target->id))
            return target;

        target = target->next;
    }
    return NULL;
}


symbol_t *symtable_insert(symtable_t *table, const char *id, symbol_type_t type, symbol_attributes attributes) {

    symbol_t *target = symtable_search(table, id);
    
    if (target) {
        target->type = type;
        target->attributes = attributes;
        return target;

    } else {

        int index = symtable_hash(id);

        symbol_t *new_item = malloc(sizeof(symbol_t));
        if (new_item == NULL)
            return NULL;

        new_item->id = malloc(strlen(id) + 1); 
        strcpy(new_item->id, id);
        new_item->type = type;
        new_item->attributes = attributes;

        new_item->next = NULL;
        (*table)[index] = new_item;
        return new_item;
    }

    return target;
}


void symtable_delete(symtable_t *table, const char *id) {
    symbol_t *target = (*table)[symtable_hash(id)];

    symbol_t *previous = NULL;
    while (target != NULL) {

        if (!strcmp(id, target->id)) {

            if (previous == NULL) {
                (*table)[symtable_hash(id)] = target->next;

            } else {
                previous->next = target->next;
            }

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
            while ((*table)[i] != NULL) {

                symbol_t *first = (*table)[i];
                symbol_t *second = first->next;
                (*table)[i] = second;
                free(first);
            }
        }
    }
}

