#include <stdio.h>
#include "dynamic_string.h"
#include <string.h>


int main() {



    string_t *string = string_create_init();
    if (!string) {
        printf("Nepovedla se alokace\n");
    }


    char helloworld[1000] = "ahhoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svetoj svet";

    for (size_t i = 0; i <= strlen(helloworld); i++) {
        string_append_char(string, helloworld[i]);
    }

    string_print(string, "\n");
    string_free(string);

    return 0;
}
