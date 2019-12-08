/**
 * @file main.c
 * @author Simon Sedlacek
 * @brief
 */

#include "parser.h"

int main(void) {
    int retvalue = init_resources();
    if (retvalue) return retvalue; 

    switch(retvalue = r_program()) {
        case SUCCESS:
            end_program();
            break;

        case ERROR_SYNTAX:
            fprintf(stderr, "syntax is incorrect\n");
            break;
            
        case ERROR_SEM_DEFINITION:
            fprintf(stderr, "semantic error - undefined symbol\n");
            break;

        case ERROR_SEM_TYPE:
            fprintf(stderr, "semantic error - invalid operand types\n");
            break;

        case ERROR_SEM_PARAM_COUNT:
            fprintf(stderr, "semantic error - invalid function parameter count\n");
            break;

        case ERROR_SEM_OTHER:
            fprintf(stderr, "semantic error - other\n");
            break;

        case ERROR_DIV_ZERO:
            fprintf(stderr, "error - division by zero\n");
            break;

        default:
            fprintf(stderr, "other error\n");
            break;
    }
    free_resources();
    return retvalue;
}
