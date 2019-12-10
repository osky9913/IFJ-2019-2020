#include "scanner.h"
#include "parser.h"
#include "errors.h"
#include "generate_ver2.h"
#include <stdio.h>
#include <stdlib.h>



generate_strings_output_t assembly_code;
uniq_id_t identificator;
labels_stacks_t general_stacks;

string_t * switch_definitions_frame(){
    if (in_function ){
        return assembly_code.function_definitions;
    }
    return  assembly_code.main;
}

int generate_strings_input_init(){

    assembly_code.stash = string_create_init();
    assembly_code.errors = string_create_init();
    assembly_code.function_definitions = string_create_init();
    assembly_code.main = string_create_init();
    assembly_code.print = string_create_init();
    //checks alloc
    if( assembly_code.stash == NULL ||
        assembly_code.errors == NULL ||
        assembly_code.function_definitions == NULL ||
        assembly_code.main == NULL ||
        assembly_code.print == NULL  
        )
    {
        return ALLOC_ERROR;
    }
    return SUCCESS;
}
void generate_assembly_start(){
    string_append(assembly_code.errors, "EXIT int@0\n");
    string_append(assembly_code.errors, "LABEL %error_label_semantic\n");
    string_append(assembly_code.errors, "DPRINT string@RUNTIME_SEMANTIC_ERROR\n");
    string_append(assembly_code.errors, "EXIT int@4\n");

    string_append(assembly_code.errors, "LABEL %error_label_0\n");
    string_append(assembly_code.errors, "DPRINT string@DIVISION_BY_ZERO\n");
    string_append(assembly_code.errors, "EXIT int@9\n");



    string_append(assembly_code.function_definitions, ".IFJcode19\n");
    string_append(assembly_code.function_definitions, "JUMP %MAIN\n");
    string_append(assembly_code.function_definitions, "\n#>---------------------------FUNCTIONS---------------------------<\n");

    //inputi
    string_append(assembly_code.function_definitions, "LABEL !inputi\n"
                                        "PUSHFRAME\n"
                                        "DEFVAR LF@%%return_value\n"
                                        "READ LF@%%return_value int\n"
                                        "POPFRAME\n"
                                        "RETURN\n");
    //inputf
    string_append(assembly_code.function_definitions, "LABEL !inputf\n"
                                        "PUSHFRAME\n"
                                        "DEFVAR LF@%%return_value\n"
                                        "READ LF@%%return_value float\n"
                                        "POPFRAME\n"
                                        "RETURN\n");
    //inputs
    string_append(assembly_code.function_definitions, "LABEL !inputs\n"
                                        "PUSHFRAME\n"
                                        "DEFVAR LF@%%return_value\n"
                                        "MOVE LF@%%return_value string@\n"
                                        "DEFVAR LF@actual_string\n"
                                        "DEFVAR LF@actual_char\n"
                                        "DEFVAR LF@str_len\n"
                                        "DEFVAR LF@increment\n"
                                        "MOVE LF@increment int@0\n"
                                        "READ LF@actual_string string\n"
                                        "STRLEN LF@str_len LF@actual_string\n"
                                        "JUMPIFEQ &len&was&zero LF@str_len int@0\n"
                                        "SUB LF@str_len LF@str_len int@1\n"
                                        "LABEL &len&cycle\n"
                                        "GETCHAR LF@actual_char LF@actual_string LF@increment\n"
                                        "JUMPIFEQ &len&end LF@increment LF@str_len\n"
                                        "CONCAT LF@%%return_value LF@%%return_value LF@actual_char\n"
                                        "ADD LF@increment LF@increment int@1\n"
                                        "JUMP &len&cycle\n"
                                        "LABEL &len&end\n"
                                        "CONCAT LF@%%return_value LF@%%return_value LF@actual_char\n"
                                        "LABEL &len&was&zero\n"
                                        "POPFRAME\n"
                                        "RETURN\n");
    //len
    string_append(assembly_code.function_definitions, "LABEL !len\n"
                                        "PUSHFRAME\n"
                                        "DEFVAR LF@%%return_value\n"
                                        "DEFVAR LF@type\n"
                                        "TYPE LF@type LF@%param1\n"
                                        "JUMPIFNEQ %len_wrong_type LF@type string@string\n"
                                        "STRLEN LF@%%return_value LF@%param1	#dlzka stringu v prvom argumente, len(s)\n"
                                        "POPFRAME\n"
                                        "RETURN\n"
                                        "LABEL %len_wrong_type\n"
                                        "EXIT int@4\n");
    //chr
    string_append(assembly_code.function_definitions, "LABEL !chr\n"
                                        "PUSHFRAME\n"
                                        "DEFVAR LF@%%return_value\n"
                                        "DEFVAR LF@type\n"
                                        "TYPE LF@type LF@%param1\n"
                                        "JUMPIFNEQ %chr_wrong_type LF@type string@int\n"
                                        "INT2CHAR LF@%%return_value LF@%param1\n"
                                        "POPFRAME\n"
                                        "RETURN\n"
                                        "LABEL %chr_wrong_type\n"
                                        "EXIT int@4\n");
    //substr
    string_append(assembly_code.function_definitions, "#substr(s, i, n)\n"
                                        "LABEL !substr\n"
                                        "PUSHFRAME\n"
                                        "DEFVAR LF@%%return_value\n"
                                        "MOVE LF@%%return_value string@\n"
                                        "DEFVAR LF@str_len\n"
                                        "DEFVAR LF@passed_type 	#premenna na ulozenie typu hodnoty v prvom argumente\n"
                                        "TYPE LF@passed_type LF@%param1\n"
                                        "JUMPIFNEQ %substr_wrong_type LF@passed_type string@string	#arg1 je to typ string?\n"
                                        "TYPE LF@passed_type LF@%param2\n"
                                        "JUMPIFNEQ %substr_wrong_type LF@passed_type string@int	#arg1 je to typ string?\n"
                                        "TYPE LF@passed_type LF@%param3\n"
                                        "JUMPIFNEQ %substr_wrong_type LF@passed_type string@int	#arg1 je to typ string?\n"
                                        "LABEL &substr&continue3\n"
                                        "DEFVAR LF@arg3_test\n"
                                        "LT LF@arg3_test LF@%param3 int@0\n"
                                        "JUMPIFEQ &substr&continue4 LF@arg3_test bool@false 	#arg3 >= 0?\n"
                                        "MOVE LF@%%return_value string@None		#nieje int -> koniec\n"
                                        "JUMP &substr&end\n"
                                        "LABEL &substr&continue4\n"
                                        "DEFVAR LF@final_str_len\n"
                                        "STRLEN LF@str_len LF@%param1\n"
                                        "SUB LF@final_str_len LF@str_len LF@%param2					#len(s)−𝑖\n"
                                        "SUB LF@str_len LF@str_len int@1\n"
                                        "DEFVAR LF@cond_up_length\n"
                                        "DEFVAR LF@cond_down_length\n"
                                        "DEFVAR LF@cond_final\n"
                                        "LT LF@cond_up_length LF@%param2 LF@str_len 					# i < strlen-1\n"
                                        "GT LF@cond_down_length LF@%param2 int@-1						# i > -1\n"
                                        "AND LF@cond_final LF@cond_up_length LF@cond_down_length		#(i < strlen-1 && i > -1)\n"
                                        "JUMPIFEQ &substr&continue5 LF@cond_final bool@true \n"
                                        "MOVE LF@%%return_value string@None\n"
                                        "JUMP &substr&end\n"
                                        "LABEL &substr&continue5\n"
                                        "DEFVAR LF@actual_char\n"
                                        "DEFVAR LF@cond_str_len\n"
                                        "DEFVAR LF@last_index\n"
                                        "LT LF@cond_str_len LF@final_str_len LF@%param3\n"
                                        "JUMPIFEQ &substr&continue6 LF@cond_str_len bool@true		# len(s)−𝑖 < n\n"
                                        "#velkost pozadovaneho stringu je mensia ako pocet zostavajucich charakterov, len(s)−𝑖 > n\n"
                                        "ADD LF@last_index LF@%param2 LF@%param3								#cyklim po index [i + n]\n"
                                        "JUMP &substr&cycle1\n"
                                        "LABEL &substr&continue6\n"
                                        "ADD LF@str_len LF@str_len int@1\n"
                                        "MOVE LF@last_index LF@str_len\n"
                                        "LABEL &substr&cycle1\n"
                                        "JUMPIFEQ &substr&end LF@last_index LF@%param2\n"
                                        "GETCHAR LF@actual_char LF@%param1 LF@%param2							#chcem char z LF@%param1[i]\n"
                                        "CONCAT LF@%%return_value LF@%%return_value LF@actual_char\n"
                                        "ADD LF@%param2 LF@%param2 int@1											#inkrementujem i++(LF@%param2++)\n"
                                        "JUMP &substr&cycle1\n"
                                        "#velkost pozadovaneho stringu je vacsie ako pocet zostavajucich charakterov, len(s)−𝑖 > n\n"
                                        "LABEL &substr&end\n"
                                        "POPFRAME\n"
                                        "RETURN\n"
                                        "LABEL %substr_wrong_type\n"
                                        "EXIT int@4\n");
    //ord
    string_append(assembly_code.function_definitions,
                  "LABEL !ord\n"
                  "PUSHFRAME\n"
                  "DEFVAR LF@%%return_value\n"
                  "DEFVAR LF@y\n"
                  "DEFVAR LF@passed_type 	#premenna na ulozenie typu hodnoty v prvom argumente\n"
                  "DEFVAR LF@compare_type 	#string na porovnanie typu s argumentom funkcie\n"
                  "DEFVAR LF@param1		#prvy argument	\n"
                  "MOVE LF@param1 LF@%param1\n"
                  "TYPE LF@passed_type LF@param1\n"
                  "JUMPIFEQ &ord&continue1 LF@passed_type string@string	#je to typ string?\n"
                  "EXIT int@4\n"
                  "MOVE LF@%%return_value string@None\n"
                  "JUMP &ord&end\n"
                  "LABEL &ord&continue1\n"
                  "DEFVAR LF@param2 					#druhy argument		\n"
                  "MOVE LF@param2 LF@%param2\n"
                  "TYPE LF@passed_type LF@param2\n"
                  "JUMPIFEQ &ord&continue2 LF@passed_type string@int 	#je to typ int?\n"
                  "EXIT int@4\n"
                  "LABEL &ord&continue2\n"
                  "DEFVAR LF@str_len\n"
                  "DEFVAR LF@cond_up_length\n"
                  "DEFVAR LF@cond_down_length\n"
                  "DEFVAR LF@cond_final\n"
                  "STRLEN LF@str_len LF@param1\n"
                  "LT LF@cond_up_length LF@param2 LF@str_len 					#param2 < strlen-1\n"
                  "GT LF@cond_down_length LF@param2 int@-1						#param2 > -1\n"
                  "AND LF@cond_final LF@cond_up_length LF@cond_down_length		#(param2 < strlen-1 && param2 > -1)\n"
                  "JUMPIFEQ &ord&succes LF@cond_final bool@true \n"
                  "MOVE LF@%%return_value string@None\n"
                  "JUMP &ord&end\n"
                  "LABEL &ord&succes\n"
                  "STRI2INT LF@%%return_value LF@param1 LF@param2\n"
                  "LABEL &ord&end\n"
                  "POPFRAME\n"
                  "RETURN\n");
    string_append(assembly_code.main, "\n#>---------------------------FUNCTIONS---------------------------<\n");
    string_append(assembly_code.main, "\n#>---------------------------MAIN---------------------------<\n");
    string_append(assembly_code.main, "\nLABEL %MAIN\n");
    string_append(assembly_code.errors, "\n#>---------------------------MAIN---------------------------<\n");
}

int start_program(){
    if (generate_strings_input_init() == ALLOC_ERROR){
        return ERROR_INTERNAL;
    }

    general_stacks.if_labels_stack = stack_general_init();
    general_stacks.while_labels_stack = stack_general_init();
    if( !general_stacks.if_labels_stack || !general_stacks.while_labels_stack ){
        return ERROR_INTERNAL; // check
    }

    generate_assembly_start();
    identificator.param_def = 1;
    return SUCCESS;
}

void free_assembly_code() {
    string_free(assembly_code.main);
    string_free(assembly_code.stash);
    string_free(assembly_code.function_definitions);
    string_free(assembly_code.errors);
    string_free(assembly_code.print);
    free(general_stacks.if_labels_stack);
    free(general_stacks.while_labels_stack);
}


void end_program() {
    string_append(assembly_code.function_definitions, assembly_code.print->array);
    string_append(assembly_code.function_definitions, assembly_code.main->array);
    string_append(assembly_code.function_definitions, assembly_code.errors->array);
    printf("%s\n", assembly_code.function_definitions->array);
}

void append_frame_to_variable(string_t *frame) {
    if (in_function) {
        string_append(frame, "LF@");
    } else {
        string_append(frame, "GF@");
    }
}

void append_token_variable_to_assembly(string_t * frame , token_t * variable){
    string_append(frame," ");
    switch (variable->type) {
        case TTYPE_ID:
            append_frame_to_variable(frame);
            string_append(frame, variable->attribute.string);
            break;
        case TTYPE_INT:
            string_append(frame,"int@");
            string_append(frame, variable->attribute.string);

            break;
        case TTYPE_DOUBLE:
            string_append(frame,"float@0x");
            string_append(frame, variable->attribute.string);
            string_append(frame,"p+0");
            break;
        case TTYPE_STR:
            string_append(frame, "string@");
            string_append(frame, variable->attribute.string);
            break;
        case TTYPE_NONE:
            string_append(frame, "nil@nil");
            break;
        default:
            fprintf(stderr, "There is a problem here, token passed to generator was neither of ID/INT/DOUBLE/STR/NONE\n");
    }

}

void append_string_variable_to_assembly(string_t *frame, const char *variable){
    string_append(frame, " ");
    append_frame_to_variable(frame);
    string_append(frame, variable);
}

void convert_floats_to_ints(string_t * frame,
                            string_t * flag_1,  string_t * flag_2,
                            token_t * assembly_1_token, token_t * assembly_2_token,
                            string_t * label1, string_t * label2 ,
                            string_t * end_of_expression ){
    string_append(frame,"JUMPIFEQ ");
    string_append(frame,label1->array);
    append_string_variable_to_assembly(frame,flag_1->array);
    string_append(frame," int@1\n");

    string_append(frame,"JUMPIFEQ ");
    string_append(frame,label2->array);
    append_string_variable_to_assembly(frame,flag_2->array);
    string_append(frame," int@1\n");
    generate_jump(frame,"JUMP",end_of_expression->array);




    generate_label(frame,label1->array);
    string_append(frame, "FLOAT2INT");
    append_token_variable_to_assembly(frame,assembly_1_token);
    append_token_variable_to_assembly(frame,assembly_1_token);
    string_append(frame,"\n");



    string_append(frame,"JUMPIFEQ ");
    string_append(frame,label2->array);
    append_string_variable_to_assembly(frame,flag_2->array);
    string_append(frame," int@1\n");
    generate_jump(frame,"JUMP",end_of_expression->array);



    generate_label(frame,label2->array);
    string_append(frame, "FLOAT2INT");
    append_token_variable_to_assembly(frame,assembly_2_token);
    append_token_variable_to_assembly(frame,assembly_2_token);
    string_append(frame,"\n");
    generate_jump(frame,"JUMP",end_of_expression->array);

}




void convert_ints_to_floats( string_t * frame, string_t * int_to_float, string_t *variable1,
                            token_t *assembly_1_token,token_t * assembly_2_token ,
                            string_t * jump_destination_label, bool is_div_zero, string_t * flag1, string_t * flag2 ){


    generate_jumpeq_string_char(frame,"JUMPIFEQ",int_to_float,variable1,"string@int");



    if(is_div_zero) {
        string_append(frame, "JUMPIFEQ ");
        string_append(frame, "%error_label_0 ");
        append_token_variable_to_assembly(frame, assembly_2_token);
        string_append(frame, " int@0\n");
    }


    // variable1 is not int so variable2 is int
    string_append(frame, "INT2FLOAT");
    append_token_variable_to_assembly(frame,assembly_2_token);
    append_token_variable_to_assembly(frame,assembly_2_token);
    string_append(frame,"\n");

    string_append(frame, "MOVE");
    append_string_variable_to_assembly(frame,flag2->array);
    string_append(frame," int@1");
    string_append(frame,"\n");



    // jump
    generate_jump(frame,"JUMP",jump_destination_label->array);


    // int to flat variable one
    generate_label(frame,int_to_float->array);
    string_append(frame, "INT2FLOAT");
    append_token_variable_to_assembly(frame,assembly_1_token);
    append_token_variable_to_assembly(frame,assembly_1_token);
    string_append(frame,"\n");

    string_append(frame, "MOVE");
    append_string_variable_to_assembly(frame,flag1->array);
    string_append(frame," int@1");
    string_append(frame,"\n");


}


void append_token_operands_to_assembly(string_t *frame, string_t *result, token_t *operand1, token_t *operand2) {
    append_string_variable_to_assembly(frame,result->array);
    append_token_variable_to_assembly(frame, operand1);
    append_token_variable_to_assembly(frame, operand2);
    string_append(frame, "\n");
}

void append_string_operands_to_assembly(string_t *frame, string_t *result, string_t *operand1, string_t *operand2) {
    append_string_variable_to_assembly(frame, result->array);
    append_string_variable_to_assembly(frame, operand1->array);
    append_string_variable_to_assembly(frame, operand2->array);
    string_append(frame, "\n");
}


void generate_jump(string_t * frame,const char * jump_instruction , const char * destination_label ){
    string_append(frame,jump_instruction);
    string_append(frame," ");
    string_append(frame,destination_label);
    string_append(frame,"\n");
}
void generate_label(string_t * frame,const char * label){
    string_append(frame,"LABEL");
    string_append(frame," ");
    string_append(frame,label);
    string_append(frame,"\n");
}

string_t* define_uniq_variable(string_t *frame, int *uniq, char *name){
    
    string_t* variable = string_create_init();
    if(variable == NULL ){
        fprintf(stderr,"ALLOC ERROR");
        return NULL;
    }

    if (name) {
        create_unic_variable(variable, uniq, name);
    }
    else {
        create_unic_variable(variable, uniq, "var");
    }

    string_append(frame, "DEFVAR");
    append_string_variable_to_assembly(frame,variable->array);
    string_append(frame, "\n");
    return variable;
}


void get_type_variable( string_t *frame, string_t *id_type,token_t *operand){
    string_append(frame, "TYPE ");
    append_string_variable_to_assembly(frame, id_type->array);
    append_token_variable_to_assembly(frame,operand);
    string_append(frame, "\n");
}


void check_if_op_type_eq(string_t *frame, char *variable_type, char *type, char *label){
    string_append(frame, "JUMPIFEQ ");
    string_append(frame, label);
    append_string_variable_to_assembly(frame, variable_type);
    string_append(frame, " ");
    string_append(frame, type);
    string_append(frame, "\n");
}

//void generate_error_labels(){return;};
//void convert_float_to_int(string_t* frame, token_t* operand);

string_t * switch_frame(){
    return assembly_code.stash;
}

void generate_jumpeq_string_char(string_t * frame , const char * jump , string_t * label , string_t * variable, const char * type ){
    string_append(frame, jump);
    string_append(frame, " ");
    string_append(frame, label->array);
    append_string_variable_to_assembly(frame,variable->array);
    string_append(frame, " ");
    string_append(frame, type);
    string_append(frame, "\n");

}


//char *generate_expression(token_t *assembly_2_token, token_t *operator, token_t *assembly_1_token){
char *generate_expression(token_t *operand2, token_t *operator, token_t *operand1){


    string_t *switching_output = switch_frame();
    string_t *switching_output_definitions = switch_definitions_frame();

    // -----------------------------------------------------------definice DEFVAR--------------------------------------------------------------
    string_t  * temporary_div1 = define_uniq_variable(switching_output_definitions, &identificator.general, "temporary_div");
    string_t  * temporary_div2 = define_uniq_variable(switching_output_definitions, &identificator.general, "temporary_div");

    string_t  * flag_1 = define_uniq_variable(switching_output_definitions, &identificator.general, "flag_1_variable");
    string_t  * flag_2 = define_uniq_variable(switching_output_definitions, &identificator.general, "flag_2_variable");


    string_t *result = define_uniq_variable(switching_output_definitions, &identificator.general, "result");
    //string which will store result of current operation (DEFVAR result)
    //string which will store first operand (DEFVAR operand1)
    string_t *variable1 = define_uniq_variable(switching_output_definitions, &identificator.general, "var");
    //string which will store second operand (DEFVAR operand1)
    string_t *variable2 = define_uniq_variable(switching_output_definitions, &identificator.general, "var");
    //string used as temporary storing place in operator >= or <=
    string_t *result_eq_1 = define_uniq_variable(switching_output_definitions, &identificator.general,"tmp_result");
    //string used as temporary storing place in operator >= or <=
    string_t *result_eq_2 = define_uniq_variable(switching_output_definitions, &identificator.general, "tmp_result");
    //string used as label for the end of expression evaluation

    string_t * assembly_1_variable = define_uniq_variable(switching_output_definitions, &identificator.general, "assembly_1_variable");
    string_t * assembly_2_variable = define_uniq_variable(switching_output_definitions, &identificator.general, "assembly_2_variable");

    token_t * assembly_1_token = tokenGen(assembly_1_variable->array);
    token_t * assembly_2_token = tokenGen(assembly_2_variable->array);


    string_append(switching_output,"MOVE");
    append_token_variable_to_assembly(switching_output,assembly_1_token);
    append_token_variable_to_assembly(switching_output,operand1);
    string_append(switching_output,"\n");

    string_append(switching_output,"MOVE");
    append_token_variable_to_assembly(switching_output,assembly_2_token);
    append_token_variable_to_assembly(switching_output,operand2);
    string_append(switching_output,"\n");



    string_append(switching_output,"MOVE");
    append_string_variable_to_assembly(switching_output,flag_1->array);
    string_append(switching_output," int@0");
    string_append(switching_output,"\n");


    string_append(switching_output,"MOVE");
    append_string_variable_to_assembly(switching_output,flag_2->array);
    string_append(switching_output," int@0");
    string_append(switching_output,"\n");



    // -----------------------------------------------------------definice DEFVAR--------------------------------------------------------------

    //-------------------------------------------------------------Labels for operation -----------------------------------------------
    string_t * label_LSOREQ = string_create_init();
    string_t * label_GTOREQ = string_create_init();
    string_t * label_ISEQ = string_create_init();
    string_t * label_LS = string_create_init();
    string_t * label_GT = string_create_init();
    string_t * label_ADD = string_create_init();
    string_t * label_SUB = string_create_init();
    string_t * label_MUL = string_create_init();
    string_t * label_DIV = string_create_init();
    string_t * label_IDIV = string_create_init();
    string_t * label_ISNEQ = string_create_init();

    string_t * int_to_float = string_create_init();

    string_t * _2_ints_to_floats = string_create_init(); // sorry. there is no other way el simon

    string_t * float_to_int = string_create_init();

    string_t  * label_bool_int_float_1 = string_create_init();
    string_t  * label_bool_int_float_2 = string_create_init();
    string_t  * is_eq_1_int = string_create_init();
    string_t  * is_eq_2_int = string_create_init();



    create_unic_label(label_LSOREQ,&identificator.label,"%label_LSOREQ");
    create_unic_label(label_GTOREQ,&identificator.label,"%label_GTOREQ");
    create_unic_label(label_ISEQ,&identificator.label,"%label_ISEQ");
    create_unic_label(label_LS,&identificator.label,"%label_LS");
    create_unic_label(label_GT,&identificator.label,"%label_GT");
    create_unic_label(label_ADD,&identificator.label,"%label_ADD");
    create_unic_label(label_SUB,&identificator.label,"%label_SUB");
    create_unic_label(label_MUL,&identificator.label,"%label_MUL");
    create_unic_label(label_DIV,&identificator.label,"%label_DIV");
    create_unic_label(label_IDIV,&identificator.label,"%label_IDIV");
    create_unic_label(label_ISNEQ,&identificator.label,"%label_ISNEQ");


    create_unic_label(int_to_float,&identificator.label,"%label_int_to_float");
    create_unic_label(float_to_int,&identificator.label,"%label_float_to_int");
    create_unic_label(_2_ints_to_floats,&identificator.label,"%_2_int_to_float");


    create_unic_label(label_bool_int_float_1, &identificator.label, "%bool_int_to_float_1");
    create_unic_label(label_bool_int_float_2, &identificator.label, "%bool_int_to_float_2");
    create_unic_label(is_eq_1_int, &identificator.label, "%is_eq_1_int");
    create_unic_label(is_eq_2_int, &identificator.label, "%is_eq_2_int");


    //-------------------------------------------------------------Labels for operation -----------------------------------------------
    //------------------------------------------------------------ Labels for expression -----------------------------------------------------
    string_t *end_of_expression = string_create_init();
    create_unic_label(end_of_expression, &identificator.label, "%end_expression_label");
    string_t *concatenation_label = string_create_init();
    create_unic_label(concatenation_label, &identificator.concat_label, "%concatenation");
    string_t *label_int_dodge = string_create_init();
    create_unic_label(label_int_dodge, &identificator.label, "%label_float_dodge");

    //------------------------------------------------------------ Labels for expression -----------------------------------------------------


    //-------------------------------------------------------komparacia typov--------------------------------------------------------------------

    string_append(switching_output, "\n#evaluating expression\n");


    // type of first operand
    get_type_variable( switching_output, variable1, assembly_1_token);

    // type of second operand
    get_type_variable(switching_output,variable2, assembly_2_token);


    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------

    //--------------------------------------------------------vykonavanie expression-------------------------------------------------------------
    switch (operator->type) {

        case TTYPE_LSOREQ:
            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");


            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");



            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_LSOREQ,variable1,variable2);
            // if one of variables is string and the other variable is another type jump to error <3
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");


            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_LSOREQ, false,flag_1,flag_2);
            generate_label(switching_output,label_LSOREQ->array);


            //check if operand1 < operand2
            string_append(switching_output, "LT");
            append_token_operands_to_assembly(switching_output, result_eq_1, assembly_1_token, assembly_2_token);
            //check if operand1 == operand2
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result_eq_2, assembly_1_token, assembly_2_token);
            //(op1 < op2) or (op1 == op2) => operand1 <= operand2
            string_append(switching_output, "OR");
            append_string_operands_to_assembly(switching_output, result, result_eq_1, result_eq_2);

            convert_floats_to_ints(switching_output,flag_1,flag_2,assembly_1_token,assembly_2_token,label_bool_int_float_1,label_bool_int_float_2,end_of_expression);




            break;
        case TTYPE_GTOREQ:
            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");


            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");



            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_GTOREQ,variable1,variable2);
            // if one of variables is string and the other variable is another type jump to error <3
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");

            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_GTOREQ, false,flag_1,flag_2);
            generate_label(switching_output,label_GTOREQ->array);


            //check if operand1 < operand2
            string_append(switching_output, "GT");
            append_token_operands_to_assembly(switching_output, result_eq_1, assembly_1_token, assembly_2_token);
            //check if operand1 == operand2
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result_eq_2, assembly_1_token, assembly_2_token);
            //(op1 < op2) or (op1 == op2) => operand1 <= operand2
            string_append(switching_output, "OR");
            append_string_operands_to_assembly(switching_output, result, result_eq_1, result_eq_2);
            convert_floats_to_ints(switching_output,flag_1,flag_2,assembly_1_token,assembly_2_token,label_bool_int_float_1,label_bool_int_float_2,end_of_expression);


            break;
        case TTYPE_LS:
            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");


            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");



            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_LS,variable1,variable2);
            // if one of variables is string and the other variable is another type jump to error <3
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");


            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_LS, false,flag_1,flag_2);
            generate_label(switching_output,label_LS->array);


             // int , float , stringy


            string_append(switching_output, "LT");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            convert_floats_to_ints(switching_output,flag_1,flag_2,assembly_1_token,assembly_2_token,label_bool_int_float_1,label_bool_int_float_2,end_of_expression);

            break;


        case TTYPE_GT:
            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");


            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");



            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_GT,variable1,variable2);
            // if one of variables is string and the other variable is another type jump to error <3
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");


            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_GT, false,flag_1,flag_2);
            generate_label(switching_output,label_GT->array);



            string_append(switching_output, "GT");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            convert_floats_to_ints(switching_output,flag_1,flag_2,assembly_1_token,assembly_2_token,label_bool_int_float_1,label_bool_int_float_2,end_of_expression);

            break;


        case TTYPE_ISEQ:


            //------------konvertovanie ------------
            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",label_bool_int_float_1,variable1,"string@int");
            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",label_bool_int_float_2,variable2,"string@int");
            generate_jump(switching_output,"JUMP",label_ISEQ->array);


            generate_label(switching_output,label_bool_int_float_1->array);
            string_append(switching_output,"MOVE");
            append_string_variable_to_assembly(switching_output,flag_1->array);
            string_append(switching_output," int@1\n");

            string_append(switching_output, "INT2FLOAT");
            append_token_variable_to_assembly(switching_output,assembly_1_token);
            append_token_variable_to_assembly(switching_output,assembly_1_token);
            string_append(switching_output,"\n");


            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",label_bool_int_float_2,variable2,"string@int");
            generate_jump(switching_output,"JUMP", label_ISEQ->array);

            generate_label(switching_output,label_bool_int_float_2->array);
            string_append(switching_output,"MOVE");
            append_string_variable_to_assembly(switching_output,flag_2->array);
            string_append(switching_output," int@1\n");


            string_append(switching_output, "INT2FLOAT");
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            string_append(switching_output,"\n");

            //------------konvertovanie ------------



            generate_label(switching_output,label_ISEQ->array);
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);


            //------------konvertovanie spat------------
            convert_floats_to_ints(switching_output,flag_1,flag_2,assembly_1_token,assembly_2_token,is_eq_1_int,is_eq_2_int,end_of_expression);


            break;
        case TTYPE_ISNEQ:

            //------------konvertovanie ------------
            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",label_bool_int_float_1,variable1,"string@int");
            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",label_bool_int_float_2,variable2,"string@int");
            generate_jump(switching_output,"JUMP",label_ISNEQ->array);


            generate_label(switching_output,label_bool_int_float_1->array);
            string_append(switching_output,"MOVE");
            append_string_variable_to_assembly(switching_output,flag_1->array);
            string_append(switching_output," int@1\n");

            string_append(switching_output, "INT2FLOAT");
            append_token_variable_to_assembly(switching_output,assembly_1_token);
            append_token_variable_to_assembly(switching_output,assembly_1_token);
            string_append(switching_output,"\n");


            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",label_bool_int_float_2,variable2,"string@int");
            generate_jump(switching_output,"JUMP", label_ISNEQ->array);

            generate_label(switching_output,label_bool_int_float_2->array);
            string_append(switching_output,"MOVE");
            append_string_variable_to_assembly(switching_output,flag_2->array);
            string_append(switching_output," int@1\n");


            string_append(switching_output, "INT2FLOAT");
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            string_append(switching_output,"\n");

            //------------konvertovanie ------------



            generate_label(switching_output,label_ISNEQ->array);
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result_eq_1, assembly_1_token, assembly_2_token);

            string_append(switching_output, "EQ");
            append_string_variable_to_assembly(switching_output,result->array);
            append_string_variable_to_assembly(switching_output,result_eq_1->array);
            string_append(switching_output," bool@false\n");


            //-------------konvertovanie spat ----------------
            convert_floats_to_ints(switching_output,flag_1,flag_2,assembly_1_token,assembly_2_token,is_eq_1_int,is_eq_2_int,end_of_expression);




            break;

        case TTYPE_ADD:
            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");



            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_ADD,variable1,variable2);
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");

            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_ADD, false,flag_1,flag_2);
            generate_label(switching_output,label_ADD->array);


            // if string jump to concatenation
            string_append(switching_output, "JUMPIFEQ ");
            string_append(switching_output, concatenation_label->array);
            append_string_variable_to_assembly(switching_output, variable1->array);
            string_append(switching_output, " string@string\n");

            // + float or int
            string_append(switching_output, "ADD");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            generate_jump(switching_output,"JUMP",end_of_expression->array);


            // Label Concat
            string_append(switching_output, "\nLABEL ");
            string_append(switching_output, concatenation_label->array);

            // concat string + string
            string_append(switching_output, "\nCONCAT");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            identificator.concat_label +=1;
            break;
        case TTYPE_SUB:



            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");

            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_SUB,variable1,variable2);

            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_GT, false,flag_1,flag_2); // check if assembli1 and 2 are in order
            generate_label(switching_output,label_SUB->array);

            string_append(switching_output, "SUB");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            break;
        case TTYPE_MUL:


            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");

            //if types are equal
            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_MUL,variable1,variable2);

            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_MUL, false,flag_1,flag_2); // check if assembli1 and 2 are in order
            generate_label(switching_output,label_MUL->array);


            string_append(switching_output, "MUL");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            break;
        case TTYPE_DIV: // todo check in code
            check_if_op_type_eq(switching_output, variable1->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            check_if_op_type_eq(switching_output, variable2->array, "nil@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@nil", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable2->array, "string@bool", "%error_label_semantic");
            //-------------------------------------------------------------------------------------------

            generate_jumpeq_string_string(switching_output,"JUMPIFEQ",label_DIV,variable1,variable2);
            convert_ints_to_floats( switching_output,  int_to_float, variable1, assembly_1_token,assembly_2_token ,  label_DIV, true,flag_1,flag_2); // check if assembli1 and 2 are in order
            generate_label(switching_output,label_DIV->array);



            get_type_variable( switching_output, variable1, assembly_1_token);
            get_type_variable(switching_output,variable2, assembly_2_token);

            generate_jumpeq_string_char(switching_output,"JUMPIFEQ",_2_ints_to_floats,variable1,"string@int");

            // dva rovnake int  alebo dva floaty -> riesenie pomocny label -> _2_ints_to_floats

            //delenie nulou pre float
            string_append(switching_output, "\nJUMPIFEQ ");
            string_append(switching_output,"%error_label_0 ");
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            string_append(switching_output," float@0x0.0p+0\n");

            string_append(switching_output,"DIV");
            append_token_operands_to_assembly(switching_output,result,assembly_1_token,assembly_2_token);
            string_append(switching_output,"\n");

            string_append(switching_output,"JUMP ");
            string_append(switching_output,end_of_expression->array);
            string_append(switching_output,"\n");

            //-------------------------koniec expressinu pre rovnake floaty


            generate_label(switching_output,_2_ints_to_floats->array);//------------------------rovnakeeeeeeeeeeeeeeee
            string_append(switching_output, "JUMPIFEQ ");
            string_append(switching_output,"%error_label_0 ");
            append_token_variable_to_assembly(switching_output,assembly_1_token);// check
            string_append(switching_output," int@0\n");

            string_append(switching_output, "INT2FLOAT");
            append_token_variable_to_assembly(switching_output,assembly_1_token);
            append_token_variable_to_assembly(switching_output,assembly_1_token);
            string_append(switching_output,"\n");

            string_append(switching_output, "INT2FLOAT");
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            append_token_variable_to_assembly(switching_output,assembly_2_token);
            string_append(switching_output,"\n");
            string_append(switching_output,"DIV");
            append_token_operands_to_assembly(switching_output,result,assembly_1_token,assembly_2_token);
            string_append(switching_output,"\n");
            //-------------------------koniec expressionu pre rovnake inty


            break;
        case TTYPE_IDIV:

            // kontrola typov ci su len dva inty
            string_append(switching_output, "JUMPIFNEQ ");
            string_append(switching_output,"%error_label_semantic" );
            append_string_variable_to_assembly(switching_output, variable1->array);
            string_append(switching_output, " string@int\n");


            string_append(switching_output, "JUMPIFNEQ ");
            string_append(switching_output,"%error_label_semantic" );
            append_string_variable_to_assembly(switching_output, variable2->array);
            string_append(switching_output, " string@int\n");
            // kontrola typov ci su len dva inty



            // kontrola ci
            string_append(switching_output, "JUMPIFEQ ");
            string_append(switching_output,"%error_label_0 ");
            append_token_variable_to_assembly(switching_output,assembly_2_token);// check
            string_append(switching_output," int@0\n");


            string_append(switching_output, "IDIV ");
            append_token_operands_to_assembly(switching_output, result, assembly_1_token, assembly_2_token);
            break;

       default:
            fprintf(stderr, "THERE IS A PROBLEM\n");
            exit(1); // check error code

    }
    //--------------------------------------------------------vykonavanie expression-------------------------------------------------------------


    generate_label(switching_output,end_of_expression->array);
    identificator.label++;
    token_free(assembly_1_token);
    token_free(assembly_2_token);

    free(assembly_1_variable);
    free(assembly_2_variable);

    free(assembly_1_token);
    free(assembly_2_token);

    string_free(flag_1);
    string_free(flag_2);

    string_free(label_bool_int_float_1);
    string_free(label_bool_int_float_2);
    string_free(is_eq_1_int);
    string_free(is_eq_2_int);



    char *final_result = string_copy_data(result);

    string_free(label_int_dodge);
    string_free(end_of_expression);
    string_free(concatenation_label);
    string_free(result);
    string_free(result_eq_1);
    string_free(result_eq_2);
    string_free(variable1);
    string_free(variable2);
    string_free(temporary_div1);
    string_free(temporary_div2);

    string_free(label_LSOREQ);
    string_free(label_GTOREQ);
    string_free(label_ISEQ);
    string_free(label_LS);
    string_free(label_GT);
    string_free(label_ADD);
    string_free(label_SUB);
    string_free(label_MUL);
    string_free(label_DIV);
    string_free(label_IDIV);
    string_free(label_ISNEQ);
    string_free(int_to_float);
    string_free(_2_ints_to_floats);
    string_free(float_to_int);
    return final_result;
}

void generate_create_frame(){

    string_t * switchting_output = switch_frame(); // todo check
    string_append(switchting_output,"CREATEFRAME\n");
    identificator.param_call = 1 ;
}

void generate_function(token_t *id){

    //------------------------ generovanie definicie-----------------------------
    string_append(assembly_code.function_definitions, "\n\nLABEL !"); // can't use generate label because !
    string_append(assembly_code.function_definitions, id->attribute.string);
    string_append(assembly_code.function_definitions, "\n");

    string_append(assembly_code.function_definitions, "PUSHFRAME\n");
    string_append(assembly_code.function_definitions, "DEFVAR LF@%%return_value\n");
}

void generate_print(const char* label){

    //toto prerobit na generate_function a generate_call_function, tak aby mi do tych funkcii posielali iba string nie token

    string_append(assembly_code.print, "\nLABEL !");
    string_append(assembly_code.print, label);
    string_append(assembly_code.print, "\n");
    string_append(assembly_code.print, "PUSHFRAME\n");
    string_append(assembly_code.print, "DEFVAR LF@%%return_value\n");
    //label for potentional next parameters after None
    string_t* process_next_param = string_create_init();
    //variable to check type
    string_t *check_variable = string_create_init();
    //parameter obtained from function call
    string_t *parameter = string_create_init();
    //label to jump to print None
    string_t *none_label = string_create_init();

    //cycle that prints parametres all parametres of print function

    for(int i = identificator.param_def; i < identificator.param_call; i++){
        //we declare variable which we will use for storing type of passed variable
        create_unic_variable(check_variable, &identificator.general, "%check_type");
        string_append(assembly_code.print, "DEFVAR LF@");
        string_append(assembly_code.print, check_variable->array);
        string_append(assembly_code.print, "\n");

        //checking type from passed parameter, generating params according to called params
        create_unic_variable(parameter, &identificator.param_def, "%param");
        string_append(assembly_code.print, "TYPE LF@");
        string_append(assembly_code.print, check_variable->array);
        string_append(assembly_code.print, " LF@");
        string_append(assembly_code.print, parameter->array);
        string_append(assembly_code.print, "\n");

        //jumps when passed parameter is nil -> NONE_LABEL
        create_unic_variable(none_label, &identificator.label, "%none_label");
        string_append(assembly_code.print, "JUMPIFEQ ");
        string_append(assembly_code.print, none_label->array);
        string_append(assembly_code.print, " LF@");
        string_append(assembly_code.print,  check_variable->array);
        string_append(assembly_code.print, " string@nil\n");

        //parameter wasnt nil -> we can print it
        string_append(assembly_code.print, "WRITE ");
        string_append(assembly_code.print, " LF@");
        string_append(assembly_code.print, parameter->array);
        string_append(assembly_code.print, "\n");

        //we have to jump over the print of None
        create_unic_variable(process_next_param, &identificator.label, "%next_param");
        string_append(assembly_code.print, "JUMP ");
        string_append(assembly_code.print, process_next_param->array);
        string_append(assembly_code.print, "\n");

        //paramter was None -> jumped here
        string_append(assembly_code.print, "LABEL ");
        string_append(assembly_code.print, none_label->array);
        string_append(assembly_code.print, "\n");

        //print None
        string_append(assembly_code.print, "WRITE string@None\n");

        //end processing of first parameter
        string_append(assembly_code.print, "LABEL ");
        string_append(assembly_code.print, process_next_param->array);
        string_append(assembly_code.print, "\n");

        /* Ensures there is no space after the last param*/
        if (i < identificator.param_call - 1)
            string_append(assembly_code.print, "WRITE string@\\032\n");
    }
    //printing \n after printing all parameters
    string_append(assembly_code.print, "WRITE string@\\010\n");
    //print always returns nil
    string_append(assembly_code.print,"MOVE LF@%%return_value nil@nil\n");
    string_append(assembly_code.print, "POPFRAME \n");
    string_append(assembly_code.print, "RETURN \n");
    identificator.param_def = 1;

    string_free(process_next_param);
    string_free(check_variable);
    string_free(parameter);
    string_free(none_label);
}

void generate_function_end() {

    //--------------------- koniec definicie funkcie
    string_append(assembly_code.stash, "POPFRAME \n");
    string_append(assembly_code.stash, "RETURN \n");
    identificator.param_def = 1;
}


void generate_call_function(const char *id){


    // call
    string_t *switching_output = switch_frame();
    // if function is print
    if((strcmp(id, "print")) == 0){

        string_t *print_label = string_create_init();
        create_unic_variable(print_label, &identificator.general, "%print");
        string_append(switching_output, "CALL !");
        string_append(switching_output, print_label->array);
        string_append(switching_output, "\n");
        generate_print(print_label->array);
        string_free(print_label);
    }
    else{
        // call normal function
        string_append(switching_output, "CALL !");
        string_append(switching_output, id);
        string_append(switching_output, "\n");
    }
}





void generate_jumpeq_string_string(string_t * frame , const char * jump , string_t * label , string_t * variable1, string_t * variable2 ){
    string_append(frame, jump);
    string_append(frame, " ");
    string_append(frame, label->array);
    append_string_variable_to_assembly(frame,variable1->array);
    append_string_variable_to_assembly(frame,variable2->array);
    string_append(frame, "\n");

}
















void generate_def_param(token_t *id){

    // declaration of variable
    string_append(assembly_code.stash, "DEFVAR");
    //printing_frame_to_variable(function_definitions); -> redundant LF@
    append_string_variable_to_assembly(assembly_code.stash, id->attribute.string);
    string_append(assembly_code.stash, "\n");


    // createion of new variable
    string_t *parameter = string_create_init();
    create_unic_variable(parameter, &identificator.param_def, "%param");
    string_append(parameter, "\n");



    //moving value
    string_append(assembly_code.stash, "MOVE");
    append_string_variable_to_assembly(assembly_code.stash, id->attribute.string);
    append_string_variable_to_assembly(assembly_code.stash, parameter->array);
    string_append(assembly_code.stash, "\n");

    string_free(parameter);
}

void generate_call_param(token_t *id){
    // definition of frame where to adding code
    string_t *switching_output = switch_frame() ;


    //  DEFVAR OF NEW VARIABLE
    string_t *parameter = string_create_init();
    create_unic_variable(parameter, &identificator.param_call, "%param");

    string_append(switching_output, "DEFVAR TF@");
    string_append(switching_output, parameter->array);
    string_append(switching_output, "\n");


    // MOVING id to parameter
    string_append(switching_output, "MOVE TF@");
    string_append(switching_output, parameter->array);
    append_token_variable_to_assembly(switching_output, id);
    string_append(switching_output, "\n");

    //clean
    string_free(parameter);
}



void generate_while(token_t * expression){
// ?
    string_t * switching_output = switch_frame();
    string_t *switching_output_definitions = switch_definitions_frame();

    string_t *while_expression = define_uniq_variable(switching_output_definitions, &identificator.expression, "%while_expression");


    string_append(switching_output, "MOVE ");
    append_string_variable_to_assembly(switching_output, while_expression->array);
    string_append(switching_output, " ");
    append_token_variable_to_assembly(switching_output, expression);
    string_append(switching_output, "\n");

    string_t *while_int_label = string_create_init();
    string_t *while_float_label = string_create_init();
    string_t *while_bool_label = string_create_init();
    string_t *while_body_label = string_create_init();
    string_t *while_end_label = string_create_init();

    //uniq label for int comparison
    create_unic_label(while_int_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_int_label");
    //uniq label for float comparison
    create_unic_label(while_float_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_float_label");
    //uniq label for bool comparison
    create_unic_label(while_bool_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_bool_label");
    //uniq label for jumping to the body of while cycle
    create_unic_label(while_body_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_body_label");
    //uniq label for jumping at the end of the cycle
    create_unic_label(while_end_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_end_label");

    //variable which will store type of passed expression
    string_t *id_type = define_uniq_variable(switching_output_definitions, &identificator.expression, "%while_check_type");

    //get type of passed expression and store the type in string id_type
    get_type_variable(switching_output,id_type ,expression);

    string_append(switching_output, "\n#comparing if while expression is false/zero\n");
    //if its int jump to int comparison
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",while_int_label,id_type,"string@int");

    //if its float jump to float comparison
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",while_float_label,id_type,"string@float");

    //JUMPIFEQ bool_label id_type string@bool
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",while_bool_label,id_type,"string@bool");

    //while (string) or while (None) -> while end

    generate_jump(switching_output,"JUMP",while_end_label->array);


    //id_type was bool, now we compare if expression is false, first we generate bool label

    generate_label(switching_output,while_bool_label->array);

    //JUMPIFEQ @todo_while_end_label expression bool@false
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",while_end_label,while_expression,"bool@false");


    generate_jump(switching_output,"JUMP",while_body_label->array);

    //id_type was int, now we compare if expression is zero

    generate_label(switching_output,while_int_label->array);

    //JUMPIFEQ @todo_while_end_label expression int@0
    string_append(switching_output, "JUMPIFEQ ");
    string_append(switching_output, while_end_label->array);
    append_string_variable_to_assembly(switching_output,while_expression->array);
    string_append(switching_output, " int@0\n");


    generate_jump(switching_output,"JUMP",while_body_label->array);

    //id_type was float, now we compare if expression is zero
    generate_label(switching_output,while_float_label->array);

    //JUMPIFEQ @todo_while_end_label expression float@0x0.0p+0

    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",while_end_label,while_expression,"float@0x0.0p+0");
    string_append(switching_output, "\n#body of while\n");

    generate_label(switching_output,while_body_label->array);

    string_free(id_type);
    string_free(while_expression);
    string_free(while_int_label);
    string_free(while_float_label);
    string_free(while_bool_label);
    string_free(while_body_label);
    string_free(while_end_label);
}// check labels

void generate_while_label(){
    string_t *switching_output = switch_frame();

    /* First we have to push the current while label counter value */
    int *tmp_label_count = malloc(sizeof(int)); 
    if (!tmp_label_count) exit(ERROR_INTERNAL);
    *tmp_label_count = ++identificator.while_label;

    stack_general_push(general_stacks.while_labels_stack, // Push current label count
            (void *)tmp_label_count);

    string_t *while_beginning_label = string_create_init();

    create_unic_label(while_beginning_label, 
        (int *)(stack_general_top(general_stacks.while_labels_stack)->data),
        "%while_beginning_label");
    string_append(switching_output, "\n#while cycle\n");

    generate_label(switching_output,while_beginning_label->array);
    //free string
    string_free(while_beginning_label);
}

void generate_while_end(){
    string_t *switching_output = switch_frame();
    string_t *while_end_label = string_create_init();
    string_t *while_beginning_label = string_create_init();

    //while end label and beginning label has to be the same as in generate_while
    create_unic_label(while_end_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_end_label");
    create_unic_label(while_beginning_label, (int *)(stack_general_top(general_stacks.while_labels_stack)->data), "%while_beginning_label");


    generate_jump(switching_output,"JUMP",while_beginning_label->array);

    string_append(switching_output, "\n#end of while cycle\n");

    // generating label for while end label
    generate_label(switching_output,while_end_label->array);

    //increase the label uniq variable, so that in next while are going to be different labels

    /* Now pop the last added value of while label counter */
    stack_pop(general_stacks.while_labels_stack);

    string_free(while_end_label);
    string_free(while_beginning_label);
}

void generate_if(token_t *expression){
    string_t *switching_output = switch_frame();

    string_t *switching_output_definitions = switch_definitions_frame();

    /* First we have to push the current if label counter value */
    int *tmp_label_count = malloc(sizeof(int)); 
    if (!tmp_label_count) exit(ERROR_INTERNAL);
    *tmp_label_count = ++identificator.if_label;

    stack_general_push(general_stacks.if_labels_stack, // Push current label count
            (void *)tmp_label_count);

    //DEFVAR if_expression
    string_t *if_expression = define_uniq_variable( switching_output_definitions, &identificator.expression, "%if_expression");

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
    create_unic_label(if_int_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%if_int_label");
    //uniq label for float comparison
    create_unic_label(if_float_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%if_float_label");
    //uniq label for bool comparison
    create_unic_label(if_bool_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%if_bool_label");
    //uniq label for jumping to the body of while cycle
    create_unic_label(else_body_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%else_body_label");
    //uniq label for jumping at the end of the cycle
    create_unic_label(if_body_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%if_body_label");

    string_append(switching_output, "MOVE");
    append_string_variable_to_assembly(switching_output, if_expression->array);
    append_token_variable_to_assembly(switching_output, expression);
    string_append(switching_output, "\n");

    string_t *id_type = define_uniq_variable(switching_output_definitions, &identificator.expression, "%if_check_type");
    //get type of passed parameter
    get_type_variable(switching_output, id_type,expression);


    string_append(switching_output, "\n#checking if expression in if statement is false\n");
    //JUMPIFEQ int_label id_type string@int
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ", if_int_label,id_type, "string@int");

    //JUMPIFEQ float_label id_type string@float
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ", if_float_label,id_type, "string@float");

    //JUMPIFEQ bool_label id_type string@bool
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ", if_bool_label,id_type, "string@bool");

    //if (string) or if (None) -> jump to else
    generate_jump(switching_output,"JUMP",else_body_label->array);


    //id_type was bool, now we compare if expression is false
    generate_label(switching_output,if_bool_label->array);

    //JUMPIFEQ @else_label if expression == bool@false
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ", else_body_label, if_expression , "bool@false");

    generate_jump(switching_output,"JUMP",if_body_label->array);

    //id_type was int, now we compare if expression is zero
    generate_label(switching_output,if_int_label->array);

    //if expression is equal to zero jump to else body
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",else_body_label,if_expression,"int@0");


    //expression wasnt zero jump to if body
    generate_jump(switching_output,"JUMP",if_body_label->array );

    //id_type was float, now we compare if expression is zero
    generate_label(switching_output,if_float_label->array);

    //if expression is equal to zero jump to else body
    generate_jumpeq_string_char(switching_output,"JUMPIFEQ",else_body_label,if_expression,"float@0x0.0p+0");

    //expression was evaluated as true, continue in if
    generate_label(switching_output,if_body_label->array);

    string_append(switching_output, "\n#in if statement\n");

    string_free(id_type);
    string_free(if_expression);
    string_free(if_int_label);
    string_free(if_float_label);
    string_free(if_bool_label);
    string_free(else_body_label);
    string_free(if_body_label);
}

void generate_else(){
    string_t *switching_output = switch_frame();

    string_t *else_body_label = string_create_init();
    //uniq label for else if
    string_t *else_if_end_label = string_create_init();
    create_unic_label(else_if_end_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%else_if_end_label");
    //uniq label for jumping to the body of else
    create_unic_label(else_body_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%else_body_label");

    generate_jump(switching_output,"JUMP",else_if_end_label->array);


    string_append(switching_output, "\n#in else body\n");
    generate_label(switching_output,else_body_label->array);

    //uniq for if labels will be set to one
    string_free(else_body_label);
    string_free(else_if_end_label);
}

void generate_elseif_end(){
    string_t *switching_output =switch_frame();

    //else if end label
    string_t *else_if_end_label = string_create_init();
    create_unic_label(else_if_end_label, (int *)(stack_general_top(general_stacks.if_labels_stack)->data), "%else_if_end_label");

    generate_label(switching_output,else_if_end_label->array);

    stack_pop(general_stacks.if_labels_stack);
    string_free(else_if_end_label);
}

void generate_assign_to_retvalue(const char *return_result){
    string_t *switching_output =switch_frame();
    // move tf@x
    string_append(switching_output, "MOVE TF@%%return_value ");

    //move tf@x from
    append_frame_to_variable(switching_output);
    string_append(switching_output, return_result);
    string_append(switching_output, "\n");
}


void generate_assign_retvalue(const char *dest){
    string_t *switching_output = switch_frame();
    string_append(switching_output, "MOVE");

    append_string_variable_to_assembly(switching_output,dest);
    string_append(switching_output, " TF@%%return_value");
    string_append(switching_output, "\n");
}


void generate_assign(const char *destination, token_t *content){
    string_t *switching_output = switch_frame();

    string_append(switching_output, "MOVE");

    append_string_variable_to_assembly(switching_output, destination);
    append_token_variable_to_assembly(switching_output, content);
    string_append(switching_output, "\n");
}

void declaration_variable(token_t *variable){

    string_t *switching_output_definitions = switch_definitions_frame();

    string_append(switching_output_definitions, "DEFVAR");
    append_token_variable_to_assembly(switching_output_definitions, variable);
    string_append(switching_output_definitions, "\n");
}


int concat_main_stash() {
    int retvalue = SUCCESS;
    string_append(assembly_code.main, assembly_code.stash->array);
    string_clear(assembly_code.stash);
    return retvalue;
}

int concat_function_stash() {
    int retvalue = SUCCESS;
    string_append(assembly_code.function_definitions, assembly_code.stash->array);
    string_clear(assembly_code.stash);
    return retvalue;
}
