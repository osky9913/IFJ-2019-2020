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


 fucking @todo div delenie nulou , div pretypovanie na float, every sprintf rewrite you dump bitch, retval in func def

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
    string_append(function_definitions, "JUMP %MAIN\n\n\n");
    string_append(output_code, "\n\n\nLABEL %MAIN");
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

    printing_frame_to_variable(frame);
    switch (operand->type) {
        case TTYPE_ID:
            string_append(frame, operand->attribute.string);
            break;
        case TTYPE_INT:
            sprintf(temp, "%ld", operand->attribute.integer);
            string_append(frame, temp);
            break;
        case TTYPE_DOUBLE:
            sprintf(temp, "0x%lfp+0", operand->attribute.decimal);
            string_append(frame, temp);
            break;
        case TTYPE_STR:
            string_append(frame, operand->attribute.string);
            break;
        default:
            printf("There is a problem");
    }
}


void adding_operands(string_t *frame, string_t *result, token_t *operand1, token_t *operand2) {
    string_append(frame, result->array);
    string_append(frame, " ");
    printing_token_to_frame(frame, operand1);
    string_append(frame, " ");
    printing_token_to_frame(frame, operand2);
    string_append(frame, "\n");

}


void adding_operands_string(string_t *frame, string_t *result, string_t *operand1, string_t *operand2) {
    string_append(frame, result->array);
    string_append(frame, " ");
    string_append(frame, operand1->array);
    string_append(frame, " ");
    string_append(frame, operand2->array);
    string_append(frame, "\n");

}

void define_uniq_variable(string_t *variable, string_t *output_string, int *uniq_temp, char *name) {
    variable = string_create_init();
    if (name) {
        string_append(variable, name);
    }
    create_unic_variable(variable, uniq_temp);
    string_append(output_string, "DEFVAR ");
    printing_frame_to_variable(output_string);
    string_append(output_string, variable->array);
    string_append(output_string, "\n");
    *uniq_temp = *uniq_temp + 1;

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
    string_t *result = string_create_init();
    //string which will store result of current operation (DEFVAR result)
    define_uniq_variable(result, switching_output, &uniq, NULL);
    //string which will store first operand (DEFVAR operand1)
    string_t *variable1 = string_create_init();
    define_uniq_variable(variable1, switching_output, &uniq, NULL);

    //string which will store second operand (DEFVAR operand1)
    string_t *variable2 = string_create_init();
    define_uniq_variable(variable2, switching_output, &uniq, NULL);

    //string used as temporary storing place in operator >= or <=
    string_t *result_eq_1 = string_create_init();
    define_uniq_variable(result_eq_1, switching_output, &uniq, NULL);

    //string used as temporary storing place in operator >= or <=
    string_t *result_eq_2 = string_create_init();
    define_uniq_variable(result_eq_2, switching_output, &uniq, NULL);

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
            string_append(switching_output, "JUMPIFNEQ #addition "); // kontrola pri idive  ci su int
            print_variable_from_string(switching_output, variable1->array);
            string_append(switching_output, " string@int\n");


            string_append(switching_output, "JUMPIFEQ #addition "); // kontrola pri idive  ci  je to rovne nula
            printing_token_to_frame(switching_output, operand2);
            string_append(switching_output, " int@0\n");


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
    string_append(function_definitions, "CREATEFRAME\n");
}

void generate_function(token_t *id) {
    string_append(function_definitions, "LABEL !");
    string_append(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");
    string_append(function_definitions, "PUSHFRAME\n");
    string_append(function_definitions, "DEFVAR LF@%%return_value\n");
}


void generate_call_function(const char* id) {
    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }
    string_append(switching_output, "CALL !");
    string_append(switching_output, id);
    string_append(switching_output, "\n");
}


void generate_def_param(token_t *id) {
    string_append(function_definitions, "DEFVAR ");
    printing_frame_to_variable(function_definitions);
    string_append(function_definitions, id->attribute.string);
    string_append(function_definitions, "\n");

    string_t *parameter = string_create_init();
    string_append(parameter, "%%param");
    create_unic_variable(parameter, &uniq_param_def);
    uniq_param_def++;
    string_append(parameter, "\n");
    string_append(function_definitions, "MOVE ");
    printing_frame_to_variable(function_definitions);
    string_append(function_definitions, id->attribute.string);
    string_append(function_definitions, " ");
    printing_frame_to_variable(function_definitions);
    string_append(function_definitions, parameter->array);
    // todo uvolnit
}

void generate_call_param(token_t *id) {
    string_t *parameter = string_create_init();
    string_append(parameter, "%%param");
    create_unic_variable(parameter, &uniq_param_call);
    uniq_param_call++;

    string_append(output_code, "DEFVAR TF@");
    string_append(output_code, parameter->array);
    string_append(output_code, "MOVE ");
    string_append(output_code, parameter->array);
    string_append(output_code, " ");
    printing_token_to_frame(output_code, id);
    string_append(output_code, "\n");

    // todo uvolnit
}

void generate_function_end() {
    string_append(function_definitions, "POPFRAME \n");
    string_append(function_definitions, "RETURN \n");
    uniq_param_call = 1;
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
}

void generate_while(token_t *expression) {

    string_t *switching_output;

    if (in_function) {
        switching_output = function_definitions;
    } else {
        switching_output = output_code;
    }

    string_t *while_expression = string_create_init();
    define_uniq_variable(while_expression, switching_output, &uniq_expression, "%%while_expression");

    if (expression->type == TTYPE_ID) {

        string_t *id_type = string_create_init();
        define_uniq_variable(id_type, switching_output, &uniq_expression, "%%while_check_type");

        //TYPE id_type expression
        get_type_variable(id_type, switching_output, expression);

        //JUMPIFEQ int_label id_type string@int
        string_append(switching_output, "JUMPIFEQ @todo_while_int_label ");
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
        printing_frame_to_variable(switching_output);
        string_append(switching_output, id_type->array);
        string_append(switching_output, " string@bool\n");

        //while (string) or while (None) -> while end
        string_append(switching_output, "JUMP @todo_while_end_label ");

        //id_type was bool, now we compare if expression is false
        string_append(switching_output, "LABEL @todo_while_bool_label ");
        //JUMPIFEQ @todo_while_end_label expression bool@false
        string_append(switching_output, "JUMPIFEQ @todo_while_end_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "bool@false");
        string_append(switching_output, "JUMP @todo_while_body_label ");
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
    string_t *while_expression = NULL;
    define_uniq_variable(while_expression, switching_output, &uniq_expression, "%%while_expression");

    if (expression->type == TTYPE_ID) {

        string_t *id_type = string_create_init();
        define_uniq_variable(id_type, switching_output, &uniq_expression, "%%if_check_type");

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
        string_append(switching_output, "JUMP @todo_else_label ");

        //id_type was bool, now we compare if expression is false
        string_append(switching_output, "LABEL @todo_if_bool_label ");
        //JUMPIFEQ @todo_while_end_label expression bool@false
        string_append(switching_output, "JUMPIFEQ @todo_else_label ");
        printing_frame_to_variable(switching_output);
        string_append(switching_output, expression->attribute.string);
        string_append(switching_output, "bool@false");
        string_append(switching_output, "JUMP @todo_if_label ");
    }
    if (expression->type == TTYPE_INT) {
        //id_type was int, now we compare if expression is zero
        string_append(switching_output, "LABEL @todo_if_int_label ");
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