/**
 * @file main.c
 * @author Simon Sedlacek
 * @brief
 */

#include "parser.h"
#include "errors.h"

int main(void) {
    init_resources();
    int retvalue;
    switch(retvalue = r_program()) {
        case SUCCESS:
            printf("program is correct\n");
            break;

        case ERROR_SYNTAX:
            printf("syntax is incorrect\n");
            break;
            
        case ERROR_SEM_DEFINITION:
            printf("semantic error - undefined symbol\n");
            break;

        case ERROR_SEM_PARAM_COUNT:
            printf("semantic error - invalid function parameter count\n");
            break;

        default:
            printf("other error\n");
            break;

    }
    free_resources();
    return 0;
}
