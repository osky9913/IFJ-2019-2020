/**
 * @file scanner.h
 * @brief This is header file of scanner.c
 * @author xziska03
 */
#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

indent_stack_t* dent_stack;

/* Return codes */
#define LEX_ERROR 301
#define LEX_SUCCES 0

extern int new_line;
extern int indents_to_pop;
extern int dent;

/**
 * @brief This enum represents types of processed token.
 */
typedef enum {
    TTYPE_ID,               // ID
    TTYPE_KEYWORD,          // def/else/if,None,pass,return,while
    TTYPE_EOF,              // end of file
    TTYPE_EOL,              // \n

    TTYPE_INT,              //whole number
    TTYPE_DOUBLE,           //desatinne čislo
    TTYPE_STR,              //string
    TTYPE_DOCSTR,           //documentation string
    TTYPE_NONE,             //None  

    TTYPE_INDENT,
    TTYPE_DEDENT,

    TTYPE_COMMA,            // ','
    TTYPE_COLUMN,           // ':'
    TTYPE_LTBRAC,           // '('
    TTYPE_RTBRAC,           // ')'
    TTYPE_ASSIGN,           // '='
    TTYPE_EXCL,             // '!'

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
    long integer;
    double decimal;
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

int calculate_dent(FILE* f, int* c);

/**
 * [Frees all allocated resources, converts string into token's attribute]
 * @param  exit_code    [exitcode of lexical analysis]
 * @param  token        [represents one lexem of processed source file]
 * @param  tmp          [temporary dynamic string]
 * @param  token_string [dynamic string, represents content of token's string attribute]
 * @return              [returns received exit_code]
 */
int finish_free_resources(int exit_code, token_t* token, string_t* tmp, string_t* token_string);

/**
 * [converts hexadecimal number from escape sequence to decimal number ]
 * @param f       [source file]
 * @param token_string [token's string attribute]
 */
void hexa_escape(FILE* f, string_t* Tstring);

/**
 * [Generates DEDENT tokens until dedent is equal to indent on top of stack]
 * @return [1 - found one dedent, 0 - indentation error, 2 - found all dedents(LA continues)
 */
int process_dedents();

/**
 * [scans through the code, recognises different tokens, ignores comments]
 * @param  f     [source file]
 * @param  token [processed token]
 * @return       [returns 0 on succes, else 1]
 */
int get_token(FILE* f, token_t* token);

#endif
