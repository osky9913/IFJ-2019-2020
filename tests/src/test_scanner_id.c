
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
        if (token.type == TTYPE_ID) {
            printf("ID ");
            free(token.attribute.string);

        }               // ID
        if (token.type == TTYPE_KEYWORD) {
            printf("KEYWORD ");

        }          // def/else/if,None,pass,return,while
        if (token.type == TTYPE_EOF) {
            printf("EOF ");

        }              // end of file
        if (token.type == TTYPE_EOL) {
            printf("EOL\n");
        }              // \n

        if (token.type == TTYPE_INT) {
            printf("INT ");
        }              //whole number
        if (token.type == TTYPE_DOUBLE) {
            printf("DOUBLE ");
        }           //desatinne čislo
        if (token.type == TTYPE_STR) {
            printf("STR ");
            free(token.attribute.string);

        }              //string
        if (token.type == TTYPE_DOCSTR) {
            printf("DOCSTR ");
            free(token.attribute.string);

        }           //documentation string
        if (token.type == TTYPE_NONE) {
            printf("NONE ");
        }             //None

        if (token.type == TTYPE_INDENT) {
            printf("INDENT ");
        }
        if (token.type == TTYPE_DEDENT) {
            printf("DEDENT ");
        }

        if (token.type == TTYPE_COMMA) {
            printf("COMMA ");
        }            // ','
        if (token.type == TTYPE_COLUMN) {
            printf("COLUMN ");
        }           // ':'
        if (token.type == TTYPE_LTBRAC) {
            printf("LTBRAC ");
        }           // '('
        if (token.type == TTYPE_RTBRAC) {
            printf("RTBRAC ");
        }           // ')'
        if (token.type == TTYPE_ASSIGN) {
            printf("ASSIGN ");
        }           // '='
        if (token.type == TTYPE_EXCL) {
            printf("EXCL ");
        }             // '!'

        if (token.type == TTYPE_ISEQ) {
            printf("ISEQ ");
        }             // '=='
        if (token.type == TTYPE_ISNEQ) {
            printf("ISNEQ ");
        }            // '!='
        if (token.type == TTYPE_GTOREQ) {
            printf("GTOREQ ");
        }           // '>='
        if (token.type == TTYPE_LSOREQ) {
            printf("LSOREQ ");
        }           // '<='
        if (token.type == TTYPE_GT) {
            printf("GT ");
        }               // '>'
        if (token.type == TTYPE_LS) {
            printf("LS ");
        }               // '<'
        if (token.type == TTYPE_ADD) {
            printf("ADD ");
        }              // '+' addition or concatenation
        if (token.type == TTYPE_SUB) {
            printf("SUB ");
        }              // '-'
        if (token.type == TTYPE_MUL) {
            printf("MUL ");
        }              // '*'
        if (token.type == TTYPE_DIV) {
            printf("DIV ");
        }              // '/'
        if (token.type == TTYPE_IDIV) {
            printf("IDIV ");
        }             // '//'
        result = get_token(f, &token);

    }

    indent_stack_free(dent_stack);

    fclose(f);
    return 0;
}