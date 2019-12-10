/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   semantic.c
 * @author Simon Sedlacek, xsedla1h
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
                        "'%s' is undefinded.\n", line_counter,
                        function->attributes.func_att.depends[i]->id,
                        curr_function_call->id);
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
                line_counter, id, curr_function_def->id);
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

int check_if_defined_var(const char *id, const char *assignment_id) {
    symbol_t *symbol = NULL;

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
                symbol = symtable_search(&table_local, id);
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
                    symbol_attributes att = { .var_att = { .type = VTYPE_UNKNOWN,
                        .defined = false} };
                    
                    symbol_t *new = symtable_insert(&table_local, id, STYPE_VAR, att);
                    if (!new) return ERROR_INTERNAL;
                    return VARIABLE_FOUND;
                }

            } else {
                if (symbol->attributes.var_att.defined)
                    return VARIABLE_FOUND;
            }

        } else if (symbol->type == STYPE_FUNC) {
            return FUNCTION_FOUND;
        }
    }
    return SYMBOL_NOT_FOUND;
}

int check_if_defined_func(const char *id) {
    symbol_t *symbol = symtable_search(&table_global, id);
    if (symbol) {
        if (symbol->type == STYPE_FUNC) {
            if (symbol->attributes.func_att.defined) {
                return FUNCTION_FOUND;
            }
        } else {
            return VARIABLE_FOUND;
        }
    }
    return SYMBOL_NOT_FOUND;
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

int define_function(const char *id) {
    if ((curr_function_def = symtable_search(&table_global, id))) {
        if (curr_function_def->type == STYPE_FUNC 
                && !curr_function_def->attributes.func_att.defined) {

            /* Set defined flag to true */
            curr_function_def->attributes.func_att.defined = true;

        } else {
            fprintf(stderr, "Line %d - Semantic error: ID '%s' "
                    "has already been defined.\n", line_counter, id);
            return ERROR_SEM_DEFINITION;
        }

    } else {
        symbol_attributes att = { .func_att = { .defined = true, .param_count = -1,
        .depends = NULL, .dep_len = 0 } };

        if (!(curr_function_def = symtable_insert(&table_global, id, STYPE_FUNC, att))) {
            fprintf(stderr, "Line %d - Internal error: Could not allocate memory "
                    "for a new function symbol.\n", line_counter);
            return ERROR_INTERNAL;
        }
    }

    return SUCCESS;
}

int add_undefined_function(const char *id) {
    curr_function_call = symtable_search(&table_local, id);
    if (curr_function_call) {
        fprintf(stderr, "Line %d - Semantic error: Calling a yet undefined function "
                "with the same name '%s' as a local variable.\n",
                line_counter, curr_function_call->id);
        return ERROR_SEM_DEFINITION;
    }

    curr_function_call = symtable_search(&table_global, id);
    if (curr_function_call) { /* The undefined function has been referenced already */

        if (curr_function_call->type == STYPE_VAR) {
            fprintf(stderr, "Line %d - Semantic error: Calling an undefined function "
                    "with the same name '%s' as a global variable.\n",
                    line_counter, id);
            return ERROR_SEM_DEFINITION;
        }

    } else { /* This is the first time this function has been referenced */
        symbol_attributes att = { .func_att = { .defined = false, .param_count = -1,
        .depends = NULL, .dep_len = 0 } };
        curr_function_call = symtable_insert(&table_global, id, STYPE_FUNC, att);
        if (!curr_function_call) {
            fprintf(stderr, "Line %d - Internal error: Could not allocate memory "
                    "for a new function symbol.\n", line_counter);
            return ERROR_INTERNAL;
        }
    }

    /* Add as a dependency to the currently defined function */
    curr_function_def->attributes.func_att.depends[
        curr_function_def->attributes.func_att.dep_len++] = curr_function_call;

    /* Realloc the dependency array if needed */
    if ((curr_function_def->attributes.func_att.dep_len % 20) == 0) {
        symbol_t **tmp = realloc(curr_function_def->attributes.func_att.depends,
                curr_function_def->attributes.func_att.dep_len * 2);
        if (!tmp) {
            fprintf(stderr, "Line %d - Internal error: Could not reallocate "
                    "the dependency list for function %s.\n", line_counter,
                    curr_function_def->id);
            return ERROR_INTERNAL;
        }
    }

    return SUCCESS;
}

int check_parameter_valid(const token_t token, const char *assignment_id) {
    if (token.type == TTYPE_ID) {
        switch (check_if_defined_var(token.attribute.string, assignment_id)) {
            case FUNCTION_FOUND:
                fprintf(stderr, "Line %d - Sematic error: parameter %s was a "
                        "function id.\n", line_counter, token.attribute.string);
                return ERROR_SEM_DEFINITION; 

            case SYMBOL_NOT_FOUND:
                fprintf(stderr, "Line %d - Sematic error: parameter %s was "
                        "undefined.\n", line_counter, token.attribute.string);
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
    if (in_function) {
        if (!curr_function_call->attributes.func_att.defined) {

            if (curr_function_call->attributes.func_att.param_count == -1) {
                curr_function_call->attributes.func_att.param_count = param_count;
            }
        }

    }
    if ((param_count != curr_function_call->attributes.func_att.param_count) &&
            (curr_function_call->attributes.func_att.param_count >= 0)) {

        fprintf(stderr, "Line %d - Semantic error: Wrong parameter "
                "count when calling function '%s'.\n", line_counter,
                curr_function_call->id);
        return ERROR_SEM_PARAM_COUNT;
    }

    return SUCCESS;
}

int check_function_call(const char *id) {
    int retvalue = check_if_defined_func(id);
    switch (retvalue) {
        case FUNCTION_FOUND:
            retvalue = ERROR_SYNTAX;
            curr_function_call = symtable_search(&table_global, id);
            if (!curr_function_call) return ERROR_INTERNAL;

            if (!in_function) { /* check the dependencies */

                /* create a dependency list to avoid infinite recursion */
                snode_t *list = NULL;
                if (check_function_dependencies(curr_function_call, &list)) {
                    list_free(list);
                    return ERROR_SEM_DEFINITION;
                }
                list_free(list);

            } else { /* Add the current function call as a dependency */
                curr_function_def->attributes.func_att.depends[
                    curr_function_def->attributes.func_att.dep_len++] = curr_function_call;

                /* Realloc the dependency array if needed */
                if ((curr_function_def->attributes.func_att.dep_len % 20) == 0) {
                    symbol_t **tmp = realloc(curr_function_def->attributes.func_att.depends,
                            curr_function_def->attributes.func_att.dep_len * 2);
                    if (!tmp) {
                        fprintf(stderr, "Line %d - Internal error: Could not reallocate "
                                "the dependency list for function %s.\n", line_counter,
                                curr_function_def->id);
                        return ERROR_INTERNAL;
                    }
                }
            }

            break;

        case VARIABLE_FOUND:
            fprintf(stderr, "Line %d - Semantic error: Function '%s' "
                    "has already been defined as a variable.\n", line_counter, id);
            return ERROR_SEM_DEFINITION;
            break;

        case SYMBOL_NOT_FOUND:
            if (in_function) {
                retvalue = add_undefined_function(id);
                if (retvalue !=SUCCESS) return retvalue;

            } else {
                fprintf(stderr, "Line %d - Semantic error: Function '%s' "
                        "is undefinded.\n", line_counter, id);
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
    symbol_attributes att = { .func_att = { .defined = true, .param_count = 0,
    .depends = NULL, .dep_len = 0 } };

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
