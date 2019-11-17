/**
 *	@file   parser.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Parser 
 */

#include "parser.h"

int main(void) {
    init_resources();
    if (!r_program()) {
        printf("correct\n");
    } else {

    }
    return 0;
}

int init_resources() {
    return 0;
}
void free_resources() {
}

int r_program() {
    get_token(stdin, &pdata.current_token);
    if (data.current_token.type == TTYPE_EOF) {
        return SUCCESS;
    } else if (data.current_token.type == TTYPE_KEYWORD) {
        
    }
}
int r_statement() {
}
int r_statement_list() {
}
int r_function_def() {
}
int r_param_list() {
}
int r_params() {
}
int r_if_else() {
}
int r_cycle() {
}
int r_function_ret() {
}
int r_retvalue() {
}
int r_value() {
}
int r_rest() {
}
int r_function_call() {
}
int r_term() {
}
