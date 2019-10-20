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
    int result = get_token(f, &token);
    if(result){
    	printf("succes\n");
    }

    fclose(f);
	return 0;
}
