#include "infixToPostfix.h"
#include <string.h>


int tokenArrCreateInit(t_array* fuckinArr){
    fuckinArr->arr = calloc(INITLENGTH, sizeof(token_t));
    if(fuckinArr->arr == NULL) {
        printf("Postfix array allocation error, change this error handling\n");
        return 1;
    }
    fuckinArr->currLen = 0;
    fuckinArr->maxLen = INITLENGTH;
    return 0;
}

int resizeArrayIfNeeded(t_array* fuckinArr){
    if(fuckinArr->currLen == fuckinArr->maxLen-1){
        fuckinArr->maxLen *=2;
        fuckinArr->arr = realloc(fuckinArr->arr, sizeof(token_t)*fuckinArr->maxLen);
        if(fuckinArr->arr == NULL){
            printf("Reallocation error\n");
            return 1;
        }
    }
    return 0;
}

int copyTokenToArray(t_array* fuckinArr, const token_t* const originalToken){
    resizeArrayIfNeeded(fuckinArr);
    token_t* tmpToken = &fuckinArr->arr[fuckinArr->currLen];
    tmpToken->type = originalToken->type;

    //if string allocation is need - based on token type
    if (originalToken->type == TTYPE_STR || originalToken->type == TTYPE_ID){
        unsigned long length = strlen(originalToken->attribute.string);
        tmpToken->attribute.string = (char*)calloc(length+1, 1);
        if(tmpToken->attribute.string == NULL){
            printf("Token copy allocation error - string, change this error handling\n");
            return 1;
        }
        strcpy(tmpToken->attribute.string, originalToken->attribute.string);
    }
    //integer
    else if (originalToken->type == TTYPE_INT){
        tmpToken->attribute.integer = originalToken->attribute.integer;
    }
    //float
    else if(originalToken->type == TTYPE_DOUBLE) {
        tmpToken->attribute.decimal = originalToken->attribute.decimal;
    }
    fuckinArr->currLen++;
    return 0;
}

void freeArray(t_array* toDelete){
    for(int i = 0; i< toDelete->currLen ; i++){
        if(toDelete->arr[i].type == TTYPE_STR || toDelete->arr[i].type == TTYPE_ID){
            free(toDelete->arr[i].attribute.string);
        }
    }
    free(toDelete->arr);
    toDelete->arr = NULL;
}

void printArray(t_array* toPrint){
    for(int i = 0; i< toPrint->currLen ; i++){
        switch(toPrint->arr[i].type){
            case TTYPE_STR:
                printf("%s", toPrint->arr[i].attribute.string);
            break;
            case TTYPE_ID:
                printf("%s", toPrint->arr[i].attribute.string);
                break;
            case TTYPE_INT:
                printf("%ld", toPrint->arr[i].attribute.integer);
                break;
            case TTYPE_DOUBLE:
                printf("%f", toPrint->arr[i].attribute.decimal);
                break;
            case TTYPE_ADD:
                printf("+");
                break;
            case TTYPE_SUB:
                printf("-");
                break;
            case TTYPE_DIV:
                printf("/");
                break;
            case TTYPE_IDIV:
                printf("//");
                break;
            case TTYPE_MUL:
                printf("*");
                break;
            case TTYPE_ISEQ:
                printf("==");
                break;
            case TTYPE_ISNEQ:
                printf("!=");
                break;
            case TTYPE_GT:
                printf(">");
                break;
            case TTYPE_GTOREQ:
                printf(">=");
                break;
            case TTYPE_LS:
                printf("<");
                break;
            case TTYPE_LSOREQ:
                printf("<=");
                break;
            default:
                break;
        }
    }
}


int infixToPostfix(stack_general_t* s, t_array* infixArray, t_array* postfixArr){
    if(s == NULL){
        printf("Inf2post stack is NULL\n");
        return 1;
    }
    int i =0;
    while(i != infixArray->currLen){
        printArray(postfixArr);
        printf("\n");
        if(infixArray->arr[i].type == TTYPE_ADD ||
           infixArray->arr[i].type == TTYPE_SUB ||
           infixArray->arr[i].type == TTYPE_MUL ||
           infixArray->arr[i].type == TTYPE_DIV ||
           infixArray->arr[i].type == TTYPE_IDIV ||
           infixArray->arr[i].type == TTYPE_ISEQ ||
           infixArray->arr[i].type == TTYPE_ISNEQ ||
           infixArray->arr[i].type == TTYPE_GT ||
           infixArray->arr[i].type == TTYPE_GTOREQ ||
           infixArray->arr[i].type == TTYPE_LS ||
           infixArray->arr[i].type == TTYPE_LSOREQ
           ){

            doOperation(s, &(infixArray->arr[i]), postfixArr);
        }
        else if(infixArray->arr[i].type == TTYPE_RTBRAC){
            untilLeftPar(s, postfixArr);
        }
        else if(infixArray->arr[i].type == TTYPE_LTBRAC){
            stack_general_push(s, &(infixArray->arr[i]));
        }
        else{
            copyTokenToArray(postfixArr, &infixArray->arr[i]);
        }
        i++;
    }

    while(!stack_empty(s)) {
        //gets token from the top of the stack
        stack_general_item_t * tmpStackItem = stack_general_top(s);
        //deep copy to postfix array
        copyTokenToArray(postfixArr, (token_t*)tmpStackItem->data);
        //pop deletes token on the top
        stack_popNoDataFree(s);
    }
    return 0;
}

void untilLeftPar(stack_general_t* s, t_array* postfixArr){
    while(!stack_empty(s)){
        stack_general_item_t * tmpStackItem = stack_general_top(s);

        if(((token_t*)tmpStackItem->data)->type == TTYPE_LTBRAC){
            stack_popNoDataFree(s);
            break;
        }
        copyTokenToArray(postfixArr, (token_t*)tmpStackItem->data);
        stack_popNoDataFree(s);
    }
}

void doOperation(stack_general_t* s, token_t* newToken, t_array* postfixArr) {
    if (stack_empty(s)) {
        stack_general_push(s, newToken);
        return;
    }

    int newTokenPrior = getPriority(newToken);
        //Everything has higher priority than relational operators, so everything from stack is poped
        //and store into postfix until left bracket or empty stack
    if (newTokenPrior == 0) {
        while (!stack_empty(s)) {
            stack_general_item_t *tmpStackItem = stack_general_top(s);
            token_t *stackToken = (token_t *) tmpStackItem->data;
            if (stackToken->type == TTYPE_LTBRAC) {
                break;
            }
            copyTokenToArray(postfixArr, stackToken);
            stack_popNoDataFree(s);
        }
        stack_general_push(s, newToken);
    }
    else if (newTokenPrior == 1) {
        while (!stack_empty(s)) {
            stack_general_item_t *tmpStackItem = stack_general_top(s);
            token_t *stackToken = (token_t *) tmpStackItem->data;
            int topStackPrior = getPriority((token_t *) tmpStackItem->data);
            if (stackToken->type == TTYPE_LTBRAC) {
                break;
            }
            if (topStackPrior < newTokenPrior) {
                break;
            }
            copyTokenToArray(postfixArr, stackToken);
            stack_popNoDataFree(s);
        }
        stack_general_push(s, newToken);
    }
    else if(newTokenPrior == 2){
        while (!stack_empty(s)) {
            stack_general_item_t *tmpStackItem = stack_general_top(s);
            token_t *stackToken = (token_t *) tmpStackItem->data;
            int topStackPrior = getPriority((token_t *) tmpStackItem->data);
            if (stackToken->type == TTYPE_LTBRAC) {
                break;
            }
            if (topStackPrior >= newTokenPrior) {
                copyTokenToArray(postfixArr, stackToken);
                stack_popNoDataFree(s);
            } else if(topStackPrior < newTokenPrior){
                break;
            }
        }
        stack_general_push(s, newToken);
    }
    else{
        printf("error unknown token\n");
    }
}

int getPriority(const token_t* const token){
    switch(token->type){
        case TTYPE_ISNEQ:
        case TTYPE_ISEQ:
        case TTYPE_GT:
        case TTYPE_GTOREQ:
        case TTYPE_LS:
        case TTYPE_LSOREQ:
            return 0;
        case TTYPE_ADD:
        case TTYPE_SUB:
            return 1;
        case TTYPE_MUL:
        case TTYPE_DIV:
        case TTYPE_IDIV:
            return 2;
        default:
            return -1;
    }
}

int postfixEval(t_array* postfix){
    stack_general_t* tokenGarbageS = stack_general_init();
    stack_general_t* evalS = stack_general_init();
    token_t currentToken = postfix->arr[0];
    for(int i = 0; i < postfix->currLen; i++){
        if(!isOperator(&(postfix->arr[i]))){
            stack_general_push(evalS, &(postfix->arr[i]));
            continue;
        }
            stack_general_item_t *tmpStackItem1 = stack_general_top(evalS);
            token_t *operand1 = (token_t *) tmpStackItem1->data;
            stack_popNoDataFree(evalS);
            stack_general_item_t *tmpStackItem2 = stack_general_top(evalS);
            token_t *operand2 = (token_t *) tmpStackItem2->data;
            stack_popNoDataFree(evalS);

        int semantic = checkSemantic(operand1, operand2, &currentToken);
        if(semantic == 0){
            //code gen -return string
            /************************************/
            //will be deleted
            char* name = malloc(10);
            strcpy(name, "qwertyuio");
            /***********************************/
            token_t* newToken = token_gen(name);
            stack_general_push(tokenGarbageS, newToken);
            stack_general_push(evalS, newToken);
        }
        else{
            return -1;
        }
    }
    return 0;
}

bool isOperator(const token_t* const token){
    switch(token->type){
        case TTYPE_ISNEQ:
        case TTYPE_ISEQ:
        case TTYPE_GT:
        case TTYPE_GTOREQ:
        case TTYPE_LS:
        case TTYPE_LSOREQ:
        case TTYPE_ADD:
        case TTYPE_SUB:
        case TTYPE_MUL:
        case TTYPE_DIV:
        case TTYPE_IDIV:
            return true;
        default:
            return false;
    }
}

token_t* token_gen(char* name){
    token_t* tmp = malloc(sizeof(token_t));
    tmp->type = TTYPE_ID;
    tmp->attribute.string = name;
    return tmp;
}


int checkSemantic(token_t *operand1, token_t *operand2, token_t *operator){
    if(operand1->type == TTYPE_ID){
        bool check = check_if_defined_var(operand1->attribute.string);
        if(!check){
            printf("Not defined variable error\n");
            return 1;
        }
    }
    if(operand2->type == TTYPE_ID){
        bool check = check_if_defined_var(operand2->attribute.string);
        if(!check){
            printf("Not defined variable error\n");
            return 1;
        }
    }
    if((operand1->type == TTYPE_INT || operand1->type == TTYPE_DOUBLE) && (operand2->type == TTYPE_INT || operand2->type == TTYPE_DOUBLE)){
        if(isOperator(operator)){
            if(operator->type == TTYPE_IDIV){
                if(operand1->type == TTYPE_DOUBLE || operand2->type == TTYPE_DOUBLE){
                    return 1;
                }
                else if(operand1->attribute.integer == 0){
                    return 1;
                }
                return 0;
            }
            if(operator->type == TTYPE_DIV) {
                if (operand1->type == TTYPE_INT && operand1->attribute.integer == 0) {
                    return 1;
                }
                return 0;
            }
        }
        else{
            return 1;
        }
    }
    if(operand1->type == TTYPE_STR && operand2->type == TTYPE_STR){
        if(operator->type == TTYPE_SUB || operator->type == TTYPE_MUL ||
        operator->type == TTYPE_DIV || operator->type == TTYPE_IDIV){
            return 1;
        }
        return 0;
    }
    if((operand1->type == TTYPE_STR && operand2->type != TTYPE_STR) || (operand2->type == TTYPE_STR && operand1->type != TTYPE_STR)){
        return 1;
    }
    if((operator->type == TTYPE_ISNEQ || operator->type == TTYPE_ISEQ || operator->type == TTYPE_GT || operator->type == TTYPE_GTOREQ ||
       operator->type == TTYPE_LS || operator->type == TTYPE_LSOREQ ) && operand1->type != operand2->type){
            return 1;
    }
    return 0;
}
