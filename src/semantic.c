/**
 *	@file   semantic.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Semantic analysis module
 */


#include "semantic.h"

//TODO: check for functions first


int check_and_add_parameter_def(const char *id) {
    symbol_t *symbol = symtable_search(&table_global, id); /* Now check for functions */
    if (symbol) {
        if (symbol->type == STYPE_FUNC) {
            fprintf(stderr, "Line %d - Function parameter %s has already been defined "
                    "as a function.\n", line_counter, id);
            return ERROR_SEM_DEFINITION;
        }
    }

    /* Now check for duplicit parameters */
    symbol = symtable_search(&table_local, id);

    if (symbol) {
        fprintf(stderr, "Line %d - Function parameter %s has already been defined "
                "as a parameter of the function %s.\n",
                line_counter, id, curr_function->id);
        return ERROR_SEM_DEFINITION;

    } else {
        /* Add a new local variable into the local symtable */
        symbol_attributes att = { .var_att = { .type = VTYPE_UNKNOWN,
            .defined = true} };
        
        symbol_t *new = symtable_insert(&table_local, id, STYPE_VAR, att);
        if (!new) return ERROR_INTERNAL;
    }
    return SUCCESS;
}

int check_if_defined_var(const char *id) {
    symbol_t *symbol;

    if (in_function) { /* First check the local table */
        symbol = symtable_search(&table_local, id);
        if (symbol) {
            if (symbol->type == STYPE_VAR && symbol->attributes.var_att.defined) {
                return VARIABLE_FOUND;
            }
        }
    }

    symbol = symtable_search(&table_global, id); /* Now check the global table */
    if (symbol) {
        if (symbol->type == STYPE_VAR) {

            if (in_function) {
                /* Add an undefined local variable into the local symtable */
                symbol_attributes att = { .var_att = { .type = VTYPE_UNKNOWN,
                    .defined = false} };
                
                symbol_t *new = symtable_insert(&table_local, id, STYPE_VAR, att);
                if (!new) return ERROR_INTERNAL;
            }

            return VARIABLE_FOUND;

        } else if (symbol->type == STYPE_FUNC) {
            return FUNCTION_FOUND;
        }
    }
    return SYMBOL_NOT_FOUND;
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
    symbol_t *symbol;

    symbol = symtable_search(&table_global, id);

    if (symbol) { /* First check if a function with the same id hasn't already
                     been defined */
        if (symbol->type == STYPE_FUNC) {

            fprintf(stderr, "Line %d - Semantic error: ID '%s' "
                    "has already been defined as a function.\n", line_counter, id);
            return ERROR_SEM_DEFINITION;
        }
    }

    if (in_function) {
        symbol = symtable_search(&table_local, id);

        if (symbol) {
            if (symbol->type == STYPE_VAR && !symbol->attributes.var_att.defined) {
                fprintf(stderr, "Line %d - Semantic error: Local variable '%s' has been "
                        "referenced before it's definition.\n", line_counter, id);
                return ERROR_SEM_DEFINITION;
            }

        } else { /* Cover the potential global variable */
            /* Add a new local variable into the local symtable */
            symbol_attributes att = { .var_att = { .type = VTYPE_UNKNOWN,
                .defined = true} };
            
            symbol_t *new = symtable_insert(&table_local, id, STYPE_VAR, att);
            if (!new) return ERROR_INTERNAL;
            return NEW_VARIABLE;
        }

    } else {

        symbol = symtable_search(&table_global, id);
        if (!symbol) {
            /* Add the new global variable into the global symtable */
            symbol_attributes att = { .var_att = { .type = VTYPE_UNKNOWN,
                .defined = true} };
            
            symbol_t *new = symtable_insert(&table_global, id, STYPE_VAR, att);
            if (!new) return ERROR_INTERNAL;
            return NEW_VARIABLE;
        }
    }

    return SUCCESS;
}

int add_symbol_function(const char *id) {
    if ((curr_function = symtable_search(&table_global, id))) {
        fprintf(stderr, "Line %d - Semantic error: ID '%s' "
                "has already been defined.\n", line_counter, id);
        return ERROR_SEM_DEFINITION;

    } else {
        func_att_t func_att = { .defined = true, .param_count = 0 };
        symbol_attributes attributes = { .func_att = func_att };

        if (!(curr_function = symtable_insert(&table_global, id, STYPE_FUNC, attributes))) {
            fprintf(stderr, "Line %d - Internal error: Could not allocate memory "
                    "for a new function symbol.\n", line_counter);
            return ERROR_INTERNAL;
        }
    }

    return SUCCESS;
}

int check_parameter_valid(token_t token) {
    if (token.type == TTYPE_ID) {
        switch (check_if_defined_var(token.attribute.string)) {
            case FUNCTION_FOUND:
                fprintf(stderr, "Line %d - Sematic error: parameter %s was a "
                        "function id.\n", line_counter, token.attribute.string);
                return ERROR_SEM_DEFINITION;

            case SYMBOL_NOT_FOUND:
                fprintf(stderr, "Line %d - Sematic error: parameter %s was "
                        "undefined.\n", line_counter, token.attribute.string);
                return ERROR_SEM_DEFINITION;

            default:
                break;
        }
    }
    return SUCCESS;
}

int add_built_in_functions() {
    int retvalue = SUCCESS;
    symbol_attributes att = { .func_att = { .defined = true, .param_count = 0 } };

    if (!(symtable_insert(&table_global, "inputs", STYPE_FUNC, att)
                && symtable_insert(&table_global, "inputi", STYPE_FUNC, att)
                && symtable_insert(&table_global, "inputf", STYPE_FUNC, att))) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = -1;
    if (!symtable_insert(&table_global, "print", STYPE_FUNC, att)) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = 1;
    if (!(symtable_insert(&table_global, "len", STYPE_FUNC, att)
                && symtable_insert(&table_global, "chr", STYPE_FUNC, att))) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = 2;
    if (!symtable_insert(&table_global, "ord", STYPE_FUNC, att)) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = 3;
    if (!symtable_insert(&table_global, "substr", STYPE_FUNC, att)) {
        retvalue = ERROR_INTERNAL;
    }

    if (retvalue)
        fprintf(stderr, "Internal error: failed to allocate memory "
                "for built in functions.\n");
    return retvalue;
}
