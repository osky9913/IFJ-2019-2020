/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   semantic.c
 * @author Simon Sedlacek - xsedla1h
 * @brief The main semantic analysis module
 */

#include "semantic.h"

int check_function_dependencies(const symbol_t *function, snode_t **dep_list) {
    if (function) {

        /* create a dependency record list if needed -> prevents recursion */
        if (!*dep_list) {
            *dep_list = list_init(function->id);
            if (!*dep_list) exit(ERROR_INTERNAL); //FIXME

        } else {

            if (list_contains(*dep_list, function->id))
                return SUCCESS; /* This function has already been checked */
            else
                list_insert_symbol(*dep_list, function->id);
        }

        /* Now iterate over the dependency list in the symbol table */
        for (unsigned i = 0; i < function->attributes.func_att.dep_len; i++) {

            if (!function->attributes.func_att.depends[i]->attributes.func_att.defined) {
                /* A dependency was undefined */
                fprintf(stderr, "Line %d - Semantic error: A dependency '%s' of function "
                        "'%s' is undefinded.\n", scn.line_counter,
                        function->attributes.func_att.depends[i]->id,
                        prg.curr_function_call->id);
                return ERROR_SEM_DEFINITION;
            } else {
                /* Recursively call this function in order to check all the dependencies
                 * of all dependencies */
                if (check_function_dependencies(function->attributes.func_att.depends[i],
                        dep_list))
                    return ERROR_SEM_DEFINITION;
            }
        }
    }

    return SUCCESS;
}

int check_and_add_parameter_def(const char *id) {
    symbol_t *symbol = symtable_search(&prg.table_global, id); /* Now check for functions */
    if (symbol) {
        if (symbol->type == STYPE_FUNC) {
            fprintf(stderr, "Line %d - Function parameter %s has already been defined "
                    "as a function.\n", scn.line_counter, id);
            return ERROR_SEM_DEFINITION;
        }
    }

    /* Now check for duplicit parameters */
    symbol = symtable_search(&prg.table_local, id);

    if (symbol) {
        fprintf(stderr, "Line %d - Function parameter %s has already been defined "
                "as a parameter of the function %s.\n",
                scn.line_counter, id, prg.curr_function_def->id);
        return ERROR_SEM_DEFINITION;

    } else {
        /* Add a new local variable into the local symtable */
        symbol_attributes att = { .var_att = { .defined = true} };
        
        symbol_t *new = symtable_insert(&prg.table_local, id, STYPE_VAR, att);
        if (!new) return ERROR_INTERNAL;
    }
    return SUCCESS;
}

int check_if_defined_var(const char *id, const char *assignment_id) {
    symbol_t *symbol = NULL;

    if (prg.in_function) { /* First check the local table */
        symbol = symtable_search(&prg.table_local, id);
        if (symbol) {
            if (symbol->type == STYPE_VAR && symbol->attributes.var_att.defined) {
                return VARIABLE_FOUND;
            }
        }
    }

    symbol = symtable_search(&prg.table_global, id); /* Now check the global table */
    if (symbol) {
        if (symbol->type == STYPE_VAR) {

            /* Now check for a potential undefinde local variable */
            if (prg.in_function) {
                symbol = symtable_search(&prg.table_local, id);
                if (symbol) {

                    if (!symbol->attributes.var_att.defined) {
                        if (assignment_id) {
                            /* This is a problem if we try to intialize a variable
                             * with itself.... */
                            if (!strcmp(id, assignment_id))
                                return SYMBOL_NOT_FOUND;
                        }

                        return VARIABLE_FOUND;
                    }

                } else {

                    /* Add an undefined local variable into the local symtable */
                    symbol_attributes att = { .var_att = { .defined = false} };
                    
                    symbol_t *new = symtable_insert(&prg.table_local, id, STYPE_VAR, att);
                    if (!new) return ERROR_INTERNAL;
                    return VARIABLE_FOUND;
                }

            } else { /* A perfectly fine global variable */
                if (symbol->attributes.var_att.defined)
                    return VARIABLE_FOUND;
            }

        } else if (symbol->type == STYPE_FUNC) { /* We found a function */
            return FUNCTION_FOUND;
        }
    }
    return SYMBOL_NOT_FOUND;
}

int check_if_defined_func(const char *id) {
    symbol_t *symbol = symtable_search(&prg.table_global, id);
    if (symbol) {
        if (symbol->type == STYPE_FUNC) {
            if (symbol->attributes.func_att.defined) {
                /* A defined function was found */
                return FUNCTION_FOUND;
            }
        } else {
            /* A variable function was found */
            return VARIABLE_FOUND;
        }
    }
    return SYMBOL_NOT_FOUND;
}

int add_symbol_var(const char *id) {
    symbol_t *symbol;

    symbol = symtable_search(&prg.table_global, id);

    if (symbol) { /* First check if a function with the same id hasn't already
                     been defined */
        if (symbol->type == STYPE_FUNC) {

            fprintf(stderr, "Line %d - Semantic error: ID '%s' "
                    "has already been defined as a function.\n", scn.line_counter, id);
            return ERROR_SEM_DEFINITION;
        }
    }

    /* The next few steps are dependent on whether we are in a function definition
     * or in the global scope */
    if (prg.in_function) {
        symbol = symtable_search(&prg.table_local, id);

        if (symbol) {
            if (symbol->type == STYPE_VAR && !symbol->attributes.var_att.defined) {
                fprintf(stderr, "Line %d - Semantic error: Local variable '%s' has been "
                        "referenced before it's definition.\n", scn.line_counter, id);
                return ERROR_SEM_DEFINITION;
            }

        } else { /* Cover the potential global variable */
            /* Add a new local variable into the local symtable */
            symbol_attributes att = { .var_att = { .defined = true} };
            
            symbol_t *new = symtable_insert(&prg.table_local, id, STYPE_VAR, att);
            if (!new) return ERROR_INTERNAL;
            return NEW_VARIABLE;
        }

    } else { /* Global scope */

        symbol = symtable_search(&prg.table_global, id);
        if (!symbol) {
            /* Add the new global variable into the global symtable */
            symbol_attributes att = { .var_att = { .defined = true} };
            
            symbol_t *new = symtable_insert(&prg.table_global, id, STYPE_VAR, att);
            if (!new) return ERROR_INTERNAL;
            return NEW_VARIABLE;
        }
    }

    return SUCCESS;
}

int define_function(const char *id) {
    if ((prg.curr_function_def = symtable_search(&prg.table_global, id))) {

        /* This first case covers any function that might have been called
         * before in a different function but was undefined at the time */
        if (prg.curr_function_def->type == STYPE_FUNC 
                && !prg.curr_function_def->attributes.func_att.defined) {

            /* Set defined flag to true */
            prg.curr_function_def->attributes.func_att.defined = true;

        } else {
            fprintf(stderr, "Line %d - Semantic error: ID '%s' "
                    "has already been defined.\n", scn.line_counter, id);
            return ERROR_SEM_DEFINITION;
        }

    } else {
        /* Defining a new function */
        symbol_attributes att = { .func_att = { .defined = true, .param_count = -1,
        .depends = NULL, .dep_len = 0 } };

        if (!(prg.curr_function_def = symtable_insert(&prg.table_global, id, STYPE_FUNC, att))) {
            fprintf(stderr, "Line %d - Internal error: Could not allocate memory "
                    "for a new function symbol.\n", scn.line_counter);
            return ERROR_INTERNAL;
        }
    }

    return SUCCESS;
}

int add_undefined_function(const char *id) {
    prg.curr_function_call = symtable_search(&prg.table_local, id);
    if (prg.curr_function_call) {
        fprintf(stderr, "Line %d - Semantic error: Calling a yet undefined function "
                "with the same name '%s' as a local variable.\n",
                scn.line_counter, prg.curr_function_call->id);
        return ERROR_SEM_DEFINITION;
    }

    prg.curr_function_call = symtable_search(&prg.table_global, id);
    if (prg.curr_function_call) { /* The undefined function has been referenced already */

        if (prg.curr_function_call->type == STYPE_VAR) {
            fprintf(stderr, "Line %d - Semantic error: Calling an undefined function "
                    "with the same name '%s' as a global variable.\n",
                    scn.line_counter, id);
            return ERROR_SEM_DEFINITION;
        }

    } else { /* This is the first time this function has been referenced */
        symbol_attributes att = { .func_att = { .defined = false, .param_count = -1,
        .depends = NULL, .dep_len = 0 } };
        prg.curr_function_call = symtable_insert(&prg.table_global, id, STYPE_FUNC, att);
        if (!prg.curr_function_call) {
            fprintf(stderr, "Line %d - Internal error: Could not allocate memory "
                    "for a new function symbol.\n", scn.line_counter);
            return ERROR_INTERNAL;
        }
    }

    /* Add as a dependency to the currently defined function */
    prg.curr_function_def->attributes.func_att.depends[
        prg.curr_function_def->attributes.func_att.dep_len++] = prg.curr_function_call;

    /* Realloc the dependency array if needed */
    if ((prg.curr_function_def->attributes.func_att.dep_len % 20) == 0) {
        symbol_t **tmp = realloc(prg.curr_function_def->attributes.func_att.depends,
                prg.curr_function_def->attributes.func_att.dep_len * 2);
        if (!tmp) {
            fprintf(stderr, "Line %d - Internal error: Could not reallocate "
                    "the dependency list for function %s.\n", scn.line_counter,
                    prg.curr_function_def->id);
            return ERROR_INTERNAL;
        }
    }

    return SUCCESS;
}

int check_parameter_valid(const token_t token, const char *assignment_id) {
    if (token.type == TTYPE_ID) {
        /* Simply check whether the passed parameter was a valid parameter */
        switch (check_if_defined_var(token.attribute.string, assignment_id)) {
            case FUNCTION_FOUND:
                fprintf(stderr, "Line %d - Sematic error: parameter %s was a "
                        "function id.\n", scn.line_counter, token.attribute.string);
                return ERROR_SEM_DEFINITION; 

            case SYMBOL_NOT_FOUND:
                fprintf(stderr, "Line %d - Sematic error: parameter %s was "
                        "undefined.\n", scn.line_counter, token.attribute.string);
                return ERROR_SEM_DEFINITION;

            case ERROR_INTERNAL:
                return ERROR_INTERNAL;

            default:
                break;
        }
    }
    return SUCCESS;
}

int check_parameter_count_call(const int param_count) {
    if (prg.in_function) {

        /* This first case sets the initial parameter count when we encounter
         * a call of a yet undefined function */
        if (!prg.curr_function_call->attributes.func_att.defined) {
            if (prg.curr_function_call->attributes.func_att.param_count == -1) {
                prg.curr_function_call->attributes.func_att.param_count = param_count;
            }
        }
    }

    /* Now check the current parameter count with the symtable stats */
    if ((param_count != prg.curr_function_call->attributes.func_att.param_count) &&
            (prg.curr_function_call->attributes.func_att.param_count >= 0)) {

        fprintf(stderr, "Line %d - Semantic error: Wrong parameter "
                "count when calling function '%s'.\n", scn.line_counter,
                prg.curr_function_call->id);
        return ERROR_SEM_PARAM_COUNT;
    }
    return SUCCESS;
}

int check_function_call(const char *id) {
    int retvalue = check_if_defined_func(id);
    switch (retvalue) {
        case FUNCTION_FOUND:
            retvalue = ERROR_SYNTAX;
            prg.curr_function_call = symtable_search(&prg.table_global, id);
            if (!prg.curr_function_call) return ERROR_INTERNAL;

            if (!prg.in_function) { /* check the dependencies */

                /* create a dependency list to avoid infinite recursion when
                 * checking function dependencies */
                snode_t *list = NULL;
                if (check_function_dependencies(prg.curr_function_call, &list)) {
                    list_free(list);
                    return ERROR_SEM_DEFINITION;
                }
                list_free(list);

            } else { /* Add the current function call as a dependency */
                prg.curr_function_def->attributes.func_att.depends[
                    prg.curr_function_def->attributes.func_att.dep_len++] = prg.curr_function_call;

                /* Realloc the dependency array if needed */
                if ((prg.curr_function_def->attributes.func_att.dep_len % 20) == 0) {
                    symbol_t **tmp = realloc(prg.curr_function_def->attributes.func_att.depends,
                            prg.curr_function_def->attributes.func_att.dep_len * 2);
                    if (!tmp) {
                        fprintf(stderr, "Line %d - Internal error: Could not reallocate "
                                "the dependency list for function %s.\n", scn.line_counter,
                                prg.curr_function_def->id);
                        return ERROR_INTERNAL;
                    }
                }
            }

            break;

        case VARIABLE_FOUND:
            fprintf(stderr, "Line %d - Semantic error: Function '%s' "
                    "has already been defined as a variable.\n", scn.line_counter, id);
            return ERROR_SEM_DEFINITION;
            break;

        case SYMBOL_NOT_FOUND:
            if (prg.in_function) {
                retvalue = add_undefined_function(id);
                if (retvalue !=SUCCESS) return retvalue;

            } else {
                fprintf(stderr, "Line %d - Semantic error: Function '%s' "
                        "is undefinded.\n", scn.line_counter, id);
                return ERROR_SEM_DEFINITION;
            }
            break;

        default:
            break;
    }
    return SUCCESS;
}

int add_built_in_functions() {
    int retvalue = SUCCESS;
    /* Setting default attributes */
    symbol_attributes att = { .func_att = { .defined = true, .param_count = 0,
    .depends = NULL, .dep_len = 0 } };

    if (!(symtable_insert(&prg.table_global, "inputs", STYPE_FUNC, att)
                && symtable_insert(&prg.table_global, "inputi", STYPE_FUNC, att)
                && symtable_insert(&prg.table_global, "inputf", STYPE_FUNC, att))) {
        retvalue = ERROR_INTERNAL;
    }

    /* For the print function, we set he default parameter count to
     * -1, which indicates the possibility of calling it with a
     *  variable amount of parameters */
    att.func_att.param_count = -1;
    if (!symtable_insert(&prg.table_global, "print", STYPE_FUNC, att)) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = 1;
    if (!(symtable_insert(&prg.table_global, "len", STYPE_FUNC, att)
                && symtable_insert(&prg.table_global, "chr", STYPE_FUNC, att))) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = 2;
    if (!symtable_insert(&prg.table_global, "ord", STYPE_FUNC, att)) {
        retvalue = ERROR_INTERNAL;
    }

    att.func_att.param_count = 3;
    if (!symtable_insert(&prg.table_global, "substr", STYPE_FUNC, att)) {
        retvalue = ERROR_INTERNAL;
    }

    if (retvalue)
        fprintf(stderr, "Internal error: failed to allocate memory "
                "for built in functions.\n");
    return retvalue;
}
