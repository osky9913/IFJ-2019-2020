/**
 *	@file   symtable.h
 *	@author	Simon Sedlacek, xsedla1h
 *	@date	
 *	@brief  Hlavickovy soubor pro symtable.c
 *	@note	TODO: revidovat funkce, struktury
 */

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/**
 * @struct htab
 * @brief Struktura hashovaci tabulky.
 *
 * @var size
 * Aktualni pocet zaznamu v tabulce.
 * @var arr_size
 * Pocet radku tabulky.
 * @var array
 * Pole seznamu se zaznamy.
 */
typedef struct htab {
    size_t size;
    size_t arr_size;
    struct htab_item *array[];
} htab_t;

/**
 * @struct htab_item
 * @brief Struktura zaznamu v hashovaci tabulce
 *
 * @var key
 * Klic daneho zaznamu.
 * @var data
 * Pocet zaznamu vazajicich se k danemu klici.
 * @var next
 * Ukazatel na dalsi polozku seznamu.
 */
struct htab_item {
    char *key;
    int data;
    struct htab_item *next;
};

/**
 * @struct htab_iterator
 * @brief Struktura iteratoru pro pruchod tabulkou
 *
 * @var ptr
 * Ukazatel na aktualni polozku v tabulce.
 * @var t
 * Ukazatel na prochazenou tabulku.
 * @var idx
 * Index urcujici, v kterem seznamu tabulky se polozka nachazi.
 */
typedef struct htab_iterator {
    struct htab_item *ptr;
    const htab_t *t;
    int idx;
} htab_iterator_t;

// rozptylovací (hash) funkce
unsigned int htab_hash_function(const char *str);

// funkce pro práci s tabulkou:
htab_t *htab_init(size_t n);
htab_t *htab_move(size_t n, htab_t *from);
void htab_clear(htab_t *t);
void htab_free(htab_t *t);

size_t htab_size(const htab_t *t);             // počet záznamů v tabulce
size_t htab_bucket_count(const htab_t *t);     // velikost pole

htab_iterator_t htab_lookup_add(htab_t *t, const char *key);
htab_iterator_t htab_begin(const htab_t *t);
htab_iterator_t htab_end(const htab_t *t);
htab_iterator_t htab_iterator_next(htab_iterator_t it);

bool htab_iterator_valid(htab_iterator_t it);
bool htab_iterator_equal(htab_iterator_t it1, htab_iterator_t it2);

const char *htab_iterator_get_key(htab_iterator_t it);
int htab_iterator_get_value(htab_iterator_t it);
int htab_iterator_set_value(htab_iterator_t it, int val);

#endif
