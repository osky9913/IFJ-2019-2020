#ifndef PRECEDENCE_ANALYSIS
#define PRECEDENCE_ANALYSIS

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
    RULE_EQ,        //E->E==E
    RULE_GT,        //E->E>E
    RULE_GET,       //E->E>=E
    RULE_LT,        //E->E<E
    RULE_LET,       //E->E<=E
    RULE_NEQ,       //E->E!=E
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

} prec_analysis_symbol_t;


#endif
