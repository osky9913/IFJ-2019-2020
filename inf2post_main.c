#include <stdio.h>
#include "src/infixToPostfix.h"


int main(){
    token_t curr;


    int check;
    t_array infixArr;
    t_array postfix;
    stack_general_t* s = stack_general_init();
    check = tokenArrCreateInit(&infixArr);


    if(check == ALLOC_ERROR) {
        printf("%d\n",check);
        return ERROR_INTERNAL;
    }
    check = tokenArrCreateInit(&postfix);
    if(check == ALLOC_ERROR) {
        printf("%d\n",check);
        freeArray(&infixArr);
        return ERROR_INTERNAL;
    }

    //nebude sa riesit v psa
    dent_stack = stack_general_init();
    if(get_token(&curr)){
        printf("lexika error\n");
        stack_free(dent_stack);
        stack_free(s);
        freeArray(&infixArr);
        freeArray(&postfix);
        return 1;
    }

    while(curr.type != TTYPE_EOF && curr.type != TTYPE_EOL){

        copyTokenToArray(&infixArr, &curr);

        if(get_token(&curr) != SUCCESS){
            stack_free(dent_stack);
            stack_free(s);
            freeArray(&infixArr);
            freeArray(&postfix);
            return ERROR_INTERNAL;
        }

    }

    infixToPostfix(s, &infixArr, &postfix);
    int precAnalysis = postfixEval(&postfix);
    switch(precAnalysis){
        case SUCCESS:
            printf("%d\n",precAnalysis);
            stack_free(dent_stack);
            freePsaResources(&infixArr, &postfix, s);
            return SUCCESS;
        case ERROR_SEM_DEFINITION:
            printf("%d\n",precAnalysis);
            stack_free(dent_stack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_SEM_DEFINITION;
        case ERROR_SEM_TYPE:
            printf("%d\n",precAnalysis);
            stack_free(dent_stack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_SEM_TYPE;
        case ERROR_DIV_ZERO:
            printf("%d\n",precAnalysis);
            stack_free(dent_stack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_DIV_ZERO;
        case ERROR_SEM_OTHER:
            printf("%d\n",precAnalysis);
            stack_free(dent_stack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_SEM_OTHER;
        default:
            printf("%d\n",precAnalysis);
            stack_free(dent_stack);
            freePsaResources(&infixArr, &postfix, s);
            return ERROR_INTERNAL;
    }
}
	
