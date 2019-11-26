#ifndef PRECEDENCE_ANALYSIS
#define PRECEDENCE_ANALYSIS

#include "scanner.h"
#include "symtable.h"
#include "general_stack.h"
#include "errors.h"
#include "parser.h"
#include "infixToPostfix.h"
#include <stdbool.h>

#define P '<' // push to stack
#define R '>' //reduce by rule
#define X '0' //expresion error 
#define M '$' //money$$$$$

/**
 * @brief [This enum represents type of symbol of precedence analysis table]
 */
typedef enum {
    PLUS_MINUS,     //+,-
    MUL_DIV,        //*,/,//
    L_BRACKET,      //(
    R_BRACKET,      //)
    ID,             //i
    REL_OP,         //<,>,<=,>=,==,!=
    DOLLAR,         //$
    E,                //should be here "E"?
    B,

} prec_analysis_symbol_t;

/**
 * [main function for precedence syntax analysis, checking syntax and semantic rules]
 * @return [0 on success, 1 when syntax error]
 */
int psa();


/**
 * [Applying precedence analysis rules] 
 * @param PAStack [Stack of non-terminals and terminals for syntax analysis]
 * @return [0 on success, otherwise 1]
 */
int apply_psa_rule(stack_general_t* PAStack);


/**
 * [Get rule for prec analysis table from first terminal in stack and newSymbol]
 * @param PAStack [Stack of precedence analysis symbols]
 * @param newSymbol [New terminal symbol used for rule making]
 * @return [Char as rule for further operations]
 */
char get_prec_table_rule(stack_general_t* PAStack, int newSymbol);


/**
 * [Calls scanner for new token and convert it to the symbol in precedence analysis table]
 * @return [returns precedence table symbol on success, else -1]
 */
int get_prec_table_symbol();

/**
 * [Replaces 3 items which should be reduced by rule to one non-terminal]
 *  @param PAStack [Stack of precedence analysis symbols]
 *  @param toPush [Non-terminal which should be pushed to the stack instead of the 3 poped]
 * @return [Returns 0 on success, otherwise 1]
 */
int reduce_on_stack(stack_general_t* PAStack, int toPush);

/**
 * [Reduces terminals and non-terminals to non-terminals by applying precedence analysis syntax rules]
 * @param PAStack [Stack of precedence analysis symbols]
 * @return [Returns 0 when reducing rule is successfully applied, otherwise 1]
 */
 int reduce(stack_general_t* PAStack);
#endif
