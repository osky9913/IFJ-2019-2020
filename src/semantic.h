/**
 *	@file   semantic.h
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Header file for semantic actions module
 */

#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

#include "parser.h"

int add_symbol_function(const char *id);
int add_symbol_var(const char *id);

bool check_if_defined_var(const char *id);
bool check_if_defined_func(const char *id);

#endif
