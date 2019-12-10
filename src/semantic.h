/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   semantic.h
 * @author Simon Sedlacek, xsedla1h
 * @brief Header file for semantic analysis module
 */

#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include "parser.h"
#include "symtable.h"
#include "symbol_list.h"

#define SYMBOL_NOT_FOUND 66 
#define VARIABLE_FOUND 67
#define FUNCTION_FOUND 68
#define UNDEF_FUNCTION_FOUND 69
#define NEW_VARIABLE 70


/**
 * @brief Adds an undefined function into the global symtable. This function has
 * to be later defined in order for it to be callable.
 *
 * @return TODO
 */
int add_undefined_function(const char *id);

/**
 * @brief Checks if all functions called within this function and all of their
 * dependencies are actually defined.
 *
 * @return TODO
 */
int check_function_dependencies(const symbol_t *function, snode_t **dep_list);

/**
 * @brief Used when in function definition. Checks if the declared parameter is
 * a valid choice and stores it into the local symtable.
 *
 * @return TODO
 */
int check_and_add_parameter_def(const char *id);

/**
 * @brief This function is used to create a defined function entry in the global symtable
 * or to simply change status of a previously used but undefined function to 'defined'.
 *
 * @return TODO
 */
int define_function(const char *id);

/**
 * @brief Based on the context (local/global) this function adds a new variable symbol
 * to the correstponding symtable. Before that, it checks if the identifier is still
 * usable for a variable definition. This function also prevents the user from defining
 * a local variable in a function after he has already referenced a global variable with
 * the same name.
 *
 * @return TODO
 */
int add_symbol_var(const char *id);

/**
 * @brief Checks if a symbol with the identifier id has already been defined. If
 * the current scope is inside a function, the function first searches the local
 * symtable and if unsuccesful, it needs to search the global table. If succesful there,
 * the function then creates an undefined local variable used to check for a unauthorised
 * definition of a local variable of the same name.
 *
 * @return Returns VARIABLE_FOUND on success and FUNCTION_FOUND when it finds a function.
 * Otherwise, returns SYMBOL_NOT_FOUND or INTERNAL_ERROR.
 */
int check_if_defined_var(const char *id, const char *assignment_id);

/**
 * @brief Checks if a function called in the main program body has already been defined.
 *
 * @return Returns either FUNCTION_FOUND if a defined function has been found,
 * VARIABLE_FOUND in case a variable has been found, or SYMBOL_NOT_FOUND in case
 * the function was undefined or doesn't exist at all.
 */
int check_if_defined_func(const char *id);

/**
 * @brief This functions checks if the token passed to it is an identifier
 * and if so, it searches the relevant symbol table in order to check if it
 * is a variable and if it has been defined.
 *
 * @return Returns SUCCESS if the parameter is a valid variable, otherwise returns
 * ERROR_SEM_DEFINITION.
 */
int check_parameter_valid(const token_t token, const char *assignment_id);

/**
 * @brief After processing the parameters of a function call, this function checks
 * if the number of parameters of that function corresponded to the declared
 * number in the global symtable.
 *
 * @return TODO
 */
int check_parameter_count_call(const int param_count);

/** 
 * @brief This function checks if the function call can be executed.
 *
 * @return TODO
 */
int check_function_call(const char *id);

/**
 * @brief Adds all the built in function symbols to the global symtable.
 *
 * @return Returns SUCCESS or ERROR_INTERNAL in case an operation fails.
 */
int add_built_in_functions();

#endif
