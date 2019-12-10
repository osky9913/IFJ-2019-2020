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
#include "generate_ver2.h"

typedef enum {
    DEFAULT,
    IF,
    WHILE,
    ASSIGN,
    RETURN,
} psa_state_t;

/** @brief This variable indicates the currently parsed construction - PSA */
extern psa_state_t psa_state;

/* Local and global symtables */
symtable_t table_global, table_local;

/**
 * @brief Indicates whether the currently analyzed block
 * of code is inside a function or not.
 */
extern bool in_function;

/**
 * @brief A variable that stores the currently processed token.
 */
extern token_t curr_token;

/**
 * @brief A variable that stores the currently processed symbol. Useful
 * when we need to check the number of function parameters and don't want
 * to lose the reference to it's id.
 */
extern symbol_t *curr_function_def;
extern symbol_t *curr_function_call;

/**
 * @brief A variable that stores up to two tokens at a time.
 */
extern token_t token_stash[2];

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
