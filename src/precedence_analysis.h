#ifndef PRECEDENCE_ANALYSIS
#define PRECEDENCE_ANALYSIS


#define P '<' // push to stack
#define R '>' //reduce by rule
#define E '0' //expresion error 
#define M '$' //money$$$$$


/**
 * @brief Precedence analysis table with operations with symbols
 */
const char prec_table[7][7] = {
    {R, P, P, R, P, R, R},
    {R, R, P, R, P, R, R},
    {P, P, P, P, P, P, E},
    {R, R, E, R, E, R, R},
    {R, R, E, R, E, R, R},
    {P, P, P, R, P, E, R},
    {P, P, P, E, P, P, M}
};


/**
 * @brief This enum represents type of rules for precedence analysis 
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
 * @brief This enum represents type of symbol of precedence analysis table
 */
typedef enum {
    ID,             //i
    DOLLAR,         //$
    MUL_DIV,        //*,/,//
    PLUS_MINUS,     //+,-
    L_BRACKET,      //(
    R_BRACKET,      //)
    REL_OP,         //<,>,<=,>=,==,!=
    //should be here "E"?
    E,
    B,

} prec_analysis_symbol_t;

/**
 * [Calls scanner for new token and convert it to the symbol in precedence analysis table]
 * @param f [Source file]
 * @return [returns precedence table symbol on succes, else -1]
 */
int get_prec_table_symbol(FILE*f);

#endif
