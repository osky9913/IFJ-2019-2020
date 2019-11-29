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
int uniq_label = 1;
int uniq_while_label = 1;
int uniq_if_label = 1;

void start_program() {
    output_code = string_create_init();
    function_definitions = string_create_init();
    errors = string_create_init();

    string_append(errors, "EXIT int@0\n");
    string_append(errors, "LABEL %error_label_semantic\n");
    string_append(errors, "DPRINT string@RUNTIME_SEMANTIC_ERROR\n");
    string_append(errors, "EXIT int@4\n");

    string_append(errors, "LABEL %error_label_0\n");
    string_append(errors, "DPRINT string@DIVISION_BY_ZERO\n");
    string_append(errors, "EXIT int@4\n");



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



//rTODO result vracat z funkcie, vymazat ho z parametrov
char *generate_expression(token_t *operand2, token_t *operator, token_t *operand1) {
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
    string_append(switching_output, "JUMPIFNEQ %error_label_semantic ");
    print_variable_from_string(switching_output, variable1->array);
    print_variable_from_string(switching_output, variable2->array);
    string_append(switching_output, "\n");
    string_append(switching_output, "JUMPIFEQ %error_label_semantic ");
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
            string_append(switching_output, "JUMPIFEQ %concatenation ");
            print_variable_from_string(switching_output, variable1->array);
            string_append(switching_output, "string@string\n");
            string_append(switching_output, "ADD ");
            adding_operands(switching_output, result, operand1, operand2);
            string_append(switching_output, "JUMP %todo_expression_end_label\n");
            string_append(switching_output, "LABEL %concatenation\n");
            string_append(switching_output, "CONCAT ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_SUB:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic ");
            string_append(switching_output, "SUB ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_MUL:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic ");
            string_append(switching_output, "MUL ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        case TTYPE_DIV:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic ");
            string_append(switching_output, "DIV ");
            adding_operands(switching_output, result, operand1, operand2);


            break;
        case TTYPE_IDIV:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic ");
            string_append(switching_output, "JUMPIFEQ %int_division "); // kontrola pri idive  ci su int
            print_variable_from_string(switching_output, variable1->array);
            string_append(switching_output, " string@int\n");

            /*  DELENIE NULOV ZABIJA INTERPRET -> OSETROVAT?
             *
            string_append(switching_output, "JUMPIFEQ %addition "); // kontrola pri idive  ci  je to rovne nula
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

            string_append(switching_output, "LABEL %int_division\n");
            string_append(switching_output, "IDIV ");
            adding_operands(switching_output, result, operand1, operand2);
            break;
        default:
            printf("%s THERE IS A PROBLEM\n", output_code->array);

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
    //label for potentional next parameters after None
    string_t* process_next_param = string_create_init();
    //variable to check type
    string_t *check_variable = string_create_init();
    //parameter obtained from function call
    string_t *parameter = string_create_init();
    //label to jump to print None
    string_t *none_label = string_create_init();

    //cycle that prints parametres all parametres of print function
    for(int i = uniq_param_def; i < uniq_param_call; i++){
        //we declare variable which we will use for storing type of passed variable
        create_unic_variable(check_variable, &uniq, "%check_type");
        string_append(function_definitions, "DEFVAR LF@");
        string_append(function_definitions, check_variable->array);
        string_append(function_definitions, "\n");

        //checking type from passed parameter, generating params according to called params
        create_unic_variable(parameter, &uniq_param_def, "%param");
        string_append(function_definitions, "TYPE LF@");
        string_append(function_definitions, check_variable->array);
        string_append(function_definitions, " LF@");
        string_append(function_definitions, parameter->array);
        string_append(function_definitions, "\n");

        //jumps when passed parameter is nil -> NONE_LABEL
        create_unic_variable(none_label, &uniq_label, "%none_label");
        string_append(function_definitions, "JUMPIFEQ ");
        string_append(function_definitions, none_label->array);
        string_append(function_definitions, " LF@");
        string_append(function_definitions,  check_variable->array);
        string_append(function_definitions, " string@nil\n");

        //parameter wasnt nil -> we can print it
        string_append(function_definitions, "WRITE ");
        string_append(function_definitions, " LF@");
        string_append(function_definitions, parameter->array);
        string_append(function_definitions, "\n");

        //we have to jump over the print of None
        create_unic_variable(process_next_param, &uniq_label, "%next_param");
        string_append(function_definitions, "JUMP ");
        string_append(function_definitions, process_next_param->array);
        string_append(function_definitions, "\n");

        //paramter was None -> jumped here
        string_append(function_definitions, "LABEL ");
        string_append(function_definitions, none_label->array);
        string_append(function_definitions, "\n");

        //print None
        string_append(function_definitions, "WRITE string@None\n");

        //end processing of first parameter
        string_append(function_definitions, "LABEL ");
        string_append(function_definitions, process_next_param->array);
        string_append(function_definitions, "\n");
        string_append(function_definitions, "WRITE string@\\032\n");
    }
    //printing \n after printing all parameters
    string_append(function_definitions, "WRITE string@\\010\n");
    //print always returns nil
    string_append(function_definitions,"MOVE LF@%%return_value nil@nil\n");
    generate_function_end();

    string_free(process_next_param);
    string_free(check_variable);
    string_free(parameter);
    string_free(none_label);
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
        string_free(print_label);
    }
    else{
        string_append(switching_output, "CALL !");
        string_append(switching_output, id);
        string_append(switching_output, "\n");
    }
}


void generate_def_param(token_t *id) {
    string_append(function_definitions, "DEFVAR ");
    //printing_frame_to_variable(function_definitions); -> redundant LF@
    print_variable_from_string(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");

    string_t *parameter = string_create_init();
    create_unic_variable(parameter, &uniq_param_def, "%param");
    string_append(parameter, "\n");
    string_append(function_definitions, "MOVE ");
    print_variable_from_string(function_definitions, id->attribute.string);
    print_variable_from_string(function_definitions, parameter->array);
    // todo uvolnit
    string_free(parameter);
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
    string_free(parameter);
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
    string_t *while_beginning_label = string_create_init();
    create_unic_label(while_beginning_label, &uniq_while_label, "%while_beginning_label");
    string_append(switching_output, "\n#while cycle\n");
    string_append(switching_output, "LABEL ");
    string_append(switching_output, while_beginning_label->array);
    string_append(switching_output, "\n");
    //free string
    string_free(while_beginning_label);
}

void generate_while(token_t *expression) {

    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }

    string_t *while_expression = define_uniq_variable(switching_output, &uniq_expression, "%while_expression");

    string_append(switching_output, "MOVE ");
    print_variable_from_string(switching_output, while_expression->array);
    string_append(switching_output, " ");
    printing_token_to_frame(switching_output, expression);
    string_append(switching_output, "\n");

    string_t *while_int_label = string_create_init();
    string_t *while_float_label = string_create_init();
    string_t *while_bool_label = string_create_init();
    string_t *while_body_label = string_create_init();
    string_t *while_end_label = string_create_init();

    //uniq label for int comparison
    create_unic_label(while_int_label, &uniq_while_label, "%while_int_label");
    //uniq label for float comparison
    create_unic_label(while_float_label, &uniq_while_label, "%while_float_label");
    //uniq label for bool comparison
    create_unic_label(while_bool_label, &uniq_while_label, "%while_bool_label");
    //uniq label for jumping to the body of while cycle
    create_unic_label(while_body_label, &uniq_while_label, "%while_body_label");
    //uniq label for jumping at the end of the cycle
    create_unic_label(while_end_label, &uniq_while_label, "%while_end_label");

    //variable which will store type of passed expression
    string_t *id_type = define_uniq_variable(switching_output, &uniq_expression, "%while_check_type");

    //get type of passed expression and store the type in string id_type
    get_type_variable(id_type, switching_output, expression);

    string_append(switching_output, "\n#comparing if while expression is false/zero\n");
    //if its int jump to int comparison
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_int_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, id_type->array);
    string_append(switching_output, " string@int\n");

    //if its float jump to float comparison
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_float_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, id_type->array);
    string_append(switching_output, " string@float\n");

    //JUMPIFEQ bool_label id_type string@bool
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_bool_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, id_type->array);
    string_append(switching_output, " string@bool\n");

    //while (string) or while (None) -> while end
    string_append(switching_output, "JUMP ");
    string_append(switching_output, while_end_label->array);
    string_append(switching_output, "\n");

    //id_type was bool, now we compare if expression is false
    string_append(switching_output, "LABEL ");
    string_append(switching_output, while_bool_label->array);
    string_append(switching_output, "\n");

    //JUMPIFEQ @todo_while_end_label expression bool@false
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_end_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, while_expression->array);
    string_append(switching_output, " bool@false\n");
    string_append(switching_output, "JUMP ");
    string_append(switching_output, while_body_label->array);
    string_append(switching_output, "\n");

    //id_type was int, now we compare if expression is zero
    string_append(switching_output, "LABEL ");
    string_append(switching_output, while_int_label->array);
    string_append(switching_output, "\n");

    //JUMPIFEQ @todo_while_end_label expression int@0
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_end_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, while_expression->array);
    string_append(switching_output, " int@0\n");
    string_append(switching_output, "JUMP ");
    string_append(switching_output, while_body_label->array);
    string_append(switching_output, "\n");

    //id_type was float, now we compare if expression is zero
    string_append(switching_output, "LABEL ");
    string_append(switching_output, while_float_label->array);
    string_append(switching_output, "\n");

    //JUMPIFEQ @todo_while_end_label expression float@0x0.0p+0
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_end_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, while_expression->array);
    string_append(switching_output, " float@0x0.0p+0");
    string_append(switching_output, "\n");

    string_append(switching_output, "\n#body of while\n");
    string_append(switching_output, "LABEL ");
    string_append(switching_output, while_body_label->array);
    string_append(switching_output, "\n");

    string_free(id_type);
    string_free(while_expression);
    string_free(while_int_label);
    string_free(while_float_label);
    string_free(while_bool_label);
    string_free(while_body_label);
    string_free(while_end_label);
}

void generate_while_end() {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_t *while_end_label = string_create_init();
    string_t *while_beginning_label = string_create_init();
    //while end label and beginning label has to be the same as in generate_while
    create_unic_label(while_end_label, &uniq_while_label, "%while_end_label");
    create_unic_label(while_beginning_label, &uniq_while_label, "%while_beginning_label");

    string_append(switching_output, "JUMP ");
    string_append(switching_output, while_beginning_label->array);
    string_append(switching_output, "\n");

    string_append(switching_output, "\n#end of while cycle\n");
    string_append(switching_output, "LABEL ");
    string_append(switching_output, while_end_label->array);
    string_append(switching_output, "\n");
    //increase the label uniq variable, so that in next while are going to be different labels
    uniq_while_label += 1;

    string_free(while_end_label);
    string_free(while_beginning_label);
}

void generate_if(token_t *expression) {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    //DEFVAR if_expression
    string_t *if_expression = define_uniq_variable( switching_output, &uniq_expression, "%if_expression");

    //uniq label for int comparison
    string_t *if_int_label = string_create_init();
    //uniq label for float comparison
    string_t *if_float_label = string_create_init();
    //uniq label for bool comparison
    string_t *if_bool_label = string_create_init();
    //uniq label used to jump to else body
    string_t *else_body_label = string_create_init();
    //uniq label used to jump to if body
    string_t *if_body_label = string_create_init();

    string_append(switching_output, "\n#if else statement\n");
    //uniq label for int comparison
    create_unic_label(if_int_label, &uniq_if_label, "%if_int_label");
    //uniq label for float comparison
    create_unic_label(if_float_label, &uniq_if_label, "%if_float_label");
    //uniq label for bool comparison
    create_unic_label(if_bool_label, &uniq_if_label, "%if_bool_label");
    //uniq label for jumping to the body of while cycle
    create_unic_label(else_body_label, &uniq_if_label, "%else_body_label");
    //uniq label for jumping at the end of the cycle
    create_unic_label(if_body_label, &uniq_if_label, "%if_body_label");

    string_append(switching_output, "MOVE ");
    print_variable_from_string(switching_output, if_expression->array);
    string_append(switching_output, " ");
    printing_token_to_frame(switching_output, expression);
    string_append(switching_output, "\n");

    string_t *id_type = define_uniq_variable(switching_output, &uniq_expression, "%if_check_type");
    //get type of passed parameter
    get_type_variable(id_type, switching_output, expression);


    string_append(switching_output, "\n#checking if expression in if statement is false\n");
    //JUMPIFEQ int_label id_type string@int
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, if_int_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, id_type->array);
    string_append(switching_output, " string@int\n");

    //JUMPIFEQ float_label id_type string@float
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, if_float_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, id_type->array);
    string_append(switching_output, " string@float\n");

    //JUMPIFEQ bool_label id_type string@bool
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, if_bool_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, id_type->array);
    string_append(switching_output, " string@bool\n");

    //if (string) or if (None) -> jump to else
    string_append(switching_output, "JUMP ");
    string_append(switching_output, else_body_label->array);

    //id_type was bool, now we compare if expression is false
    string_append(switching_output, "\nLABEL ");
    string_append(switching_output, if_bool_label->array);
    string_append(switching_output, "\n");
    //JUMPIFEQ @todo_while_end_label expression bool@false
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, else_body_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, if_expression->array);
    string_append(switching_output, " bool@false\n");
    string_append(switching_output, "JUMP ");
    string_append(switching_output, if_body_label->array);
    string_append(switching_output, " \n");

    //id_type was int, now we compare if expression is zero
    string_append(switching_output, "LABEL ");
    string_append(switching_output, if_int_label->array);
    string_append(switching_output, "\n");
    //if expression is equal to zero jump to else body
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, else_body_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, if_expression->array);
    string_append(switching_output, "int@0");
    string_append(switching_output, "\n");
    //expression wasnt zero jump to if body
    string_append(switching_output, "JUMP ");
    string_append(switching_output, if_body_label->array);
    string_append(switching_output, "\n");

    //id_type was float, now we compare if expression is zero
    string_append(switching_output, "LABEL ");
    string_append(switching_output, if_float_label->array);
    string_append(switching_output, "\n");

    //if expression is equal to zero jump to else body
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, else_body_label->array);
    string_append(switching_output, " ");
    printing_frame_to_variable(switching_output);
    string_append(switching_output, if_expression->array);
    string_append(switching_output, "float@0x0.0p+0");
    string_append(switching_output, "\n");

    //expression was evaluated as true, continue in if
    string_append(switching_output, "LABEL ");
    string_append(switching_output, if_body_label->array);
    string_append(switching_output, "\n");
    string_append(switching_output, "\n#in if statement\n");

    string_free(id_type);
    string_free(if_expression);
    string_free(if_int_label);
    string_free(if_float_label);
    string_free(if_bool_label);
    string_free(else_body_label);
    string_free(if_body_label);
}

void generate_else() {
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_t *else_body_label = string_create_init();
    //uniq label for else if
    string_t *else_if_end_label = string_create_init();
    create_unic_label(else_if_end_label, &uniq_if_label, "%else_if_end_label");
    //uniq label for jumping to the body of else
    create_unic_label(else_body_label, &uniq_if_label, "%else_body_label");

    string_append(switching_output, "JUMP ");
    string_append(switching_output, else_if_end_label->array);
    string_append(switching_output, "\n");

    string_append(switching_output, "\n#in else body\n");
    string_append(switching_output, "LABEL ");
    string_append(switching_output, else_body_label->array);
    string_append(switching_output, "\n");
    //uniq for if labels will be set to one
    string_free(else_body_label);
    string_free(else_if_end_label);
}

void generate_elseif_end(){
    string_t *switching_output;
    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    //else if end label
    string_t *else_if_end_label = string_create_init();
    create_unic_label(else_if_end_label, &uniq_if_label, "%else_if_end_label");
    string_append(switching_output, "LABEL ");
    string_append(switching_output, else_if_end_label->array);
    string_append(switching_output, "\n");
    uniq_if_label+=1;
    string_free(else_if_end_label);
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
