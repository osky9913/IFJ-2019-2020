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
//#include "symtable.h"
//#include "precedence_analysis.h"
//#include "general_stack.h"
//#include "error.h"


#define SUCCESS 0
#define UNEXPECTED_TOKEN 1

//symtable_t symtable_local;
//symtable_t symtable_global;

extern bool in_function; /* Indicates whether the current block
                             of code is inside a function or not */

extern token_t curr_token;
extern token_t token_stash[2];

int init_resources();
void free_resources();

int next_token(bool load_from_stash);
void unget_token();

int r_program();
int r_function_def();
int r_statement();

#endif
