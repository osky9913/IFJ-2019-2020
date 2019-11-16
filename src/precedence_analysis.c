#include "precedence_analysis.h"
#include "scanner.h"
#include "symtable.h"
#include "general_stack.h"



int get_prec_table_symbol(FILE* f){
    token_t newToken ;
    int lex_analysis = get_token(f, &newToken);
    if(lex_analysis == 1){
        printf("FUCK YOU TOKEN ERROR");
        return -1;
    }

    switch(newToken.type){
        case TTYPE_ID:
        case TTYPE_INT:
        case TTYPE_DOUBLE:
        case TTYPE_STR:
        case TTYPE_NONE:
        //not sure
        case TTYPE_DOCSTR:
            return ID;
        
        case TTYPE_LTBRAC:
            return L_BRACKET;
        
        case TTYPE_RTBRAC:
            return R_BRACKET;
        
        case TTYPE_ISEQ:
        case TTYPE_ISNEQ:
        case TTYPE_GTOREQ:
        case TTYPE_LSOREQ:
        case TTYPE_GT:
        case TTYPE_LS:
            return REL_OP;

        case TTYPE_ADD:
        case TTYPE_SUB:
            return PLUS_MINUS;

        case TTYPE_MUL:
        case TTYPE_DIV:
        case TTYPE_IDIV:
            return MUL_DIV;
        //not sure
        case TTYPE_EOL:
            return DOLLAR;

        default:
            return -1;
    }
}

char get_prec_table_rule(int i, int j){
    



}




int placeholder(FILE* f){
    int newSymbol = get_prec_table_symbol(f);
    if(newSymbol == -1){
        printf("TABLE SYMBOL ERROR PLACEHOLDER");
    }
    //precedence analysis stack of symbols
    stack_general_t* PAStack = stack_general_init();
    char rule = get_prec_table_rule(PAStack, newSymbol);
    if(rule == P){
        //stack push placeholder
        return 2;//gimme more
    }
    else if(rule == E){
        printf("FUCK YOU TABLE RULE ERROR\n");
        return 3;//its fucked up
    }
    else if(rule == M){
        printf("YOU WIN\n");
        return 1;//this is the end
    }
    else{
        reduce_rule();
        return 0;//its ok
    }
}