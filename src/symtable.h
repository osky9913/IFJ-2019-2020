/**
 *	@file   symtable.h
 *	@author Simon Sedlacek Martin Osvald , xsedla1h , xosval03
 *	@date
 *	@brief Modul implementujici tabulku symbolu.
 */


#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "errors.h"

#define  SYMTABLE_SIZE 12289

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
    VTYPE_STR,
    VTYPE_UNKNOWN
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
    var_type_t type;
    bool defined;
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
    unsigned param_count;
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


/** 
 * @brief A static typedef for our symtable.
 */
typedef symbol_t *symtable_t[SYMTABLE_SIZE];


/**
 * @brief Hash funkce pro ziskani indexu do tabulky symbolu.
 * @return Vraci index do tabulky pro zadany klic
 */
unsigned int symtable_hash(const char *key);

/**
 * @brief Initializes a pre-allocated hash table.
 * @param table Table we need to init.
 */
void symtable_init(symtable_t *table);

/**
 * @brief Searches the hash table for an item.
 * @return Returns pointer to the found item or NULL if the item couldn't be found.
 */
symbol_t *symtable_search(symtable_t *table, const char *id);

/**
 * @brief Inserts a new symbol in the table, updates its attributes.
 * @return Returns a pointer to the inserted item or NULL if something goes wrong. 
 */
symbol_t *symtable_insert(symtable_t *table, const char *id, symbol_type_t type, symbol_attributes attributes);

/**
 * @brief Deletes a symbol from the table.
 */
void symtable_delete(symtable_t *table, const char *id);

/**
 * @brief Deletes all symbols from a table.
 */
void symtable_clear_all(symtable_t *table);

#endif 
