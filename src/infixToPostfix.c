#include "infixToPostfix.h"

int postfixArrCreateInit(t_array* fuckinArr){
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
    token_t* tmpToken = &fuckinArr->arr[fuckinArr->currLen];
    tmpToken->type = originalToken->type;

    //if string allocation is need - based on token type
    if (originalToken->type == TTYPE_STR || originalToken->type == TTYPE_ID){
        unsigned long length = strlen(originalToken->attribute.string);
        tmpToken->attribute.string = (char*)calloc(length, 1);
        if(tmpToken->attribute.string == NULL){
            printf("Token copy allocation error - string, change this error handling\n");
            return 1;
        }
        strcpy(tmpToken->attribute.string, originalToken->attribute.string);
    }
    //if not, string  should be NULL
    else{
        tmpToken->attribute.string = NULL;
    }
    //copy other attributes
    tmpToken->attribute.decimal = originalToken->attribute.decimal;
    tmpToken->attribute.integer = originalToken->attribute.integer;
    tmpToken->attribute.keyword = originalToken->attribute.keyword;

    fuckinArr->currLen++;
    return 0;
}




int infixToPostfix(stack_general_t* s, t_array* infixArray, t_array* postfixArr){
    if(s == NULL){
        printf("Inf2post stack is NULL\n");
        return 1;
    }

    int i =0;
    while(i == infixArray->currLen-1){
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
        //pop deletes token on the top
        stack_popNoDataFree(s);
        //deep copy to postfix array
        copyTokenToArray(postfixArr, (token_t*)tmpStackItem->data);
    }
    return 0;
}

void untilLeftPar(stack_general_t* s, t_array* postfixArr){
    while(!stack_empty(s)){
        stack_general_item_t * tmpStackItem = stack_general_top(s);
        stack_popNoDataFree(s);
        if(((token_t*)tmpStackItem->data)->type == TTYPE_LTBRAC){
            break;
        }
        copyTokenToArray(postfixArr, (token_t*)tmpStackItem->data);
    }
}

void doOperation(stack_general_t* s, token_t* token, t_array* postfixArr){
    if(stack_empty(s)){
        stack_general_push(s, token);
    }
    //Everything has higher priority than relational operators, so everything from stack is poped
    //and store into postfix until left bracket or empty stack
    else if(token->type == TTYPE_ISEQ || token->type == TTYPE_ISNEQ ||
    token->type == TTYPE_GT || token->type == TTYPE_GTOREQ ||
    token->type == TTYPE_LS || token->type == TTYPE_LSOREQ)
    {
        while(!stack_empty(s)){
            stack_general_item_t * tmpStackItem = stack_general_top(s);
            token_t* tmpToken = (token_t*)tmpStackItem->data;
            if(tmpToken->type == TTYPE_LTBRAC){
                break;
            }
            copyTokenToArray(postfixArr, tmpToken);
            stack_popNoDataFree(s);

        }
        stack_general_push(s, token);
    }
    else if(token->type == TTYPE_SUB || token->type == TTYPE_ADD){
        while(!stack_empty(s)) {
            stack_general_item_t * tmpStackItem = stack_general_top(s);
            token_t* tmpToken = (token_t*)tmpStackItem->data;
            if(tmpToken->type == TTYPE_LTBRAC){
                break;
            }
            if(token->type == TTYPE_ISEQ || token->type == TTYPE_ISNEQ ||
                token->type == TTYPE_GT || token->type == TTYPE_GTOREQ ||
                token->type == TTYPE_LS || token->type == TTYPE_LSOREQ)
            {
                break;
            }
            copyTokenToArray(postfixArr, tmpToken);
            stack_popNoDataFree(s);
        }
        stack_general_push(s, token);
    }
    else {
        while(!stack_empty(s)) {
            stack_general_item_t * tmpStackItem = stack_general_top(s);
            token_t* tmpToken = (token_t*)tmpStackItem->data;
            if(tmpToken->type == TTYPE_LTBRAC){
                break;
            }
            if(token->type == TTYPE_MUL || token->type == TTYPE_DIV || token->type == TTYPE_IDIV){
                copyTokenToArray(postfixArr, tmpToken);
                stack_popNoDataFree(s);
            }
            else{
                break;
            }
        }
        stack_general_push(s, token);
    }


}
