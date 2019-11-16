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

struct parsing_data {
    
    

};

/* Indicates whether the current block of code is inside a function or not */
bool in_function = false;

int parse();
int recursive_dedent();
#endif
