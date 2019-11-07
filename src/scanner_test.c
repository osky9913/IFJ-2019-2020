
#include "dynamic_string.h"
#include "indent_stack.h"
#include "scanner.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    FILE *f;
    if (argc == 1)
    {
        printf("Neni zadan vstupni soubor\n");
        return 0;
    }
    if ((f = fopen(argv[1], "r")) == NULL)
    {
        printf("Soubor se nepodarilo otevrit\n");
        return 0;
    }

    token_t token;
    int result;
    dent_stack = indent_stack_init();

    while(token.type != TTYPE_EOF){
        result = get_token(f, &token);
        if(token.type == TTYPE_EOL){
            printf("NEWLINE\n");
        }
        else if(token.type == TTYPE_STR || token.type == TTYPE_DOCSTR || token.type == TTYPE_ID){
            printf("STRING or ID found |%s|\n", token.attribute.string);
        }
        else if(token.type == TTYPE_INDENT){
            printf("INDEEENT \n");
        }
        else if(token.type == TTYPE_EOF){
            printf("EOF\n");
        }
        else if(token.type == TTYPE_DEDENT){
            if(result){
                printf("Dedent error!\n");
            }
            else{
                printf("DEDEEENT \n");
            }
        }
        else if(result){
                printf("ERROR, actual token\n");
        }
        else if(!result){
                printf("SUCCES, actual token\n");
        }
        
    }
    indent_stack_free(dent_stack);

    fclose(f);
    return 0;
}
