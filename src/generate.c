/*MOVE //⟨var⟩ ⟨symb ⟩Přiřazení hodnoty do proměnné
CREATEFRAME // vytvor docasny ramec
PUSHFRAME //Přesun dočasného rámce na zásobník rámců
POPFRAME //Presun aktualniho ramce do docasneho
DEFVAR //⟨var) Definuj novou promenou v ramci
CALL // <label> Skok na návěští s podporou návratu
RETURN //
PUSHS // <symbol>
POPS // <var> dojde k chybe 56
CLEARS //
ADD //
SUB //
MUL //
DIV //
IDIV //
ADDS/SUBS/MULS/DIVS/IDIVS

*/



#include "generate.h"
#include <stdio.h>
#include <stdlib.h>


string_t *output_code;
#define
int unic = 1;


void start_program() {
    output_code = string_create_init();
    string_append(output_code, ".IFJcode19\n");
    string_append(output_code, "JUMP %MAIN\n");


}

void genenarte_expression(token_t *operand1, char *operator, token_t *operand2, string_t *result) {
    result = string_create_init();
    create_unic_variable(result, &unic);


}


void end_program() {
    printf("%s\n\0", output_code->array);
    string_free(output_code);
}




