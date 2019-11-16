/**
 *	@file   parser.h
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Parser header file
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "symtable.h"
#include "precedence_analysis.h"
#include "general_stack.h"

struct parsing_data {
    symtable_t symtable_local;
    symtable_t symtable_global;

    bool in_function = false; /* Indicates whether the current block
                                 of code is inside a function or not */

    token_t current_token;
    stack_general_t *token_stash;
};

#endif
