/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file scanner.h
 * @author Marek Ziska, xziska03
 * @brief This is header file of scanner.c
 */
#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "dynamic_string.h"
#include "general_stack.h"
#include "errors.h"

/**
 * @brief This structure holds all the data that the scanner needs to work with.
 */
typedef struct scanner_data {
    /* variable to determine if we are on new line(time to evaluate scn.indent/dedent)
      -1 -> when beginning of the file
       0 -> when LA found token thats not EOL
       1 -> when LA found token that is EOL */
    int line_counter;
    int new_line;
    
    /* global variable do determine if there are more dedent tokens to be generated */
    int indents_to_pop;

    /* actual line scn.indentation */
    int indent;

    stack_general_t* dent_stack;
} sdata_t;

/* An instance of scanner_data */
extern sdata_t scn;

/**
 * @brief This enum represents types of processed token.
 */
typedef enum {
    TTYPE_ID,               // ID
    TTYPE_KEYWORD,          // def/else/if,None,pass,return,while
    TTYPE_EOF,              // end of file
    TTYPE_EOL,              // \n
    TTYPE_NONE,

    TTYPE_INT,              //whole number
    TTYPE_DOUBLE,           //float number
    TTYPE_STR,              //string

    TTYPE_INDENT,
    TTYPE_DEDENT,

    TTYPE_COMMA,            // ','
    TTYPE_COLUMN,           // ':'
    TTYPE_LTBRAC,           // '('
    TTYPE_RTBRAC,           // ')'
    TTYPE_ASSIGN,           // '='

    TTYPE_ISEQ,             // '=='
    TTYPE_ISNEQ,            // '!='
    TTYPE_GTOREQ,           // '>='
    TTYPE_LSOREQ,           // '<='
    TTYPE_GT,               // '>'
    TTYPE_LS,               // '<'
    TTYPE_ADD,              // '+' addition or concatenation
    TTYPE_SUB,              // '-'
    TTYPE_MUL,              // '*'
    TTYPE_DIV,              // '/'
    TTYPE_IDIV,             // '//'
} token_type_t; 

/**
 * @brief This enum represents types of keywords.
 */
typedef enum {
    KEY_IF,                 
    KEY_DEF,
    KEY_ELSE,
    KEY_NONE,
    KEY_PASS,
    KEY_RETURN,
    KEY_WHILE,
} keyword_t;

/**
 * @brief This union represents token's attributes.
 */
typedef union {
    char* string;
    keyword_t keyword;
} attribute_t;

/**
 * @brief This structure represents one lexem processed in lexical analysis.
 */
typedef struct{
    token_type_t type;
    attribute_t attribute;
} token_t;

int calculate_dent(int* c);

/**
 * @brief Frees all allocated resources, converts string into token's attribute
 * @param  exit_code    [exitcode of lexical analysis]
 * @param  token        [represents one lexem of processed source file]
 * @param  tmp          [temporary dynamic string]
 * @param  token_string [dynamic string, represents content of token's string attribute]
 * @return              [returns received exit_code]
 */
int finish_free_resources(int exit_code, token_t* token, string_t* tmp, string_t* token_string);


/**
 * @brief Generates DEDENT tokens until dedent is equal to indent on top of stack
 * @return [1 - found one dedent, 0 - indentation error, 2 - found all dedents(LA continues)
 */
int process_dedents();

/**
 * @brief scans through the code, recognises different tokens, ignores comments
 * @param  f     [source file]
 * @param  token [processed token]
 * @return       [returns 0 on succes, else 1]
 */
int get_token(token_t* token);

#endif
