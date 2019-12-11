/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   parser.h
 * @author Simon Sedlacek, xsedla1h
 * @brief Parser header file
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "precedence_analysis.h"
#include "errors.h"
#include "semantic.h"
#include "generate.h"

/** @brief This enum indicates the currently parsed construction - PSA */
typedef enum {
    DEFAULT,
    IF,
    WHILE,
    ASSIGN,
    RETURN,
} psa_state_t;

/**
 * @struct parser_data
 * @brief This structure holds all the data the parser needs in order to function
 * and communicate with other modules.
 *
 * @var table_local The local symtable
 * @var table_global The global symtable
 *
 * @var curr_token The currently processed token
 * @var token_stash A special variable that can hold up to two tokens that are to
 * be withdrawn in the future
 *
 * @var curr_function_def This variable points to a function in the global symtable,
 * which is the function that is currently being defined
 * @var curr_function_call Analogical to curr_function_def, but represents the function
 * that is currently being called
 * @var undef_symbol This is a special symbol variable used by semantic analysis that
 * is used to prevent constructions like a = a if the variable a hasn't
 * been defined before
 *
 * @var in_function This boolean indicates whether we are analysing a function or
 * the global scope of the source program
 * @var psa_state This variable tells the precedence semantic analysis the context
 * of the currently analysed expression so that it can perform the necessary semantic
 * operations
 *
 * @var param_count This variable is used to count the number of parameters of a function
 * call or a function definition
 */
typedef struct parser_data {

    symtable_t table_local;
    symtable_t table_global;

    token_t curr_token;
    token_t token_stash[2];

    symbol_t *curr_function_def;
    symbol_t *curr_function_call;
    symbol_t *undef_symbol;

    bool in_function;
    psa_state_t psa_state;

    int param_count;
} pdata_t;

/**
 * @brief This is a parser data global structure
 * */
extern pdata_t prg;

/**
 * @brief Inits all the resources for the compiler.
 * @return Returns a return code corresponding to the result
 * of resource initialization - see errors.h
 */
int init_resources();

/**
 * @brief Frees all the resources for the compiler.
 */
void free_resources();

/**
 * @brief Reads the next token from standard input, stores it
 * into the variable curr_token.
 *
 * @param load_from_stash If set to true, the function will first try to
 * load a token from stash. If there is no token in stash, it automatically
 * calls the scanner for a new token.
 *
 * @return Returns a return code corresponding to the result
 * of lexical analysis - see errors.h
 */
int next_token(bool load_from_stash);

/**
 * @brief Stashes the current token to token_stash
 * @note Only two tokens can be stashed at a time!!
 */
void unget_token();

/**
 * @brief Clears token stash
 */
void stash_clear();

/**
 * @brief Checks if token stash is empty
 */
bool stash_empty();

/**
 * @brief Frees the memory allocated for the token attribute if it's of type string.
 */
void token_free(token_t *token);


/* What follows are prototypes of funcions, that implement the syntax and semantic
 * analysis of the input program. The rules are derived from our LL grammar and LL
 * table so the descriptions of the functions are analogical. */

/**
 * @brief The topmost rule of the syntax analysis.
 * @return Returns a return code corresponding to the result
 * of the analysis - see errors.h
 */
int r_program();
int r_statement();
int r_statement_list();
int r_function_def();
int r_param_list_def();
int r_params_def();
int r_param_list();
int r_params();
int r_if_else();
int r_cycle();
int r_function_ret();
int r_retvalue();
int r_value();
int r_rest();
int r_function_call();
int r_term();
int r_literal();

#endif
