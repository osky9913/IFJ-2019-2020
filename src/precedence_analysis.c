#include "precedence_analysis.h"
#include "errors.h"

const char PREC_TABLE[7][7] =
        {
                {R, P, P, R, P, R, R},
                {R, R, P, R, P, R, R},
                {P, P, P, P, P, P, X},
                {R, R, X, R, X, R, R},
                {R, R, X, R, X, R, R},
                {P, P, P, R, P, X, R},
                {P, P, P, X, P, P, M}
        };

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
        printf("fhowrhf%d\n",stackTerm);
    }
    printf("TABLE RULE:[%d][%d]\n",stackTerm, newSymbol);
    return PREC_TABLE[stackTerm][newSymbol];

}

int reduce_on_stack(stack_general_t* PAStack, int toPush){
    stack_pop(PAStack);
    stack_pop(PAStack);
    stack_pop(PAStack);
    if(stack_general_push_int(PAStack, toPush) == ALLOC_ERROR){
        printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
        return -2000;
    }
    return 0;
}

int reduce(stack_general_t* PAStack){
    stack_general_item_t* tempStackItem = stack_general_top(PAStack);
    int operand1 = *(int*)tempStackItem->data;
    //Rule number 1 E->i
    if(operand1 == ID){
        printf("RULE 1\n");
        stack_pop(PAStack);
        //If stack push fails
        if(stack_general_push_int(PAStack, E) == ALLOC_ERROR){
            printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
            return -2000;
        }
        return 0;   
    }
    //getting second operand and operator to check for the rules
    tempStackItem = tempStackItem->next;
    int operator = *(int*)tempStackItem->data;
    tempStackItem = tempStackItem->next;
    int operand2 = *(int*)tempStackItem->next;
    
    if(operand1 == E && operand2 == E){
        printf("RULE1\n");
        //RULES E->E+E, E->E-E 
        if(operator == PLUS_MINUS){
            if(reduce_on_stack(PAStack, E) == -2000){
                return 20000;
            }
        }

        //RULES E->E*E, E->E/E, E->E//E
        if(operator == MUL_DIV){
            if(reduce_on_stack(PAStack, E) == -2000){
                return 20000;
            }
        }

        //RULES B->E==E, B->E<=E, B->E>=E, B->E!=E, B->E<E, B->E>E 
        if(operator == REL_OP){
            if(reduce_on_stack(PAStack, B) == -2000){
                return 2000;
            }
        }
    }
    //RULE E->(E)
    else if(operand1 == L_BRACKET && operand2 == R_BRACKET && operator == E){
        if(reduce_on_stack(PAStack, E) == -2000){
            return 20000;
        }
    }
    return 0;
}


int apply_psa_rule( stack_general_t* PAStack, token_t newToken){
    //converting token to symbol for from precedence table
    int newSymbol = get_prec_table_symbol(newToken);
    printf("TOKEN SYMBOL[%d]\n",newSymbol);
    if(newSymbol == -1){
        printf("TABLE SYMBOL ERROR PLACEHOLDER");
    }
    
    //precedence table rules for two terminals, one on stack on as input(newSymbol)
    char rule = get_prec_table_rule(PAStack, newSymbol);
    printf("RULE %c\n", rule);
    //rule handling
    int reducing = false;
    do{
        if(rule == P){
            if(stack_general_push_int(PAStack, newSymbol) == ALLOC_ERROR) {
                printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
                return 1;
            }
            break;
        }
        else if(rule == X){
            printf("FUCK YOU TABLE RULE ERROR\n");
            return 1;
        }
        else if(rule == M){
            printf("YOU WIN\n");
            break;
        }
        else{
            reduce(PAStack);
            reducing = 1;
        }
        rule = get_prec_table_rule(PAStack, newSymbol);
    }while(reducing);

    return 0;
}


int psa(FILE* f){
    //declaration of needed data structures
    stack_general_t* PAStack = stack_general_init();//precedence analysis stack of symbols
    token_t newToken;

    if(stack_general_push_int(PAStack, DOLLAR) == ALLOC_ERROR){
        printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
        stack_free(PAStack);
        return 1;
    }
    int lex_analysis = get_token(f, &newToken);
    //if scanner return 1 as error
    if(lex_analysis == 1){
        printf("FUCK YOU TOKEN ERROR");
        stack_free(PAStack);
        return false;
    }

    while(newToken.type != TTYPE_COLUMN && newToken.type != TTYPE_EOL){
        int psaCheck = apply_psa_rule(PAStack, newToken);
        printf("PSACHECK:%d\n",psaCheck);

        //incorrect syntax
        if(psaCheck) {
            printf("INCORRECT SYNTAX\n");
            stack_free(PAStack);
            return false;
        }

        if(newToken.type == TTYPE_ID || newToken.type == TTYPE_STR || newToken.type == TTYPE_DOCSTR){
            free(newToken.attribute.string);
        }

        //getting new token and checking for error
        lex_analysis = get_token(f, &newToken);
        if(lex_analysis == 0){
            printf("FUCK YOU TOKEN ERROR");
            stack_free(PAStack);
            return false;
        }
    }
    stack_free(PAStack);

    return true;

}