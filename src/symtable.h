/**
 *	@file   symtable.h
 *	@author Simon Sedlacek Martin Osvald , xsedla1h , xosval03
 *	@date
 *	@brief Modul implementujici tabulku symbolu.
 */


#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "scanner.h"


#define SYM_TABLE_SIZE 49157


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
typedef struct symbol {
    char *id;
    symbol_type_t type;
    symbol_attributes attributes;
    struct symbol *next;
} symbol_t;


typedef symbol_t *hash_table[SYM_TABLE_SIZE];

extern int SYMTABLE_SIZE;

/* Hlavičky řešených procedur a funkcí. */

int hashCode(char *id);

void symtable_init(hash_table *table);

symbol_t *symtable_search(hash_table *table, char *id);

void symtable_insert(hash_table *table, char *id, symbol_type_t type, symbol_attributes attributes);

void symtable_delete(hash_table *table, char *id);

void symtable_clear_all(hash_table *table);

#endif 
