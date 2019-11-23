#include "precedence_analysis.h"

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

int get_prec_table_symbol(){
    switch(curr_token.type){
        case TTYPE_ID:
        case TTYPE_INT:
        case TTYPE_DOUBLE:
        case TTYPE_STR:
        case TTYPE_NONE:
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

        case TTYPE_COLUMN:
        case TTYPE_EOL:
            return DOLLAR;

        default:
            return PREC_SYMBOL_TABLE_ERROR;
    }
}


char get_prec_table_rule(stack_general_t* PAStack, int newSymbol){
    stack_general_item_t* top = stack_general_top(PAStack);
    int stackTerm =  *(int*)top->data;

    //if on the top of the stack is non-terminal we grab next item which should be terminal
    if(stackTerm == E || stackTerm == B){
        stackTerm = *(int*)top->next->data;
    }

    printf("TABLE RULE:[%d][%d]\n",stackTerm, newSymbol);
    //getting rule from the precedence table
    return PREC_TABLE[stackTerm][newSymbol];

}

int reduce_on_stack(stack_general_t* PAStack, int toPush){
    printf("POPING 3 times\n");

    //checking for empty stack after each pop, empty cause an error (should be syntax error ->
    // cannot reduce 3 items if theres less then 4 on stack (3 symbols plus $)
    stack_pop(PAStack);
    if(stack_empty(PAStack)){
        return 1;
    }
    stack_pop(PAStack);
    if(stack_empty(PAStack)){
        return 1;
    }
    stack_pop(PAStack);
    if(stack_empty(PAStack)){
        return 1;
    }

    //allocation error handling when pushing on stack
    if(stack_general_push_int(PAStack, toPush) == ALLOC_ERROR){
        printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
        return 1;
    }
    return 0;
}

int reduce(stack_general_t* PAStack){

    //getting first item on the stack to decide which rule to use
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

    //getting second operand and operator to check for the rules, check if there is less items on
    //stack in that case cause error
    tempStackItem = tempStackItem->next;
    if(tempStackItem == NULL){
        return 1;
    }
    int operator = *(int*)tempStackItem->data;


    tempStackItem = tempStackItem->next;
    if(tempStackItem == NULL){
        return 1;
    }
    int operand2 = *(int*)tempStackItem->data;

    printf("TWO OPERAND RULES");
    printf("{%d}{%d}{%d}\n", operand1, operator, operand2);

    //Applying rules to reduce items on stack
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
    //left bracket is operand2 because stack pop gives right bracket first
    else if(operand1 == R_BRACKET && operand2 == L_BRACKET && operator == E){
        printf("RULE E->(E)\n");
        if(reduce_on_stack(PAStack, E) == 1){
            return 1;
        }
    }
    else if(operand1 == R_BRACKET && operand2 == L_BRACKET && operator == B){
        printf("RULE B->(B)\n");
        if(reduce_on_stack(PAStack, B) == 1){
            return 1;
        }
    }
    //better handle or delete
    else{
        printf("SUM BULLSHIT\n");
        return 1;
    }
    return 0;
}


int apply_psa_rule( stack_general_t* PAStack){
    //converting token to symbol for from precedence table
    int newSymbol = get_prec_table_symbol();

    printf("TOKEN SYMBOL[%d]\n",newSymbol);

    //error handling, symbol should not be inside of an expression
    if(newSymbol == -1){
        printf("TABLE SYMBOL ERROR - given symbol is not in the precedence symbol table");
        return 1;
    }
    
    //precedence table rule chosen by newSymbol from the token and last terminal on stack
    char rule = get_prec_table_rule(PAStack, newSymbol);
    if(rule == -1){
        return 1;
    }
    printf("RULE %c\n", rule);

    //rule handling
    int reducing = 0;
    do{
        //only pushes symbol to stack
        if(rule == P){
            printf("PUSH RULE\n");
            if(stack_general_push_int(PAStack, newSymbol) == ALLOC_ERROR) {
                printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION - when pushing to stack\n");
                return 1;
            }
            //select false to end loop
            reducing = 0;
        }

        //rule does not exist in the table -> cause an error
        else if(rule == X){
            printf("TABLE RULE ERROR\n");
            return 1;
        }
        //rule chosen by two '$' symbols - end of the syntax expresion check everything is correct
        else if(rule == M){
            printf("STACK REDUCED CORRECTLY\n");
            reducing = 0;
        }
        //stack has to be reduce by rules, reduce function is called
        else{
            printf("REDUCE RULE\n");
            if(reduce(PAStack)){
                printf("ERROR AFTER REDUCING\n");
                return -1;
            }
            //when stack is reduced, new rule have to be calculated
            rule = get_prec_table_rule(PAStack, newSymbol);

            printf("RULE %c\n", rule);
            //new rule error handling
            if(rule == -1){
                return 1;
            }

            //select true to repeat process with new rule calculated and same new symbol from token
            reducing = 1;
        }
    }while(reducing);

    return 0;
}


int psa(){
    //declaration of needed data structures
    stack_general_t* PAStack = stack_general_init();//precedence analysis stack of symbols

    //pushing starting symbol - DOLLAR '$'
    if(stack_general_push_int(PAStack, DOLLAR) == ALLOC_ERROR){
        printf("ALLOC ERROR IN PRECEDENCE ANALYSIS REDUCING FUNCTION\n");
        stack_free(PAStack);
        return 1;
    }

    next_token(true);

    //loop through whole expression
    while(curr_token.type != TTYPE_COLUMN && curr_token.type != TTYPE_EOL){

        //applying rules and checking for error indicating syntax error
        int psaCheck = apply_psa_rule(PAStack);
        printf("PSACHECK:%d\n",psaCheck);

        //incorrect syntax
        if(psaCheck) {
            printf("INCORRECT SYNTAX\n");
            stack_free(PAStack);
            return 1;
        }

        next_token(true);
    }

    //applying rules and check for possible errors
    int psaCheck = apply_psa_rule(PAStack);
    if(psaCheck) {
        printf("INCORRECT SYNTAX\n");
        stack_free(PAStack);
        return 1;
    }
    unget_token();

    //free stack memory
    stack_free(PAStack);
    return 0;

}
