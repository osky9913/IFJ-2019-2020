/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   precedence_analysis.c
 * @author Marek Sarvas, xsarva00
 * @brief Precedence syntax analysis module
 */

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


    //getting rule from the precedence table
    return PREC_TABLE[stackTerm][newSymbol];

}

int reduce_on_stack(stack_general_t* PAStack, int toPush){


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
        stack_pop(PAStack);
        //If stack push fails
        if(stack_general_push_int(PAStack, E) == ALLOC_ERROR){
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

    //Applying rules to reduce items on stack
    if(operand1 == E && operand2 == E){
        //RULES E->E+E, E->E-E 
        if(operator == PLUS_MINUS){

            if(reduce_on_stack(PAStack, E) == 1){
                return 1;
            }
        }

        //RULES E->E*E, E->E/E, E->E//E
        if(operator == MUL_DIV){

            if(reduce_on_stack(PAStack, E) == 1){
                return 1;
            }
        }

        //RULES B->E==E, B->E<=E, B->E>=E, B->E!=E, B->E<E, B->E>E 
        if(operator == REL_OP){

            if(reduce_on_stack(PAStack, B) == 1){
                return 1;
            }
        }
    }
    //RULE E->(E)
    //left bracket is operand2 because stack pop gives right bracket first
    else if(operand1 == R_BRACKET && operand2 == L_BRACKET && operator == E){

        if(reduce_on_stack(PAStack, E) == 1){
            return 1;
        }
    }
    else if(operand1 == R_BRACKET && operand2 == L_BRACKET && operator == B){

        if(reduce_on_stack(PAStack, B) == 1){
            return 1;
        }
    }
    //better handle or delete
    else{
        return 1;
    }
    return 0;
}


int apply_psa_rule( stack_general_t* PAStack){
    //converting token to symbol for from precedence table
    int newSymbol = get_prec_table_symbol();


    //error handling, symbol should not be inside of an expression
    if(newSymbol == -1){

        return 1;
    }
    
    //precedence table rule chosen by newSymbol from the token and last terminal on stack
    char rule = get_prec_table_rule(PAStack, newSymbol);
    if(rule == -1){
        return 1;
    }


    //rule handling
    int reducing = 0;
    do{
        //only pushes symbol to stack
        if(rule == P){

            if(stack_general_push_int(PAStack, newSymbol) == ALLOC_ERROR) {

                return 1;
            }
            //select false to end loop
            reducing = 0;
        }

        //rule does not exist in the table -> cause an error
        else if(rule == X){

            return 1;
        }
        //rule chosen by two '$' symbols - end of the syntax expresion check everything is correct
        else if(rule == M){

            reducing = 0;
        }
        //stack has to be reduce by rules, reduce function is called
        else{

            if(reduce(PAStack)){
                return -1;
            }
            //when stack is reduced, new rule have to be calculated
            rule = get_prec_table_rule(PAStack, newSymbol);

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


int psa(const char* assignmentID){
    //declaration of needed data structures
    stack_general_t* PAStack = stack_general_init();//precedence analysis stack of symbols


    int check;
    t_array infixArr;
    t_array postfix;
    stack_general_t* s = stack_general_init();
    if(s == NULL){
        return ERROR_INTERNAL;
    }
    check = tokenArrCreateInit(&infixArr);
    if(check == ALLOC_ERROR) {
        return ERROR_INTERNAL;
    }
    check = tokenArrCreateInit(&postfix);
    if(check == ALLOC_ERROR) {
        freeArray(&infixArr);
        return ERROR_INTERNAL;
    }


    //pushing starting symbol - DOLLAR '$'
    if(stack_general_push_int(PAStack, DOLLAR) == ALLOC_ERROR){
        stack_free(PAStack);
        return ERROR_INTERNAL;
    }

    next_token(true);

    if (curr_token.type == TTYPE_EOL || curr_token.type == TTYPE_COLUMN) {
        stack_free(PAStack);
        freePsaResources(&infixArr, &postfix, s);
        fprintf(stderr, "Line %d - Syntax error in psa - expression cannot be empty.\n",
                line_counter);
        return ERROR_SYNTAX;

    }

    //loop through whole expression
    while(curr_token.type != TTYPE_COLUMN && curr_token.type != TTYPE_EOL){


        copyTokenToArray(&infixArr, &curr_token);

        //applying rules and checking for error indicating syntax error
        int psaCheck = apply_psa_rule(PAStack);


        //incorrect syntax
        if(psaCheck) {

            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            fprintf(stderr, "Line %d - Syntax error in psa\n", line_counter);
            return ERROR_SYNTAX;
        }

        next_token(true);
    }

    //applying rules and check for possible errors
    int psaCheck = apply_psa_rule(PAStack);
    if(psaCheck) {
        stack_free(PAStack);
        freePsaResources(&infixArr, &postfix, s);
        fprintf(stderr, "Line %d - Syntax error in psa\n", line_counter);
        return ERROR_SYNTAX;
    }

    //changing infix expression to postfix
    infixToPostfix(s, &infixArr, &postfix);

    //checking if relational operator is only in if or while statement
    int lastOperator = getPriority(&(postfix.arr[postfix.currLen-1]));
    if((psa_state == RETURN || psa_state == ASSIGN) && lastOperator == 0){
        unget_token();
        stack_free(PAStack);
        freePsaResources(&infixArr, &postfix, s);
        return ERROR_SEM_OTHER;
    }
    int precAnalysis = postfixEval(&postfix, assignmentID);

    switch(precAnalysis){
        case SUCCESS:
            unget_token();
            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            return SUCCESS;
        case ERROR_SEM_DEFINITION:
            fprintf(stderr, "%d\n",precAnalysis);
            unget_token();
            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_SEM_DEFINITION;
        case ERROR_SEM_TYPE:
            fprintf(stderr, "%d\n",precAnalysis);
            unget_token();
            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_SEM_TYPE;
        case ERROR_DIV_ZERO:
            fprintf(stderr, "%d\n",precAnalysis);
            unget_token();
            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_DIV_ZERO;
        case ERROR_SEM_OTHER:
            fprintf(stderr, "%d\n",precAnalysis);
            unget_token();
            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_SEM_OTHER;
        default:
            fprintf(stderr, "%d\n",precAnalysis);
            unget_token();
            stack_free(PAStack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_INTERNAL;
    }

}
