
#include "../../src/general_stack.h"

/**/
/************************* c202-advanced-test.c *******************************/
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c202 - Zásobník znaků v poli                                        */
/*  Vytvořil: Václav Topinka, září 2005                                       */
/*  Úpravy: Bohuslav Křena, říjen 2016                                        */
/* ************************************************************************** */

/* Pokročilé testy pro c202.c */


#include <stdio.h>
#include <stdlib.h>




/****************************************************************************** 
 * Speciální ošetření testovaných funkcí.                                     *
 ******************************************************************************/

/* Vytiskne obsah zásobníku. */
void stackPrint(stack_general_t *ptrstack) {
    if (ptrstack == NULL) {
        printf("nealokovany stack \n");

        return;
    }
    stack_general_item_t *temp = ptrstack->top;
    if (temp == NULL) {
        printf("prazdny stack \n");
        return;
    }
    printf("--- BOTTOM [ ");
    while (temp != NULL) {
        putchar(*(char *) temp->data);

        /* if( temp->data == NULL ) {
             printf("data sa rovaju null\n");
             return ;
         }
         else{*/
        // return ;
        //}

        //char premena =*( char*) temp->data;
        //printf("%c" , premena);
        temp = temp->next;
    }
    printf(" ] TOP ---\n");
}

/* Zavolá funkci stackInit a v případě, že nebyla řešena, ohlásí to. */
stack_general_t *use_stack_init() {
    return stack_general_init();
}

/* Zavolá funkci stackEmpty a vytiskne výsledek. */
void use_stack_empty(stack_general_t *ptrstack) {

    bool i = stack_empty(ptrstack);
    printf("stackEmpty returned '%s'\n", i ? "TRUE" : "FALSE");
}


/* Zavolá funkci stackTop a vytiskne výsledek. */
void use_stack_top(stack_general_t *ptrstack) {

    char c;
    stack_general_item_t *temp = stack_general_top(ptrstack);
    if (temp != NULL) {
        c = *(char *) (temp->data);
    } else {
        c = '0';
    }
    printf("stackTop returned '%c'\n", c);
}

/* Zavolá funkci stackPop. */
void use_stack_pop(stack_general_t *ptrstack) {

    stack_pop(ptrstack);

}

/* Zavolá funkci stackPush. */
void use_stack_push(stack_general_t *ptrstack, char c) {

    stack_general_push_char(ptrstack, c);

}

/****************************************************************************** 
 * Vlastní testování                                                          *
 ******************************************************************************/

int main() {
    printf("C202 - Stack Implemented Using an Array - Advanced Tests\n");
    printf("--------------------------------------------------------\n");

    printf("\n[TEST02] We check a stack of size 1.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    stack_general_t *ptrstack = use_stack_init();
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST03] Pop should do nothing here ...\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_pop(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST04] ... while Top should cause an error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_top(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST05] We push 'O' as the first item.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_push(ptrstack, 'O');
    //stack_general_push(ptrstack,NULL);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST06] We try to push another item. It should cause an error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_push(ptrstack, 'K');
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST07] And once again.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_push(ptrstack, '!');
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST08] What is on the top now?\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_top(ptrstack);

    printf("\n[TEST09] We remove it ...\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_pop(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST10] ... and check behaviour of Pop and Top.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_pop(ptrstack);
    use_stack_top(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST11] Now, we check a stack of maximum size.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST12] Pop should do nothing here ...\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_pop(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST13] ... while Top should cause an error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_top(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST14] Pushing small letters (but we have no enough space for that).\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    for (int i = 0; i < 26; i++)
        use_stack_push(ptrstack, 'a' + i);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);


    printf("\n[TEST15] What is on the top now?\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    stackPrint(ptrstack);
    use_stack_top(ptrstack);

    printf("\n[TEST16] Pushing another item should cause another error.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_push(ptrstack, 'X');
    use_stack_empty(ptrstack);


    printf("\n[TEST17] The top item should be the same.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    stackPrint(ptrstack);
    use_stack_top(ptrstack);

    printf("\n[TEST18] We remove all items without proper counting ...\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    stack_clear(ptrstack);

    stackPrint(ptrstack);
    use_stack_empty(ptrstack);

    printf("\n[TEST19] ... and check behaviour of Pop and Top.\n");
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    use_stack_pop(ptrstack);
    use_stack_pop(ptrstack);
    use_stack_top(ptrstack);
    use_stack_top(ptrstack);
    stackPrint(ptrstack);
    use_stack_empty(ptrstack);
    printf("\n\n----- C202 - The End of Advanced Tests -----\n");

    stack_free(ptrstack);
    return (0);
}

/* Konec c202-advanced-test.c */
/**/

