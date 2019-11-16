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
            return PREC_SYMBOL_TABLE_ERROR;
    }
}


char get_prec_table_rule(stack_general_t* PAStack, int newSymbol){
    stack_general_item_t* top = stack_general_top(PAStack);
    int stackTerm =  *(int*)top->data;
    if(stackTerm == E){
        stackTerm = *(int*)top->next->data;
    }
    else if(stackTerm == newSymbol){
        printf("SAME SYMBOL TO STRACK AS IS ON STACK\n");
        return -1;
    }
    printf("TABLE RULE:[%d][%d]\n",stackTerm, newSymbol);
    return PREC_TABLE[stackTerm][newSymbol];

}

int reduce_on_stack(stack_general_t* PAStack, int toPush){
    printf("POPING 3 time\n");
    stack_pop(PAStack);
    stack_pop(PAStack);
    stack_pop(PAStack);
    if(stack_general_push_int(PAStack, toPush) == ALLOC_ERROR){
        printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
        return 1;
    }
    return 0;
}

int reduce(stack_general_t* PAStack){
    stack_general_item_t* tempStackItem = stack_general_top(PAStack);
    int operand1 = *(int*)tempStackItem->data;

    //Rule E->i
    if(operand1 == ID){
        printf("RULE E->i\n");
        stack_pop(PAStack);
        //If stack push fails
        if(stack_general_push_int(PAStack, E) == ALLOC_ERROR){
            printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
            return 1;
        }
        return 0;   
    }
    //getting second operand and operator to check for the rules
    tempStackItem = tempStackItem->next;
    int operator = *(int*)tempStackItem->data;
    tempStackItem = tempStackItem->next;
    int operand2 = *(int*)tempStackItem->data;

    printf("TWO OPERAND RULES");
    printf("{%d}{%d}{%d}\n", operand1, operator, operand2);

    if(operand1 == E && operand2 == E){
        //RULES E->E+E, E->E-E 
        if(operator == PLUS_MINUS){
            printf("RULE E->E+-E\n");
            if(reduce_on_stack(PAStack, E) == 1){
                return 1;
            }
        }

        //RULES E->E*E, E->E/E, E->E//E
        if(operator == MUL_DIV){
            printf("RULE E->E*/E\n");
            if(reduce_on_stack(PAStack, E) == 1){
                return 1;
            }
        }

        //RULES B->E==E, B->E<=E, B->E>=E, B->E!=E, B->E<E, B->E>E 
        if(operator == REL_OP){
            printf("RULE E->E<>==E\n");
            if(reduce_on_stack(PAStack, B) == 1){
                return 1;
            }
        }
    }
    //RULE E->(E)
    else if(operand1 == L_BRACKET && operand2 == R_BRACKET && operator == E){
        printf("RULE E->(E)\n");
        if(reduce_on_stack(PAStack, E) == 1){
            return 1;
        }
    }
    else{
        printf("SUM BULLSHIT\n");
        return 1;
    }
    return 0;
}


int apply_psa_rule( stack_general_t* PAStack, token_t newToken){
    //converting token to symbol for from precedence table
    int newSymbol = get_prec_table_symbol(newToken);
    printf("TOKEN SYMBOL[%d]\n",newSymbol);
    if(newSymbol == -1){
        printf("TABLE SYMBOL ERROR - given symbol is not in the precedence symbol table");
        return 1;
    }
    
    //precedence table rules for two terminals, one on stack on as input(newSymbol)
    char rule = get_prec_table_rule(PAStack, newSymbol);
    if(rule == -1){
        return 1;
    }
    printf("RULE %c\n", rule);

    //rule handling
    int reducing = FALSE;
    do{
        if(rule == P){
            printf("PUSH RULE\n");
            if(stack_general_push_int(PAStack, newSymbol) == ALLOC_ERROR) {
                printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION - when pushing to stack\n");
                return 1;
            }
            reducing = FALSE;//not sure
        }
        else if(rule == X){
            printf("TABLE RULE ERROR\n");
            return 1;
        }
        else if(rule == M){
            printf("STACK REDUCED CORRECTLY\n");
            reducing = FALSE;
        }
        else{
            printf("REDUCE RULE\n");
            if(reduce(PAStack)){
                printf("ERROR AFTER REDUCING\n");
                return -1;
            }
            //when stack is reduced, new rule have to be calculated
            rule = get_prec_table_rule(PAStack, newSymbol);
            printf("RULE %c\n", rule);
            if(rule == -1){
                return 1;
            }
            reducing = TRUE;
        }
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
        printf("TOKEN ERROR");
        stack_free(PAStack);
        return 1;
    }

    //loop through whole expression
    while(newToken.type != TTYPE_COLUMN && newToken.type != TTYPE_EOL){
        int psaCheck = apply_psa_rule(PAStack, newToken);
        printf("PSACHECK:%d\n",psaCheck);

        //incorrect syntax
        if(psaCheck) {
            printf("INCORRECT SYNTAX\n");
            stack_free(PAStack);
            return 1;
        }

        if(newToken.type == TTYPE_ID || newToken.type == TTYPE_STR || newToken.type == TTYPE_DOCSTR){
            free(newToken.attribute.string);
        }

        //getting new token and checking for error
        lex_analysis = get_token(f, &newToken);
        if(lex_analysis == 1){
            printf("GET TOKEN ERROR\n");
            stack_free(PAStack);
            return 1;
        }
    }

    stack_free(PAStack);
    return 0;

}