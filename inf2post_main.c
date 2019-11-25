#include <stdio.h>
#include "src/infixToPostfix.h"


int main(){
    token_t curr;
    int check;
    t_array infixArr;
    t_array postfix;
    stack_general_t* s = stack_general_init();
    check = tokenArrCreateInit(&infixArr);
    if(check == 1) {
        printf("FUCK YOU\n");
        return 1;
    }
    check = tokenArrCreateInit(&postfix);
    if(check == 1) {
        printf("FUCK YOU\n");
        return 1;
    }

    dent_stack = stack_general_init();
    if(get_token(&curr)){
        printf("lexika je napicu\n");
        stack_free(dent_stack);
        stack_free(s);
        freeArray(&infixArr);
        freeArray(&postfix);
        return 1;
    }

    while(curr.type != TTYPE_EOF && curr.type != TTYPE_EOL){
        copyTokenToArray(&infixArr, &curr);
        if(get_token(&curr)){
            printf("lexika je napicu\n");
            stack_free(dent_stack);
            stack_free(s);
            freeArray(&infixArr);
            freeArray(&postfix);
            return 1;
        }

    }
    infixToPostfix(s, &infixArr, &postfix);
    if(postfixEval(&postfix)){
        printf("je to napicu\n");
    }


    stack_free(dent_stack);
    stack_free(s);
    freeArray(&infixArr);
    freeArray(&postfix);

    return 0;
}
	
