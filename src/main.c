#include <stdio.h>
#include "dynamic_string.h"
#include "scanner.h"

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
    
    Ttoken token;
    int result;

    while(token.type != TTYPE_EOF){
        result = get_token(f, &token);
        if(token.type == TTYPE_KEYWORD){
            printf("Keyword was found\n");
        }
        else if(token.type == TTYPE_INT){
            if(result){
                printf("ERROR\n");
            }
            else{
                printf("Found integer |%ld|\n", token.attr.integer);
            }
        }
        else if(token.type == TTYPE_DOUBLE){
            if(result){
                printf("ERROR\n");
            }
            else{
                printf("Found double |%f|\n", token.attr.decimal);
            }
        }
        else{
            if(result){
                printf("ERROR, actual token |%s|\n", token.attr.string.array);
            }
            if(!result){
                printf("SUCCES, actual token |%s|\n", token.attr.string.array);
            }
        }
     }

    fclose(f);
	return 0;
}
