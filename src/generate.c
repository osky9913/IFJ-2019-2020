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


 fucking @todo div delenie nulou , div pretypovanie na float, every sprintf rewrite you dump bitch

*/



#include "generate.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>


string_t *output_code;
string_t *function_definitions;
string_t *errors;


int unic = 1;
int unic_param = 1;


void start_program() {
    output_code = string_create_init();

    string_append(output_code, ".IFJcode19\n");
    string_append(output_code, "JUMP %MAIN\n");


}


void token_generator(token_t *operand) {


    switch (operand->type) {
        case TTYPE_ID:
            sprintf(output_code->array, "%s LF-%s", output_code->array, operand->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%s LF-%ld", output_code->array, operand->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%s LF-%lf", output_code->array, operand->attribute.decimal);
            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%s LF-%s", output_code->array, operand->attribute.string);
            break;
        default:
            sprintf(output_code->array, "%s THERE IS A PROBLEM\n", output_code->array);
    }


}


void adding_operands(string_t *result, token_t *operand1, token_t *operand2) {
    string_append(output_code, result->array);
    token_generator(operand1);
    token_generator(operand2);
    string_append(output_code, "\n");

}




string_t *genenarte_expression(token_t *operand1, token_t *operator, token_t *operand2, string_t *result) {






    result = string_create_init();
    create_unic_variable(result, &unic);
    sprintf(output_code->array, "%sDEFVAR LF-%s\n", output_code->array, result->array);


    string_t *variable1 = string_create_init();
    create_unic_variable(variable1, &unic);
    sprintf(output_code->array, "%sDEFVAR LF-%s\n", output_code->array, variable1->array);

    string_t *variable2 = string_create_init();
    create_unic_variable(variable2, &unic);
    sprintf(output_code->array, "%sDEFVAR LF-%s\n", output_code->array, variable2->array);


    string_t *result_eq_1 = string_create_init();
    create_unic_variable(result, &unic);
    sprintf(output_code->array, "%sDEFVAR LF-%s\n", output_code->array, result->array);


    string_t *result_eq_2 = string_create_init();
    create_unic_variable(result, &unic);
    sprintf(output_code->array, "%sDEFVAR LF-%s\n", output_code->array, result->array);



    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------




    switch (operand1->type) {


        case TTYPE_ID:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%s\n", output_code->array, variable1->array,
                    operand1->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%ld\n", output_code->array, variable1->array,
                    operand1->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%lf\n", output_code->array, variable1->array,
                    operand1->attribute.decimal);

            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%s\n", output_code->array, variable1->array,
                    operand1->attribute.string);
            break;
        default:
            sprintf(output_code->array, "%s THERE IS A PROBLEM\n", output_code->array);


    }


    switch (operand2->type) {
        case TTYPE_ID:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%s\n", output_code->array, variable1->array,
                    operand2->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%ld\n", output_code->array, variable1->array,
                    operand2->attribute.integer);
            break;
        case TTYPE_DOUBLE:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%lf\n", output_code->array, variable1->array,
                    operand2->attribute.decimal);

            break;
        case TTYPE_STR:
            sprintf(output_code->array, "%sTYPE LF-%s LF-%s\n", output_code->array, variable1->array,
                    operand2->attribute.string);
            break;
        default:
            sprintf(output_code->array, "%s THERE IS A PROBLEM\n", output_code->array);

    }


    sprintf(output_code->array, "%sJUMPIFNEQ @todo_lable %s %s\n", output_code->array, variable1->array,
            variable2->array);


    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------

    // vykonavanie expression
    switch (operator->type) {

        case TTYPE_LSOREQ:
            string_append(output_code, "LS ");
            adding_operands(result_eq_1, operand1, operand2);
            string_append(output_code, "EQ ");
            adding_operands(result_eq_2, operand1, operand2);
            string_append(output_code, "OR ");
            sprintf(output_code->array, "%s LF-%s LF-%s\n", output_code->array, result_eq_1->array, result_eq_2->array);

            break;

        case TTYPE_GTOREQ:
            string_append(output_code, "LS ");
            adding_operands(result_eq_1, operand1, operand2);
            string_append(output_code, "EQ ");
            adding_operands(result_eq_2, operand1, operand2);
            string_append(output_code, "OR ");
            sprintf(output_code->array, "%s LF-%s LF-%s\n", output_code->array, result_eq_1->array, result_eq_2->array);

            break;


        case TTYPE_ISEQ:
            string_append(output_code, "EQ ");
            adding_operands(result, operand1, operand2);
            break;
        case TTYPE_LS:
            string_append(output_code, "LT ");
            adding_operands(result, operand1, operand2);
            break;
        case TTYPE_GT:
            string_append(output_code, "GT ");
            adding_operands(result, operand1, operand2);
            break;

        case TTYPE_ADD:
            string_append(output_code, "ADD ");
            adding_operands(result, operand1, operand2);
            break;
        case TTYPE_SUB:
            string_append(output_code, "SUB ");
            adding_operands(result, operand1, operand2);
            break;
        case TTYPE_MUL:
            string_append(output_code, "MUL ");
            adding_operands(result, operand1, operand2);
            break;
        case TTYPE_DIV:
            string_append(output_code, "DIV ");
            adding_operands(result, operand1, operand2);
            break;
        case TTYPE_IDIV:
            string_append(output_code, "IDIV ");
            adding_operands(result, operand1, operand2);
            break;
        default:
            sprintf(output_code->array, "%s THERE IS A PROBLEM\n", output_code->array);

    }

    string_free(result_eq_1);
    string_free(result_eq_2);
    string_free(variable1);
    string_free(variable2);
    return result;

}


void generate_create_frame() {
    string_append(function_definitions, "CREATEFRAME\n");
}

void generate_function(token_t *id) {
    string_append(function_definitions, "LABEL !");
    string_append(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");
    string_append(function_definitions, "PUSHFRAME\n");

}


void generate_call_function(token_t *id) {
    string_append(output_code, "CALL !");
    string_append(output_code, id->attribute.string);
    string_append(output_code, "\n");


}


void generate_def_param(token_t *id) {
    string_append(function_definitions, "DEFVAR ");
    string_append(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");
}

void generate_call_param(token_t *id) {


}


void generate_function_end(token_t *id) {

}


void generate_function_end_no_return(token_t *id) {

}




void end_program() {
    printf("%s\n\0", output_code->array);
    string_free(output_code);
}




//symtable search -> attributs function attributy param count