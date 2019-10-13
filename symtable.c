/**
 *	@file   symtable.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Modul implementujici tabulku symbolu.
 *	@note	TODO: revidovat funkce, struktury
 */

#include "htab.h"


bool htab_iterator_valid(htab_iterator_t it) {
    return it.ptr!=NULL;
}

bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2) {
    return it1.ptr==it2.ptr && it1.t == it2.t;
}

size_t htab_bucket_count(const htab_t *t) {
    if (t == NULL)
        return 0;
    return t->arr_size;
}

void htab_clear(htab_t *t) {
    if (t == NULL)
        return;

    struct htab_item *next;

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

void htab_free(htab_t *t) {
    htab_clear(t);
    free(t);
}

unsigned int htab_hash_function(const char *str) {
    uint32_t h = 0;
    const unsigned char *p;

    for (p = (const unsigned char*)str; *p != '\0'; p++)
        h = (65599 * h) + *p;

    return h;
}

htab_t *htab_init(size_t n) {
    if (n == 0) {
        fprintf(stderr, "Error: tabulka musi mit alespon jeden radek\n");
        return NULL;
    }

    htab_t *new = malloc(sizeof(htab_t) + sizeof(struct htab_item *) * n);

    if (new == NULL) {
        fprintf(stderr, "Error: nepodarilo se alokovat pamet pro tabulku\n");
        return NULL;
    }

    new->size = 0;
    new->arr_size = n;

    for (size_t i = 0; i < n; i++) {
        new->array[i] = NULL;
    }

    return new;
}

htab_iterator_t htab_begin(const htab_t *t) {
    htab_iterator_t begin;
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

htab_iterator_t htab_end(const htab_t *t) {

    htab_iterator_t end;
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

const char *htab_iterator_get_key(htab_iterator_t it) {
    assert(it.ptr != NULL);
    return it.ptr->key;
}

htab_iterator_t htab_iterator_next(htab_iterator_t it) {
    if (!htab_iterator_valid(it))
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

        it = htab_end(it.t);

    } else {
        it.ptr = it.ptr->next;
    }
    
    return it;
}

int htab_iterator_get_value(htab_iterator_t it) {
    assert(it.ptr != NULL);
    return it.ptr->data;
}

int htab_iterator_set_value(htab_iterator_t it, int val) {
    assert(it.ptr != NULL);
    return it.ptr->data = val;
}

htab_iterator_t htab_lookup_add(htab_t *t, const char *key) {
    if (t == NULL || key == NULL) {
        fprintf(stderr, "Error: tabulka nebo klic neexistuji\n");
        return htab_end(t);
    }

    int index = (htab_hash_function(key) % t->arr_size);
    htab_iterator_t it = { .ptr = t->array[index], .t = t, .idx = index };

    /* Projdeme cely seznam na danem indexu a zjistime, zda se v tabulce jiz
     * dane slovo nachazi */
    while (htab_iterator_valid(it) && it.idx == index) {
        if (!strcmp(htab_iterator_get_key(it), key)) {
            return it;
        }

        it = htab_iterator_next(it);
    }

    /* Tabulka danou polozku neobsahovala, tudiz musime polozku pridat */
    it.ptr = malloc(sizeof(struct htab_item));
    if (it.ptr == NULL) {
        fprintf(stderr, "Error: nepodarilo se vlozit klic do tabulky\n");
        return htab_end(t);
    }

    /* Alokace a kontrola alokace pameti pro novy zaznam */
    it.ptr->key = malloc(strlen(key) + 1);

    if (htab_iterator_get_key(it) == NULL) {
        free(it.ptr);
        return htab_end(t);
    }

    /* Inicializace nove polozky a uprava dat tabulky */
    strcpy(it.ptr->key, key);
    htab_iterator_set_value(it, 0);
    it.ptr->next = t->array[index];
    t->array[index] = it.ptr;
    t->size++;
    return it;
}

htab_t *htab_move(size_t n, htab_t *from) {
    if (from == NULL || n == 0)
        return NULL;

    htab_t *new = htab_init(n);

    if (new == NULL) {
        fprintf(stderr, "Error: nepodarilo se alokovat pamet "
                "pro novou tabulku\n");
        return NULL;
    }

    /* Pomocne iteratory pro prochazeni puvodni tabulkou a
     * kontrolu operace vlozeni zaznamu do nove tabulky */
    htab_iterator_t iterator, check;

    /* Vsechny polozky z puvodni tabulky postupne vlozime do nove tabulky */
    for (iterator = htab_begin(from);
            htab_iterator_valid(iterator);
            iterator = htab_iterator_next(iterator)) {

        check = htab_lookup_add(new, iterator.ptr->key);
        
        if (!htab_iterator_valid(check)) {
            fprintf(stderr, "Error: nepodarilo se presunout polozku "
                    "do nove tabulky\n");
            htab_clear(new);
            return NULL;
        }

        htab_iterator_set_value(check, htab_iterator_get_value(iterator));
    }

    htab_clear(from);
    return new;
}

size_t htab_size(const htab_t *t) {
    if (t == NULL)
        return 0;
    return t->size;
}
