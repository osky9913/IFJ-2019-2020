/**
 *	@file   parser.h
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Parser header file
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"
#include "precedence_analysis.h"
#include "errors.h"


#define SUCCESS 0
#define UNEXPECTED_TOKEN 1
#define RETURN_IN_PROGRAM_BODY 1

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
 * @brief Checks the return code of lexical analysis passed in the parameter
 * retcode, if retcode indicates that an error had ocurred, frees all the resources
 * and exits the program with a corresponding error code.
 * @param retcode The return code which is to be processed.
 */
void lex_check(int retcode);

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
