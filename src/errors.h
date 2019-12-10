/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   errors.h
 * @author Martin Osvald - xosval03
 * @brief This header file contains all the used return codes
 */


#ifndef __ERRORS_H__
#define __ERRORS_H__


enum errors {
    SUCCESS=0, ALLOC_ERROR=60, NULL_PASSED, PREC_SYMBOL_TABLE_ERROR,
};

#define ERROR_LEXICAL 1
#define ERROR_SYNTAX 2
#define ERROR_SEM_DEFINITION 3
#define ERROR_SEM_TYPE 4
#define ERROR_SEM_PARAM_COUNT 5
#define ERROR_SEM_OTHER 6
#define ERROR_DIV_ZERO 9
#define ERROR_INTERNAL 99


#endif
