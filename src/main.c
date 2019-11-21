/**
 * @file main.c
 * @author Simon Sedlacek
 * @brief
 */

#include "parser.h"

int main(void) {
    init_resources();
    if (r_program() == SUCCESS) {
        printf("syntax is correct\n");
    } else {
        printf("syntax is incorrect\n");
    }
    free_resources();
    return 0;
}
