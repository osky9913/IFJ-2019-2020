#include "precedence_analysis.h"
#include "scanner.h"
#include "symtable.h"
#include "general_stack.h"



int get_prec_table_symbol(token_t newToken){
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


char get_prec_table_rule(stack_general_t* PAStack, int newSymbol){
    stack_general_item_t* top = stack_general_top(PAStack);
    int stackTerm =  *(int*)top->data;
    if(stackTerm == E){
        stackTerm = *(int*)top->next->data;
    }

}


int placeholder( stack_general_t* PAStack, token_t newToken){
    //converting token to symbol for from precedence table
    int newSymbol = get_prec_table_symbol(newToken);
    if(newSymbol == -1){
        printf("TABLE SYMBOL ERROR PLACEHOLDER");
    }

    //precedence table rules for two terminals, one on stack on as input(newSymbol)
    char rule = get_prec_table_rule(PAStack, newSymbol);
    
    //rule handling
    if(rule == P){
        //stack push placeholder
        return 2;//gimme more
    }
    else if(rule == X){
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


int psa(FILE* f){
    //declaration of needed data structures
    stack_general_t* PAStack = stack_general_init();//precedence analysis stack of symbols
    token_t newToken;

    int lex_analysis = get_token(f, &newToken);
    //if scanner return 1 as error
    if(lex_analysis == 1){
        printf("FUCK YOU TOKEN ERROR");
        return -1;
    }

    while(newToken.type != TTYPE_COLUMN && newToken.type != TTYPE_EOL){
        int placeholder(PAStack, newToken);
        int seman_analysis_placeholder();
        
        //getting new token and checking for error
        lex_analysis = get_token(f, &newToken);
        if(lex_analysis == 1){
            printf("FUCK YOU TOKEN ERROR");
            return -1;
        }
    }

}