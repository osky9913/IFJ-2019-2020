/**
 *	@file   symtable.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Modul implementujici tabulku symbolu.
 *	@note	TODO: revidovat funkce, struktury
 */

#include "symtable.h"
#include <stdio.h>

int HTSIZE = 49157;


int hashCode(char *id) {
    int retval = 1;
    int keylen = strlen(id);
    for (int i = 0; i < keylen; i++)
        retval += id[i];
    return (retval % HTSIZE);
}


void symtable_init(hash_table *table) {
    if (table == NULL) {
        return;
    }

    for (int i = 0; i < HTSIZE; i++) {
        (*table)[i] = NULL;
    }
}


symbol_t *symtable_search(hash_table *table, char *id) {
    symbol_t *destination = (*table)[hashCode(id)];

    if (destination == NULL) {
        return NULL;
    };

    while (destination != NULL) {
        if (strcmp(id, destination->id) == 0) {
            return destination;
        }
        destination = destination->next;
    }
    return NULL;
}


//void symtable_insert ( hash_table* table, tKey key, tData data )
void symtable_insert(hash_table *table, char *id, symbol_type_t type, symbol_attributes attributes) {

    symbol_t *destination = symtable_search(table, id);

    // ak je zoznam  na indexe prazdny
    if (symtable_search(table, id) == NULL) {
        symbol_t *new_item = (symbol_t *) malloc(sizeof(symbol_t));
        if (new_item == NULL) {
            return;
        }
        new_item->id = id;
        new_item->type = type;
        new_item->attributes = attributes;

        new_item->next = (*table)[hashCode(id)];
        (*table)[hashCode(id)] = new_item;
        return;
    }

    // ak neni prazdny pozeram ci tam nemam rovnaky prvok s inymi datami
    if (destination != NULL &&
        strcmp(id, destination->id) == 0
            ) {
        destination->type = type;
        destination->attributes = attributes;
        return;
    }


}


void symtable_delete(hash_table *table, char *id) {

    // cielovy prvok pomocou pristupu indexoveho hasu
    symbol_t *destination = (*table)[hashCode(id)];

    //ak tam nic neni vrat sa
    if (destination == NULL) {
        return;
    }

    // ak prvy prvok je hladany zmaz ho
    if (strcmp(id, destination->id) == 0) {
        (*table)[hashCode(id)] = destination->next;
        free(destination);
        destination = NULL;
        return;
    }

    // ak neni prvy prvok hladany tak iteruj
    symbol_t *previous_item = destination;
    destination = destination->next;
    while (destination != NULL) {
        if (strcmp(id, destination->id) == 0) {
            previous_item->next = destination->next;
            free(destination);
            destination = NULL;
            return;
        }
        previous_item = destination;
        destination = destination->next;
    }
    return;
}


void symtable_clear_all(hash_table *table) {
    // ak je tabulka prazda tak null
    if (table == NULL) {
        return;
    }

    // inac uvolni kazdy item
    for (int i = 0; i < HTSIZE; i++) {
        while ((*table)[i] != NULL) {
            symbol_t *first = (*table)[i];
            symbol_t *second = first->next;
            (*table)[i] = second;
            free(first);
            first = NULL;
        }
    }
}
