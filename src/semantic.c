/**
 *	@file   semantic.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Semantic analysis module
 */


#include "semantic.h"

bool check_if_defined_var(const char *id) {
    symbol_t *symbol = symtable_search(&table_global, id);
    if (symbol) {
        if (symbol->type == STYPE_VAR && symbol->attributes.var_att.defined) {
            return true;
        }
    }
    return false;
}

bool check_if_defined_func(const char *id) {
    symbol_t *symbol = symtable_search(&table_global, id);
    if (symbol) {
        if (symbol->type == STYPE_FUNC && symbol->attributes.func_att.defined) {
            return true;
        }
    }
    return false;
}

int add_symbol_var(const char *id) {
    symbol_t *symbol = symtable_search(&table_global, id);

    if (!in_function) { /* Global scope */
        if (symbol) {
            if (symbol->type == STYPE_FUNC) {

            fprintf(stderr, "Line 42 - Semantic error: ID '%s' "
                    "has already been defined as a function.\n", id);
            return ERROR_SEM_DEFINITION;
            }
        }
    } /*else { // Local scope
        if (symbol) {
        } else {
            if (!symtable_insert(&table_local, id, STYPE_FUNC, attributes)) {
                fprintf(stderr, "Line 42 - Internal error: Could not allocate memory "
                        "for a new function symbol.\n");
                return ERROR_INTERNAL;
            }
        }
    }*/
        

    return SUCCESS;
}

int add_symbol_function(const char *id) {
    if ((curr_symbol = symtable_search(&table_global, id))) {
        fprintf(stderr, "Line 42 - Semantic error: ID '%s' "
                "has already been defined.\n", id);
        return ERROR_SEM_DEFINITION;

    } else {
        func_att_t func_att = { .defined = true, .param_count = 0 };
        symbol_attributes attributes = { .func_att = func_att };

        if (!(curr_symbol = symtable_insert(&table_global, id, STYPE_FUNC, attributes))) {
            fprintf(stderr, "Line 42 - Internal error: Could not allocate memory "
                    "for a new function symbol.\n");
            return ERROR_INTERNAL;
        }
    }

    return SUCCESS;
}
