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
#include "scanner.h"
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

string_t *genenarte_expression(token_t *operand1, token_t *operator, token_t *operand2, string_t *result) {











    result = string_create_init();
    create_unic_variable(result, &unic);

    sprintf(output_code->array, "%sDEFVAR %s\n", output_code->array, result->array);


    string_t *variable1 = string_create_init();
    create_unic_variable(variable1, &unic);

    sprintf(output_code->array, "%sDEFVAR %s\n", output_code->array, variable1->array);

    string_t *variable2 = string_create_init();
    create_unic_variable(variable2, &unic);

    sprintf(output_code->array, "%sDEFVAR %s\n", output_code->array, variable2->array);



    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------




    switch (operand1->type) {
        case TTYPE_ID:
            sprintf(output_code->array, "%sTYPE %s %s\n", output_code->array, variable1->array,
                    operand1->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%sTYPE %s %ld\n", output_code->array, variable1->array,
                    operand1->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%sTYPE %s %lf\n", output_code->array, variable1->array,
                    operand1->attribute.decimal);

            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%sTYPE %s %s\n", output_code->array, variable1->array,
                    operand1->attribute.string);
            break;
        default:

    }


    switch (operand2->type) {
        case TTYPE_ID:
            sprintf(output_code->array, "%sTYPE %s %s\n", output_code->array, variable1->array,
                    operand2->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%sTYPE %s %ld\n", output_code->array, variable1->array,
                    operand2->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%sTYPE %s %lf\n", output_code->array, variable1->array,
                    operand2->attribute.decimal);

            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%sTYPE %s %s\n", output_code->array, variable1->array,
                    operand2->attribute.string);
            break;
        default:

    }


    sprintf(output_code->array, "%sJUMPIFNEQ @todo_lable %s %s\n", output_code->array, variable1->array,
            variable2->array);


    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------


    // vykonavanie expression
    switch (operator->type) {
        case TTYPE_ADD:
            string_append(output_code, "ADD ");

            break;
        case TTYPE_SUB:
            string_append(output_code, "SUB ");

            break;
        case TTYPE_MUL:
            string_append(output_code, "MUL ");

            break;
        case TTYPE_DIV:
            string_append(output_code, "DIV ");

            break;
        case TTYPE_IDIV:
            string_append(output_code, "IDIV ");

            break;
        default:

    }

    string_append(output_code, result->array);


    switch (operand1->type) {
        case TTYPE_ID:
            sprintf(output_code->array, "%s %s", output_code->array, operand1->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%s %ld", output_code->array, operand1->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%s %lf", output_code->array, operand1->attribute.decimal);
            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%s %s", output_code->array, operand1->attribute.string);
            break;
        default:

    }


    switch (operand2->type) {
        case TTYPE_ID:
            sprintf(output_code->array, "%s %s\n", output_code->array, operand2->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%s %ld\n", output_code->array, operand2->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%s %lf\n", output_code->array, operand2->attribute.decimal);
            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%s %s\n", output_code->array, operand2->attribute.string);
            break;

        default:
    }


    return result;

}


void end_program() {
    printf("%s\n\0", output_code->array);
    string_free(output_code);
}




