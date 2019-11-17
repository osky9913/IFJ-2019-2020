
#include "../../src/dynamic_string.h"
#include "../../src/indent_stack.h"
#include "../../src/scanner.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[]) {
    FILE *f;
    if (argc == 1) {
        printf("Neni zadan vstupni soubor\n");
        return 0;
    }
    if ((f = fopen(argv[1], "r")) == NULL) {
        printf("Soubor se nepodarilo otevrit\n");
        return 0;
    }
    dent_stack = indent_stack_init();

    token_t token;

    int result;
    result = get_token(f, &token);


    while (token.type != TTYPE_EOF) {
        if(result){
            printf("Error\n");
            break;
        }
        if (token.type == TTYPE_EOL) {
            printf("NEWLINE\n");
        }
        if (token.type == TTYPE_STR || token.type == TTYPE_DOCSTR || token.type == TTYPE_ID) {
            printf("STRING or ID found |%s|\n", token.attribute.string);
            free(token.attribute.string);
        }
        if (token.type == TTYPE_INDENT) {
            printf("INDEEENT \n");
        }
        if (token.type == TTYPE_DEDENT) {
            if (result) {
                printf("Dedent error!\n");
            } else {
                printf("DEDEEENT \n");
            }
        }
        if (token.type == TTYPE_IDIV) {
            printf("IDIV \n");
        }
        if (token.type == TTYPE_DIV) {
            printf("DIV \n");
        }
        if (token.type == TTYPE_MUL) {
            printf("MUL \n");
        }
        if (token.type == TTYPE_SUB) {
            printf("SUB \n");
        }
        if (token.type == TTYPE_ADD) {
            printf("ADD \n");
        }
        if (token.type == TTYPE_LS) {
            printf("LS \n");
        }
        if (token.type == TTYPE_GT) {
            printf("GT \n");
        }
        if (token.type == TTYPE_LSOREQ) {
            printf("LSOREQ \n");
        }
        if (token.type == TTYPE_GTOREQ) {
            printf("GTOREQ \n");
        }
        if (token.type == TTYPE_ISNEQ) {
            printf("ISNEQ \n");
        }
        if (token.type == TTYPE_ISEQ) {
            printf("ISEQ \n");
        }
        if (token.type == TTYPE_ASSIGN) {
            printf("ASSIGN \n");
        }
        if (token.type == TTYPE_RTBRAC) {
            printf("RTBRAC \n");
        }
        if (token.type == TTYPE_LTBRAC) {
            printf("LTBRAC \n");
        }
        if (token.type == TTYPE_COLUMN) {
            printf("COLUMN \n");
        }
        if (token.type == TTYPE_COMMA) {
            printf("COMMA \n");
        }
        if (token.type == TTYPE_NONE) {
            printf("NONE \n");
        }
        if (token.type == TTYPE_KEYWORD) {
            printf("KEYWORD \n");
        }
        if (token.type == TTYPE_EOF) {
            printf("EOF\n");
        }
        if (token.type == TTYPE_DOUBLE) {
            printf("NUM %f\n", token.attribute.decimal);
        }
        if (token.type == TTYPE_INT) {
            printf("NUM %ld\n", token.attribute.integer);
        }
        result = get_token(f, &token);

    }

    indent_stack_free(dent_stack);

    fclose(f);
    return 0;
}
