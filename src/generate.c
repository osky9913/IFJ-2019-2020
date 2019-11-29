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


 fucking todo   div delenie nulou?? , div pretypovanie na float, fokin labels, fokin string uniq array disappearing, fokin add embedded functions,
                fokin print is not done at all, fok maj lajf, about to kms brb

*/
#include "generate.h"
#include "scanner.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

string_t *output_code;
string_t *function_definitions;
string_t *errors;

int uniq = 1;
int uniq_param_call = 1;
int uniq_param_def = 1;
int uniq_expression = 1;

void start_program() {
    output_code = string_create_init();
    function_definitions = string_create_init();
    errors = string_create_init();

    string_append(function_definitions, ".IFJcode19\n");
    string_append(function_definitions, "JUMP %MAIN\n");
    string_append(function_definitions, "\n#>---------------------------FUNCTIONS---------------------------<\n");
    string_append(output_code, "\n#>---------------------------FUNCTIONS---------------------------<\n");
    string_append(output_code, "\n#>---------------------------MAIN---------------------------<\n");
    string_append(output_code, "\nLABEL %MAIN\n");
    string_append(errors, "\n#>---------------------------MAIN---------------------------<\n");
}


void printing_frame_to_variable(string_t *frame) {
    if (in_function) {
        string_append(frame, "LF@");
    } else {
        string_append(frame, "GF@");
    }
}

void printing_token_to_frame(string_t *frame, token_t *operand) {

    char temp[70] = {0}; // el simon 70

    switch (operand->type) {
        case TTYPE_ID:
            printing_frame_to_variable(frame);
            string_append(frame, operand->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(temp, "int@%ld", operand->attribute.integer);
            string_append(frame, temp);
            break;
        case TTYPE_DOUBLE:
            sprintf(temp, "float@0x%lfp+0", operand->attribute.decimal);
            string_append(frame, temp);
            break;
        case TTYPE_STR:
            string_append(frame, "string@");
            string_append(frame, operand->attribute.string);
            break;
        case TTYPE_NONE:
            string_append(frame, "string@nil");
            break;
        default:
            printf("There is a problem here, token passed to generator was neither of ID/INT/DOUBLE/STR/NONE\n");
    }
}


void adding_operands(string_t *frame, string_t *result, token_t *operand1, token_t *operand2) {
    printing_frame_to_variable(frame);
    string_append(frame, result->array);
    string_append(frame, " ");
    printing_token_to_frame(frame, operand1);
    string_append(frame, " ");
    printing_token_to_frame(frame, operand2);
    string_append(frame, "\n");

}


void adding_operands_string(string_t *frame, string_t *result, string_t *operand1, string_t *operand2) {
    print_variable_from_string(frame, result->array);
    print_variable_from_string(frame, operand1->array);
    print_variable_from_string(frame, operand2->array);
    string_append(frame, "\n");

  /*  printing_token_to_frame(frame, operand1);
    string_append(frame, result->array);
    string_append(frame, " ");
    printing_token_to_frame(frame, operand1);
    string_append(frame, operand1->array);
    string_append(frame, " ");
    printing_token_to_frame(frame, operand1);
    string_append(frame, operand2->array);
    string_append(frame, "\n");*/

}

string_t* define_uniq_variable(string_t *output_string, int *uniq_temp, char *name) {
    string_t* variable = string_create_init();
    if (name) {
        create_unic_variable(variable, uniq_temp, name);
    }
    else {
        create_unic_variable(variable, uniq_temp, "var");
    }
    string_append(output_string, "DEFVAR ");
    printing_frame_to_variable(output_string);
    string_append(output_string, variable->array);
    string_append(output_string, "\n");
    return variable;
}

void get_type_variable(string_t *id_type, string_t *output_string, token_t *operand) {
    string_append(output_string, "TYPE ");
    printing_frame_to_variable(output_string);
    string_append(output_string, id_type->array);
    string_append(output_string, " ");
    printing_token_to_frame(output_string, operand);
    string_append(output_string, "\n");
}

void print_variable_from_string(string_t *frame, const char *variable) {
    printing_frame_to_variable(frame);
    string_append(frame, variable);
    string_append(frame, " ");

}

void check_if_op_type_eq(string_t *frame, char *variable_type, char *type, char *label) {
    // @todo error label
    string_append(frame, "JUMPIFNEQ ");
    string_append(frame, label);
    print_variable_from_string(frame, variable_type);
    string_append(frame, type);
    string_append(frame, "\n");

}
/*
void convert_float_to_int(string_t* frame, token_t* operand){
    string_append(switching_output, "FLOAT2INT ");
    printing_token_to_frame(switching_output, operand);
    printing_token_to_frame(switching_output, operand);
}*/

void generate_error_labels(){
    string_append(errors,"LABEL !neq_operands_error\n");
    string_append(errors,"DPRINT\032string@Operands\032are\032not\032thez\032same\032type!");

}

//rTODO result vracat z funkcie, vymazat ho z parametrov
char *generate_expression(token_t *operand1, token_t *operator, token_t *operand2) {
    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "\n#evaluating expression\n");
    string_t *result = define_uniq_variable(switching_output, &uniq, "result");
    //string which will store result of current operation (DEFVAR result)

    //string which will store first operand (DEFVAR operand1)
    string_t *variable1 = define_uniq_variable(switching_output, &uniq, "var");

    //problem is here variable->array is empty after returning from define_uniq_variable


    //string which will store second operand (DEFVAR operand1)
    string_t *variable2 = define_uniq_variable(switching_output, &uniq, "var");

    //string used as temporary storing place in operator >= or <=
    string_t *result_eq_1 = define_uniq_variable(switching_output, &uniq,"tmp_result");

    //string used as temporary storing place in operator >= or <=
    string_t *result_eq_2 = define_uniq_variable(switching_output, &uniq, "tmp_result");

    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------

    // type of first operand
    get_type_variable(variable1, switching_output, operand1);

    // type of second operand
    get_type_variable(variable2, switching_output, operand2);

    // types of operands are not equal -> error
    string_append(switching_output, "JUMPIFNEQ @todo_lable ");
    print_variable_from_string(switching_output, variable1->array);
    print_variable_from_string(switching_output, variable2->array);
    string_append(switching_output, "\n");
    string_append(switching_output, "JUMPIFEQ @todo_error_lable ");
    print_variable_from_string(switching_output, variable1->array);
    string_append(switching_output, " string@nil");
    string_append(switching_output, "\n");
    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------

    // vykonavanie expression
    switch (operator->type) {

        case TTYPE_LSOREQ:
            //check if operand1 < operand2
            string_append(switching_output, "LS ");
            adding_operands(switching_output, result_eq_1, operand1, operand2);
            //check if operand1 == operand2
            string_append(switching_output, "EQ ");
            adding_operands(switching_output, result_eq_2, operand1, operand2);
            //(op1 < op2) or (op1 == op2) => operand1 <= operand2
            string_append(switching_output, "OR ");
            adding_operands_string(switching_output, result, result_eq_1, result_eq_2);

            break;
        case TTYPE_GTOREQ:
            //check if operand1 < operand2
            string_append(switching_output, "GT ");
            adding_operands(switching_output, result_eq_1, operand1, operand2);
            //check if operand1 == operand2
            string_append(switching_output, "EQ ");
            adding_operands(switching_output, result_eq_2, operand1, operand2);
            //(op1 < op2) or (op1 == op2) => operand1 <= operand2
            string_append(switching_output, "OR ");
            adding_operands_string(switching_output, result, result_eq_1, result_eq_2);
            break;
        case TTYPE_ISEQ:
            string_append(switching_output, "EQ ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_LS:
            string_append(switching_output, "LT ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_GT:
            string_append(switching_output, "GT ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_ADD:
            string_append(switching_output, "JUMPIFEQ #concatenation ");
            print_variable_from_string(switching_output, variable1->array);
            string_append(switching_output, "string@string\n");
            string_append(switching_output, "ADD ");
            adding_operands(switching_output, result, operand1, operand2);
            string_append(switching_output, "JUMP @todo_expression_end_label\n");
            string_append(switching_output, "LABEL #concatenation\n");
            string_append(switching_output, "CONCAT ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_SUB:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "@todo_error_label ");
            string_append(switching_output, "SUB ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_MUL:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "@todo_error_label ");
            string_append(switching_output, "MUL ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_DIV:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "@todo_error_label ");
            string_append(switching_output, "DIV ");
            adding_operands(switching_output, result, operand1, operand2);


            break;
        case TTYPE_IDIV:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "@todo_error_label ");
            string_append(switching_output, "JUMPIFEQ #int_division "); // kontrola pri idive  ci su int
            print_variable_from_string(switching_output, variable1->array);
            string_append(switching_output, " string@int\n");

            /*  DELENIE NULOV ZABIJA INTERPRET -> OSETROVAT?
             *
            string_append(switching_output, "JUMPIFEQ #addition "); // kontrola pri idive  ci  je to rovne nula
            printing_token_to_frame(switching_output, operand2);
            string_append(switching_output, " int@0\n");
            */

            //nebol to int -> pretypovanie na int
            //FLOAT2INT operand1 operand1
            string_append(switching_output, "FLOAT2INT ");
            printing_token_to_frame(switching_output, operand1);
            printing_token_to_frame(switching_output, operand1);
            //FLOAT2INT operand2 operand2
            string_append(switching_output, "FLOAT2INT ");
            printing_token_to_frame(switching_output, operand2);
            printing_token_to_frame(switching_output, operand2);

            string_append(switching_output, "LABEL #int_division\n");
            string_append(switching_output, "IDIV ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        default:
            sprintf(output_code->array, "%s THERE IS A PROBLEM\n", output_code->array);

    }
    char *final_result = string_copy_data(result);
    string_free(result);
    string_free(result_eq_1);
    string_free(result_eq_2);
    string_free(variable1);
    string_free(variable2);
    return final_result;
}

void generate_create_frame() {
    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "CREATEFRAME\n");
    uniq_param_call = 1;
}

void generate_function(token_t *id) {
    string_append(function_definitions, "\n\nLABEL !");
    string_append(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");
    string_append(function_definitions, "PUSHFRAME\n");
    string_append(function_definitions, "DEFVAR LF@%%return_value\n");
}

void generate_print(const char* label){
    //toto prerobit na generate_function a generate_call_function, tak aby mi do tych funkcii posielali iba string nie token
    string_append(function_definitions, "\nLABEL !");
    string_append(function_definitions, label);
    string_append(function_definitions, "\n");
    string_append(function_definitions, "PUSHFRAME\n");
    string_append(function_definitions, "DEFVAR LF@%%return_value\n");
    for(int i = uniq_param_def; i <= uniq_param_call; i++){
        string_append(function_definitions, "DEFVAR LF@");
        string_t *print_variable = string_create_init();
        create_unic_variable(print_variable, &uniq, "%print_var");
        string_append(function_definitions, print_variable->array);
        string_append(function_definitions, " ");
        string_append(function_definitions, "\n");

        string_t *parameter = string_create_init();
        create_unic_variable(parameter, &uniq_param_def, "%param");
        string_append(function_definitions, "MOVE LF@");
        string_append(function_definitions, print_variable->array);
        string_append(function_definitions, " LF@");
        string_append(function_definitions, parameter->array);
        string_append(function_definitions, "\n");

        string_append(function_definitions, "WRITE ");
        string_append(function_definitions, " LF@");
        string_append(function_definitions, print_variable->array);
        string_append(function_definitions, "\n");
    }
    string_append(function_definitions,"MOVE LF@%%return_value nil@nil\n");
    generate_function_end();
    //uvolnit
}

void generate_call_function(const char* id) {
    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    if((strcmp(id, "print")) == 0){
        string_t *print_label = string_create_init();
        create_unic_variable(print_label, &uniq, "%print");
        string_append(switching_output, "CALL !");
        string_append(switching_output, print_label->array);
        string_append(switching_output, "\n");
        generate_print(print_label->array);
    }
    else{
        string_append(switching_output, "CALL !");
        string_append(switching_output, id);
        string_append(switching_output, "\n");
    }
}


void generate_def_param(token_t *id) {
    string_append(function_definitions, "DEFVAR ");
    printing_frame_to_variable(function_definitions);
    print_variable_from_string(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");

    string_t *parameter = string_create_init();
    create_unic_variable(parameter, &uniq_param_def, "%param");
    string_append(parameter, "\n");
    string_append(function_definitions, "MOVE ");
    print_variable_from_string(function_definitions, id->attribute.string);
    print_variable_from_string(function_definitions, parameter->array);
    // todo uvolnit
}

void generate_call_param(token_t *id) {
    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_t *parameter = string_create_init();
    create_unic_variable(parameter, &uniq_param_call, "%param");

    string_append(switching_output, "DEFVAR TF@");
    string_append(switching_output, parameter->array);
    string_append(switching_output, "\n");
    string_append(switching_output, "MOVE TF@");
    string_append(switching_output, parameter->array);
    string_append(switching_output, " ");
    printing_token_to_frame(switching_output, id);
    string_append(switching_output, "\n");

    // todo uvolnit
}

void generate_function_end() {
    string_append(function_definitions, "POPFRAME \n");
    string_append(function_definitions, "RETURN \n");
    uniq_param_def = 1;
}

void generate_while_lable() {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }

    string_append(switching_output, "LABEL @todo_while_beginning_label ");
    string_append(switching_output, "\n");
}

void generate_while(token_t *expression) {

    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }

    string_t *while_expression = define_uniq_variable(switching_output, &uniq_expression, "%while_expression");

    if (expression->type == TTYPE_ID) {

        string_t *id_type = define_uniq_variable(switching_output, &uniq_expression, "%while_check_type");

        //TYPE id_type expression
        get_type_variable(id_type, switching_output, expression);

        //JUMPIFEQ int_label id_type string@int
        string_append(switching_output, "JUMPIFEQ @todo_while_int_label ");
        string_append(switching_output, "\n");

        printing_frame_to_variable(switching_output);
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@int\n");

        //JUMPIFEQ float_label id_type string@float
        string_append(switching_output, "JUMPIFEQ @todo_while_float_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@float\n");

        //JUMPIFEQ bool_label id_type string@bool
        string_append(switching_output, "JUMPIFEQ @todo_while_bool_label ");
        string_append(switching_output, "\n");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@bool\n");

        //while (string) or while (None) -> while end
        string_append(switching_output, "JUMP @todo_while_end_label");
        string_append(switching_output, "\n");

        //id_type was bool, now we compare if expression is false
        string_append(switching_output, "LABEL @todo_while_bool_label ");
        string_append(switching_output, "\n");
        //JUMPIFEQ @todo_while_end_label expression bool@false
        string_append(switching_output, "JUMPIFEQ @todo_while_end_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "bool@false\n");
        string_append(switching_output, "JUMP @todo_while_body_label ");
        string_append(switching_output, "\n");
    }
    if (expression->type == TTYPE_INT) {
        //id_type was int, now we compare if expression is zero
        string_append(switching_output, "LABEL @todo_while_int_label ");
        string_append(switching_output, "\n");

        //JUMPIFEQ @todo_while_end_label expression int@0
        string_append(switching_output, "JUMPIFEQ @todo_while_end_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "int@0");
        string_append(switching_output, "\n");
        string_append(switching_output, "JUMP @todo_while_body_label ");
        string_append(switching_output, "\n");
    }
    if (expression->type == TTYPE_DOUBLE) {
        //id_type was float, now we compare if expression is zero
        string_append(switching_output, "LABEL @todo_while_float_label ");

        //JUMPIFEQ @todo_while_end_label expression float@0x0.0p+0
        string_append(switching_output, "JUMPIFEQ @todo_while_end_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "float@0x0.0p+0");
        string_append(switching_output, "\n");

        string_append(switching_output, "JUMP @todo_while_body_label ");
        string_append(switching_output, "\n");

    }
    string_append(switching_output, "LABEL @todo_while_body_label ");
    string_append(switching_output, "\n");


}

void generate_while_end() {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "LABEL @todo_while_end_label ");
    //uniq for while labels will be set to one
}

void generate_if(token_t *expression) {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    //DEFVAR while_expression
    string_t *while_expression = define_uniq_variable( switching_output, &uniq_expression, "%while_expression");

    if (expression->type == TTYPE_ID) {

        string_t *id_type = define_uniq_variable(switching_output, &uniq_expression, "%if_check_type");

        //TYPE id_type expression
        get_type_variable(id_type, switching_output, expression);

        //JUMPIFEQ int_label id_type string@int
        string_append(switching_output, "JUMPIFEQ @todo_if_int_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@int\n");

        //JUMPIFEQ float_label id_type string@float
        string_append(switching_output, "JUMPIFEQ @todo_if_float_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@float\n");

        //JUMPIFEQ bool_label id_type string@bool
        string_append(switching_output, "JUMPIFEQ @todo_if_bool_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, " ");
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@bool\n");

        //if (string) or if (None) -> else
        string_append(switching_output, "JUMP @todo_else_label \n");

        //id_type was bool, now we compare if expression is false
        string_append(switching_output, "LABEL @todo_if_bool_label ");
        string_append(switching_output, "\n");
        //JUMPIFEQ @todo_while_end_label expression bool@false
        string_append(switching_output, "JUMPIFEQ @todo_else_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "bool@false\n");
        string_append(switching_output, "JUMP @todo_if_label ");
        string_append(switching_output, "\n");
    }
    if (expression->type == TTYPE_INT) {
        //id_type was int, now we compare if expression is zero
        string_append(switching_output, "LABEL @todo_if_int_label ");
        string_append(switching_output, "\n");
        //JUMPIFEQ @todo_while_end_label expression int@0
        string_append(switching_output, "JUMPIFEQ @todo_else_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "int@0");
        string_append(switching_output, "\n");
        string_append(switching_output, "JUMP @todo_if_label ");
        string_append(switching_output, "\n");
    }
    if (expression->type == TTYPE_DOUBLE) {
        //id_type was float, now we compare if expression is zero
        string_append(switching_output, "LABEL @todo_if_float_label ");
        string_append(switching_output, "\n");
        //JUMPIFEQ @todo_while_end_label expression float@0x0.0p+0
        string_append(switching_output, "JUMPIFEQ @todo_else_label ");
        string_append(switching_output, "\n");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, "\n");
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "float@0x0.0p+0");
        string_append(switching_output, "\n");
        string_append(switching_output, "JUMP @todo_if_label ");
        string_append(switching_output, "\n");
    }

    string_append(switching_output, "LABEL @todo_if_label");
    string_append(switching_output, "\n");
}

void generate_else() {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "LABEL @todo_else_label");
    string_append(switching_output, "\n");

    //uniq for if labels will be set to one
}

void generate_elseif_end(){
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "LABEL @todo_elseif_end_label");
    string_append(switching_output, "\n");
}

void generate_assign_retvalue(const char *dest){
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "MOVE TF@%%return_value ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, dest);
    string_append(switching_output, "\n");
}

void generate_assign(const char *destination, token_t *content) {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }

    string_append(switching_output, "MOVE ");
    print_variable_from_string(switching_output, destination);
    string_append(switching_output, " ");
    printing_token_to_frame(switching_output, content);
    string_append(switching_output, "\n");
}

void declaration_variable(token_t *variable) {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }

    string_append(switching_output, "DEFVAR ");
    printing_token_to_frame(switching_output, variable);
    string_append(switching_output, "\n");
}

void free_finals_string() {
    string_free_array(output_code);
    string_free_array(errors);
    string_free_array(function_definitions);

}


void end_program() {
    string_append(function_definitions, output_code->array);
    string_append(function_definitions, errors->array);
    printf("%s", function_definitions->array);

}


//symtable search -> attributs function attributy param count
