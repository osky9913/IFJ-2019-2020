#ifndef PRECEDENCE_ANALYSIS
#define PRECEDENCE_ANALYSIS

#include "scanner.h"
#include "symtable.h"
#include "general_stack.h"

#define P '<' // push to stack
#define R '>' //reduce by rule
#define X '0' //expresion error 
#define M '$' //money$$$$$



/**
 * @brief [This enum represents type of rules for precedence analysis] 
 */
typedef enum {
    RULE_MUL,       //E->E*E
    RULE_DIV,       //E->E/E
    RULE_IDIV,      //E->E//E
    RULE_PLUS,      //E->E+E
    RULE_MINUS,     //E->E-E
    RULE_ID,        //E->i
    RULE_BRACKET,   //E->(E)
    RULE_EQ,        //B->E==E
    RULE_GT,        //B->E>E
    RULE_GET,       //B->E>=E
    RULE_LT,        //B->E<E
    RULE_LET,       //B->E<=E
    RULE_NEQ,       //B->E!=E
} prec_analysis_rule_t;

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
 * @param f[Source file] 
 * @return
 */
int psa(FILE* f);


/**
 * [Applying precedence analysis rules] 
 * @param PAStack [Stack of non-terminals and terminals for syntax analysis]
 * @param newToken [new token from lexical analysis, will be converted to prec analysis table symbol]
 * 
 */
int apply_psa_rule(stack_general_t* PAStack, token_t newToken);


/**
 * [Get rule for prec analysis table from first terminal in stack and newSymbol]
 * @param PAStack [Stack of precedence analysis symbols]
 * @param newSymbol [New terminal symbol used for rule making]
 * @return [Char as rule for further operations]
 */
char get_prec_table_rule(stack_general_t* PAStack, int newSymbol);


/**
 * [Calls scanner for new token and convert it to the symbol in precedence analysis table]
 * @param f [Source file]
 * @return [returns precedence table symbol on succes, else -1]
 */
int get_prec_table_symbol(token_t newToken);

/**
 * [Reduces terminal and non-terminals on stack by applying one of the precedence analysis rules]
 * 
 * 
 * @return [Return value as int]
 */
int reduce();
#endif
