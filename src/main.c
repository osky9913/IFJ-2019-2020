#include <stdio.h>
#include "dynamic_string.h"
#include <string.h>


int main() {

    struct tString my_string;
    string_init(&my_string);


    char helloworld[1000] = "ahhoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svethoj svetoj svet";

    for (size_t i = 0; i <= strlen(helloworld); i++) {
        string_append(&my_string, helloworld[i]);

    }

    string_print(&my_string);
    string_free(&my_string);

    return 0;
}