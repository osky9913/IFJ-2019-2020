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
#include "scanner.h"

#define SYMTABLE_SIZE 1233

/**
 * @brief Reprezentuje typ symbolu.
 */
typedef enum {
    STYPE_VAR,
    STYPE_FUNC,
    STYPE_LABEL
} symbol_type_t;

/**
 * @brief Reprezentuje typ promenne.
 */
typedef enum {
    VTYPE_INT,
    VTYPE_DBL,
    VTYPE_STR
} var_type_t;


/**
 * @struct variable_attributes
 * @brief Struktura reprezentujici atributy promenne
 *
 * @var defined Indikuje, zda byla promenna jiz definovana
 * @var type Typ promenne
 * @var scope Reprezentuje scope promenne
 * @var int_value Int hodnota promenne
 * @var double_value Double hodnota promenne
 * @var string_value String hodnota promenne
 */
typedef struct variable_attributes {
    bool defined;
    var_type_t type;
    int scope; // nejsem si jisty, jak to zatim reprezentovat TODO
    union {
        int int_value;
        double double_value;
        char *string_value;
    };
} var_att_t;

/**
 * @struct function_attributes
 * @brief Struktura reprezentujici atributy funkce
 *
 * @var defined Indikuje, zda byla funkce jiz definovana
 * @var param_count Pocet parametru deklarovany pri definici funkce
 */
typedef struct function_attributes {
    bool defined;
    int param_count;
} func_att_t;

/**
 * @struct label_attributes
 * @brief Struktura reprezentujici atributy navesti
 *
 * @var defined Indikuje, zda bylo navesti jiz definovane
 */
typedef struct label_attributes {
    bool defined;
} label_att_t;

/**
 * @union symbol_attributes
 * @brief Unie uchovavajici atributy promenne
 *
 * @var var_att Atributy promenne
 * @var func_att Atributy funkce
 * @var label_att Atributy navesti
 */
typedef union {
    var_att_t var_att;
    func_att_t func_att;
    label_att_t label_att;
} symbol_attributes;
/**
 * @struct symbol
 * @brief Struktura reprezentujici jeden symbol
 *
 * @var id   Identifikator symbolu
 * @var type Typ symbolu
 * @var attributes Unie uchovavajici informace o symbolu potrebne pro semantickou analyzu
 * @var data Pocet zaznamu vazajicich se k danemu klici.
 * @var next Ukazatel na dalsi polozku seznamu.
 */
struct symbol {
    char *id;
    symbol_type_t type;
    symbol_attributes attributes;
    int data;
    struct symbol *next;
} symbol_t;

/**
 * @struct symtable
 * @brief Struktura hashovaci tabulky.
 *
 * @var size     Aktualni pocet zaznamu v tabulce.
 * @var arr_size Pocet radku tabulky.
 * @var array    Pole seznamu se zaznamy.
 */
typedef struct symtable {
    size_t size;
    symbol_t *array[SYMTABLE_SIZE];
} symtable_t;

/**
 * @struct symtable_iterator
 * @brief Struktura iteratoru pro pruchod tabulkou
 *
 * @var ptr Ukazatel na aktualni polozku v tabulce.
 * @var t   Ukazatel na prochazenou tabulku.
 * @var idx Index urcujici, v kterem seznamu tabulky se polozka nachazi.
 */
typedef struct symtable_iterator {
    symbol_t *ptr;
    const symtable_t *t;
    unsigned idx;
} symtable_iterator_t;

/**
 * @brief Hash funkce pro ziskani indexu do tabulky symbolu.
 *
 * @return Vraci hash hodnotu pro zadany klic
 */
unsigned int symtable_elf_hash(const char *str);

// funkce pro práci s tabulkou:
void symtable_init(symtable_t *t);
void symtable_clear(symtable_t *t); // TODO
void symtable_free(symtable_t *t); // TODO

size_t symtable_size(const symtable_t *t); // počet záznamů v tabulce

symtable_iterator_t symtable_lookup_add(symtable_t *t, const char *key); // TODO
symtable_iterator_t symtable_begin(const symtable_t *t);
symtable_iterator_t symtable_end(const symtable_t *t);
symtable_iterator_t symtable_iterator_next(symtable_iterator_t it);

bool symtable_iterator_valid(symtable_iterator_t it);
bool symtable_iterator_equal(symtable_iterator_t it1, symtable_iterator_t it2);

const char *symtable_iterator_get_key(symtable_iterator_t it);
int symtable_iterator_get_value(symtable_iterator_t it);
int symtable_iterator_set_value(symtable_iterator_t it, int val);

#endif
