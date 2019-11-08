/**
 *	@file   symtable.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Modul implementujici tabulku symbolu.
 *	@note	TODO: revidovat funkce, struktury
 */

#include "symtable.h"


bool symtable_iterator_valid(symtable_iterator_t it) {
    return it.ptr!=NULL;
}

bool symtable_iterator_equal(symtable_iterator_t it1, symtable_iterator_t it2) {
    return it1.ptr==it2.ptr && it1.t == it2.t;
}

void symtable_clear(symtable_t *t) {
    if (t == NULL)
        return;

    symbol_t *next;

    for (size_t i = 0; i < t->arr_size; i++) {

        /* Uvolnime obsah kazdeho radku tabulky
         * a nasledne radek samotny */
        while (t->array[i] != NULL) {
            next = t->array[i]->next;
            free(t->array[i]->key);
            free(t->array[i]);
            t->array[i] = next;
        }
    }

    t->size = 0;
}

void symtable_free(symtable_t *t) {
    symtable_clear(t);
    free(t);
}

unsigned int symtable_elf_hash(const char *str) {
    unsigned long   h = 0, high;

    while (*str) {
        h = (h << 4) + *str++;
        if (high = h & 0xF0000000)
            h ^= high >> 24;
        h &= ~high;
    }

    return h;
}

void symtable_init(symtable_t *t) {
    t->size = 0;
    t->arr_size = SYMTABLE_SIZE;

    for (size_t i = 0; i < SYMTABLE_SIZE; i++) {
        t->array[i] = NULL;
    }
}

symtable_iterator_t symtable_begin(const symtable_t *t) {
    symtable_iterator_t begin;
    begin.ptr = NULL;
    begin.t = NULL;
    begin.idx = 0;

    if (t == NULL)
        return begin;

    begin.t = t;

    /* Najdeme prvni validni zaznam v tabulce */
    for (size_t i = 0; i < t->arr_size; i++) {
        if ((begin.ptr = t->array[i]) != NULL) {
            begin.idx = i;
            break;
        }
    }

    return begin;
}

symtable_iterator_t symtable_end(const symtable_t *t) {

    symtable_iterator_t end;
    end.ptr = NULL;

    if (t == NULL) {
        end.t = NULL;
        end.idx = 0;
    } else {
        end.t = t;
        end.idx = t->arr_size - 1;
        for (int i = t->arr_size - 1; i >= 0; i--) {
            end.idx = i;
            if (t->array[i] != NULL)
                break;
        }
    }

    return end;
}

const char *symtable_iterator_get_key(symtable_iterator_t it) {
    assert(it.ptr != NULL);
    return it.ptr->key;
}

symtable_iterator_t symtable_iterator_next(symtable_iterator_t it) {
    if (!symtable_iterator_valid(it))
        return it;

    /* V pripade, ze aktualni zaznam je v seznamu posledni */
    if (it.ptr->next == NULL) {

        /* Pokud se zaznam nenachazi na poslednim radku tabulky,... */
        if ((size_t)it.idx < it.t->arr_size - 1)

            /* ...najdeme nejblizsi dalsi validni zaznam v tabulce */
            for (size_t i = it.idx + 1; i < it.t->arr_size; i++) {
                if ((it.ptr = it.t->array[i]) != NULL) {
                    it.idx = i;
                    return it;
                }
            }

        it = symtable_end(it.t);

    } else {
        it.ptr = it.ptr->next;
    }
    
    return it;
}

int symtable_iterator_get_value(symtable_iterator_t it) {
    assert(it.ptr != NULL);
    return it.ptr->data;
}

int symtable_iterator_set_value(symtable_iterator_t it, int val) {
    assert(it.ptr != NULL);
    return it.ptr->data = val;
}

symtable_iterator_t symtable_lookup_add(symtable_t *t, const char *key) {
    if (t == NULL || key == NULL) {
        fprintf(stderr, "Error: tabulka nebo klic neexistuji\n");
        return symtable_end(t);
    }

    int index = (symtable_hash_function(key) % t->arr_size);
    symtable_iterator_t it = { .ptr = t->array[index], .t = t, .idx = index };

    /* Projdeme cely seznam na danem indexu a zjistime, zda se v tabulce jiz
     * dane slovo nachazi */
    while (symtable_iterator_valid(it) && it.idx == index) {
        if (!strcmp(symtable_iterator_get_key(it), key)) {
            return it;
        }

        it = symtable_iterator_next(it);
    }

    /* Tabulka danou polozku neobsahovala, tudiz musime polozku pridat */
    it.ptr = malloc(sizeof(symbol__t));
    if (it.ptr == NULL) {
        fprintf(stderr, "Error: nepodarilo se vlozit klic do tabulky\n");
        return symtable_end(t);
    }

    /* Alokace a kontrola alokace pameti pro novy zaznam */
    it.ptr->key = malloc(strlen(key) + 1);

    if (symtable_iterator_get_key(it) == NULL) {
        free(it.ptr);
        return symtable_end(t);
    }

    /* Inicializace nove polozky a uprava dat tabulky */
    strcpy(it.ptr->key, key);
    symtable_iterator_set_value(it, 0);
    it.ptr->next = t->array[index];
    t->array[index] = it.ptr;
    t->size++;
    return it;
}

size_t symtable_size(const symtable_t *t) {
    if (t == NULL)
        return 0;
    return t->size;
}
