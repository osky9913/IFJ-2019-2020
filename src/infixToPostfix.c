#include "infixToPostfix.h"
#include <string.h>


int tokenArrCreateInit(t_array* newArr){
    //allocate memory for the array of tokens
    newArr->arr = calloc(INITLENGTH, sizeof(token_t));
    if(newArr->arr == NULL) {
        printf("Postfix array allocation error, change this error handling\n");
        return 1;
    }
    
    //initializes array values
    newArr->currLen = 0;
    newArr->maxLen = INITLENGTH;
    return 0;
}

//realloc memory for the array if it's full and new token needs to be added
int resizeArrayIfNeeded(t_array* toresizeArr){
    if(toresizeArr->currLen == toresizeArr->maxLen - 1){
        toresizeArr->maxLen *=2;
        toresizeArr->arr = realloc(toresizeArr->arr, sizeof(token_t) * toresizeArr->maxLen);
        if(toresizeArr->arr == NULL){
            printf("Reallocation error\n");
            return 1;
        }
    }
    return 0;
}


int copyTokenToArray(t_array* arrayToCopy, const token_t* const originalToken){
    //checks if array is not full, if so, resizes it's length
    resizeArrayIfNeeded(arrayToCopy);

    //create new token which will be add to the
    token_t* tmpToken = &arrayToCopy->arr[arrayToCopy->currLen];
    tmpToken->type = originalToken->type;

    //if string allocation is need - based on token type
    if (originalToken->type == TTYPE_STR || originalToken->type == TTYPE_ID){
        
        //get length of string which will be copied
        unsigned long length = strlen(originalToken->attribute.string);
        //make room for zero at the end
        length++;
        
        //allocate memory for the string
        tmpToken->attribute.string = (char*)calloc(length, 1);
        if(tmpToken->attribute.string == NULL){
            printf("Token copy allocation error - string, change this error handling\n");
            return 1;
        }
        
        //copy string from given token to the new one
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
    //increment current number of tokens in array as current length
    arrayToCopy->currLen++;
    return 0;
}

//free all memory allocated for given array
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

        /************* REMOVE LATER *************/
        printArray(postfixArr);
        printf("\n");
        /************* REMOVE LATER *************/

        //infix to postfix algorithm
        if(isOperator(&(infixArray->arr[i]))){
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
        //get item from top of the stack
        stack_general_item_t * tmpStackItem = stack_general_top(s);

        //make pointer to token from stack item - type conversion to token_t* than access token type
        if(((token_t*)tmpStackItem->data)->type == TTYPE_LTBRAC){//if left bracket is on the top pop it and break from loop
            stack_popNoDataFree(s);
            break;
        }
        //else push it to the postfix array and pop it from the stack
        copyTokenToArray(postfixArr, (token_t*)tmpStackItem->data);
        stack_popNoDataFree(s);
    }
}


void doOperation(stack_general_t* s, token_t* newToken, t_array* postfixArr) {
    //if stack is empty push new token to it
    if (stack_empty(s)) {
        stack_general_push(s, newToken);
        return;
    }

    int newTokenPrior = getPriority(newToken);

    //Everything has higher priority than relational operators, so everything from stack is poped
    //and stored into postfix array until left bracket or empty stack
    if (newTokenPrior == 0) {
        while (!stack_empty(s)) {
            //getting item from top of the stack and converting it to token
            stack_general_item_t *tmpStackItem = stack_general_top(s);
            token_t *stackToken = (token_t *) tmpStackItem->data;

            //if token is left bracket it breaks the loop otherwise it's looping until empty stack
            if (stackToken->type == TTYPE_LTBRAC) {
                break;
            }
            //every iteration is item from the top added to postfix array and then poped from the stack
            copyTokenToArray(postfixArr, stackToken);
            stack_popNoDataFree(s);
        }
        //after loop push new token to the stack
        stack_general_push(s, newToken);
    }
    
    //'+' and '-' have priority 1
    else if (newTokenPrior == 1) {
        while (!stack_empty(s)) {
            //getting item from top of the stack and converting it to token
            stack_general_item_t *tmpStackItem = stack_general_top(s);
            token_t *stackToken = (token_t *) tmpStackItem->data;
            
            //than converting token to priority
            int topStackPrior = getPriority((token_t *) tmpStackItem->data);
            
            //token on the top is either left bracket or token with lower priority break the loop
            if (stackToken->type == TTYPE_LTBRAC) {
                break;
            }
            if (topStackPrior < newTokenPrior) {
                break;
            }
            
            //every iteration is item from the top added to postfix array and then poped from the stack
            copyTokenToArray(postfixArr, stackToken);
            stack_popNoDataFree(s);
        }
        //after loop push new token to the stack
        stack_general_push(s, newToken);
    }
    
    //'*', '/' and '//' have priority 2
    else if(newTokenPrior == 2){
        while (!stack_empty(s)) {
            //getting item from top of the stack and converting it to token
            stack_general_item_t *tmpStackItem = stack_general_top(s);
            token_t *stackToken = (token_t *) tmpStackItem->data;

            //than converting token to priority
            int topStackPrior = getPriority(stackToken);

            //token on the top is either left bracket or token with lower priority break the loop,
            //otherwise it's looping until empty stack
            if (stackToken->type == TTYPE_LTBRAC) {
                break;
            }
            if(topStackPrior < newTokenPrior){
                break;
            }

            //every iteration is item from the top added to postfix array and then poped from the stack
            copyTokenToArray(postfixArr, stackToken);
            stack_popNoDataFree(s);
        }
        //after loop push new token to the stack
        stack_general_push(s, newToken);
    }
}

//get priority from token based on token type
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

//evaluate postfix expression
int postfixEval(t_array* postfix){

    //creating stack for collecting new semi-result tokens
    stack_general_t* tokenGarbageS = stack_general_init();
    //creating stack for postfix evaluation algorithm
    stack_general_t* evalS = stack_general_init();

    //get first token before loop
    token_t currentToken = postfix->arr[0];
    for(int i = 0; i < postfix->currLen; i++){
        //operands are pushed on stack
        if(!isOperator(&(postfix->arr[i]))){
            stack_general_push(evalS, &(postfix->arr[i]));
            continue;
        }
        //every operator is binary so 2 operands are poped from the stack
        stack_general_item_t *tmpStackItem1 = stack_general_top(evalS);
        token_t *operand1 = (token_t *) tmpStackItem1->data;
        stack_popNoDataFree(evalS);

        stack_general_item_t *tmpStackItem2 = stack_general_top(evalS);
        token_t *operand2 = (token_t *) tmpStackItem2->data;
        stack_popNoDataFree(evalS);

        //checks if all variables in expression are defined
        int checkDefine = checkDefinedVarInPostfix(postfix);
        if(checkDefine == SYMBOL_NOT_FOUND){
            printf("SYMBOL NOT FOUND\n");
            stack_free(tokenGarbageS);
            stack_free(evalS);
            return 1;
        }
        //check if expression semantic is correct
        int semantic = checkSemantic(operand1, operand2, &currentToken);

        //if semantic is correct it generates code for binary expression
        if(semantic == 0){
            //code gen -return string

            /*****************DELETE THIS*******************/
            char* name = malloc(10);
            strcpy(name, "qwertyuio");
            /****************DELETE THIS*******************/

            //generate token for semi-result
            token_t* newToken = tokenGen(name);
            //pushed it to the postfix evaluation stack as operand
            stack_general_push(evalS, newToken);
            //also pushed it to new tokens array so it's memory can be freed later
            stack_general_push(tokenGarbageS, newToken);
        }
        else{
            stack_free(evalS);
            stack_free(tokenGarbageS);
            return -1;
        }
    }
    stack_free(evalS);
    stack_free(tokenGarbageS);
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

//generate new token as semi-result in postfix evaluation
token_t* tokenGen(char* name){
    token_t* tmp = malloc(sizeof(token_t));
    tmp->type = TTYPE_ID;
    tmp->attribute.string = name;
    return tmp;
}

//loop through the postfix array and if token is variable check if it's defined
int checkDefinedVarInPostfix(t_array* postfix){
    int checkDef;
    for(int i = 0; i < postfix->currLen ; i++){
        if(postfix->arr[i].type == TTYPE_ID){
            checkDef = check_if_defined_var(postfix->arr[i].attribute.string);
            if(checkDef == SYMBOL_NOT_FOUND){
                return SYMBOL_NOT_FOUND;
            }
        }
    }
    return VARIABLE_FOUND;
}

int checkSemantic(token_t *operand1, token_t *operand2, token_t *operator){
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
    int priority = getPriority(operator);
    if(priority == 0 && operand1->type != operand2->type){
            return 1;
    }
    return 0;
}
