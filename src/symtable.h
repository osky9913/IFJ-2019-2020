/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   symtable.h
 * @author Simon Sedlacek, xsedla1h
 * @brief A header file for the symtable module
 */


#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "errors.h"

/* Default symtable size */
#define  SYMTABLE_SIZE 12289

/**
 * @brief Represents the type of the symbol
 */
typedef enum {
    STYPE_VAR,
    STYPE_FUNC
} symbol_type_t;

/**
 * @brief Represents the type of the variable
 */
typedef enum {
    VTYPE_INT,
    VTYPE_DBL,
    VTYPE_STR,
    VTYPE_UNKNOWN
} var_type_t;

/**
 * @struct variable_attributes
 * @brief This structure represents the variable symbol attributes
 *
 * @var defined Indicates whether the variable has been defined
 * @var type Variable type
 */
typedef struct variable_attributes {
    var_type_t type;
    bool defined;
} var_att_t;

/**
 * @struct function_attributes
 * @brief This structure represents the function symbol attributes
 *
 * @var defined Indicates whether the function has been defined
 * @var param_count The number of parameters this function should be called with
 * @var depends Array of pointers to other functions that have to be
 * defined when calling this function.
 * @var dep_len The number of dependencies in depends.
 */
typedef struct function_attributes {
    bool defined;
    int param_count;
    struct symbol **depends;
    unsigned dep_len;
} func_att_t;

/* The default length of the function dependency array */
#define DEPEND_LEN 20

/**
 * @union symbol_attributes
 * @brief This union stores the symbol attributes
 *
 * @var var_att Variable attributes
 * @var func_att Function attributes
 */
typedef union {
    var_att_t var_att;
    func_att_t func_att;
} symbol_attributes;

/**
 * @struct symbol
 * @brief This structure represents a symbol in the symbol table
 *
 * @var id   Symbol identifier
 * @var type The type of the symbol
 * @var attributes A union that stores the symbol attributes
 * @var next A pointer to the next symbol in the symbol table
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
 * @brief The hash function used to obtain the table index for a key
 * @return Returns the table index for a specific key
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
 * @note This function is not used at all in the compiler currently, we just implemented
 * it and left it here in case we needed it in the future.
 */
void symtable_delete(symtable_t *table, const char *id);

/**
 * @brief Deletes all symbols from a table.
 */
void symtable_clear_all(symtable_t *table);

#endif 
