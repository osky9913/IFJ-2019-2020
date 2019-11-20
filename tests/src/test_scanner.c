
#include "../../src/dynamic_string.h"
#include "../../src/scanner.h"
#include "../../src/general_stack.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int main() {
    //dent_stack = indent_stack_init();


    dent_stack = stack_general_init();
    token_t token; 

    int result;
    result = get_token(&token);


    while (token.type != TTYPE_EOF) {
        if(result){
            printf("<Error>");
            break;
        }
        if (token.type == TTYPE_EOL) {
            printf("<NEWLINE> \n");
        }
        if (token.type == TTYPE_STR || token.type == TTYPE_ID) {
            printf("<STRING or ID |%s| >  ", token.attribute.string);
            free(token.attribute.string);
        }
        if (token.type == TTYPE_INDENT) {
            printf("<INDEEENT> ");
        }
        if (token.type == TTYPE_DEDENT) {
            if (result) {
                printf("Dedent error!");
            } else {
                printf("<DEDENT> ");
            }
        }
        if (token.type == TTYPE_IDIV) {
            printf("<IDIV> ");
        }
        if (token.type == TTYPE_DIV) {
            printf("<DIV> ");
        }
        if (token.type == TTYPE_MUL) {
            printf("<MUL> ");
        }
        if (token.type == TTYPE_SUB) {
            printf("<SUB> ");
        }
        if (token.type == TTYPE_ADD) {
            printf("<ADD> ");
        }
        if (token.type == TTYPE_LS) {
            printf("<LS> ");
        }
        if (token.type == TTYPE_GT) {
            printf("<GT> ");
        }
        if (token.type == TTYPE_LSOREQ) {
            printf("<LSOREQ> ");
        }
        if (token.type == TTYPE_GTOREQ) {
            printf("<GTOREQ> ");
        }
        if (token.type == TTYPE_ISNEQ) {
            printf("<ISNEQ> ");
        }
        if (token.type == TTYPE_ISEQ) {
            printf("<ISEQ> ");
        }
        if (token.type == TTYPE_ASSIGN) {
            printf("<ASSIGN> ");
        }
        if (token.type == TTYPE_RTBRAC) {
            printf("<RTBRAC> ");
        }
        if (token.type == TTYPE_LTBRAC) {
            printf("<LTBRAC> ");
        }
        if (token.type == TTYPE_COLUMN) {
            printf("<COLUMN> ");
        }
        if (token.type == TTYPE_COMMA) {
            printf("<COMMA> ");
        }
        if (token.type == TTYPE_KEYWORD) {
            printf("<KEYWORD> ");
        }
        if (token.type == TTYPE_EOF) {
            printf("<EOF> ");
        }
        if (token.type == TTYPE_DOUBLE) {
            printf("<NUM |%f|> ", token.attribute.decimal);
        }
        if (token.type == TTYPE_INT) {
            printf("<NUM |%ld| >", token.attribute.integer);
        }
        result = get_token(&token);

    }
    if (token.type == TTYPE_EOF) {
            printf("<EOF> \n");
    }
    stack_free(dent_stack);
    //indent_stack_free(dent_stack);

    return 0;
}
