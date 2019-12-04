#include "scanner.h"
#include "parser.h"
#include "errors.h"
#include "generate_ver2.h"
#include <stdio.h>
#include <stdlib.h>



generate_strings_output_t assembly_code;
uniq_id_t identificator;



int generate_strings_input_init(){

    assembly_code.stash = string_create_init();
    assembly_code.errors = string_create_init();
    assembly_code.function_definitions = string_create_init();
    assembly_code.main = string_create_init();
    //checks alloc
    if( assembly_code.stash == NULL ||
        assembly_code.errors == NULL ||
        assembly_code.function_definitions == NULL ||
        assembly_code.main == NULL  
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
        string_append(assembly_code.errors, "EXIT int@4\n");



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
        "SUB LF@str_len LF@str_len int@1\n"
        "LABEL &len&cycle\n"
        "GETCHAR LF@actual_char LF@actual_string LF@increment\n"
        "JUMPIFEQ &len&end LF@increment LF@str_len\n"
        "CONCAT LF@%%return_value LF@%%return_value LF@actual_char\n"
        "ADD LF@increment LF@increment int@1\n"
        "JUMP &len&cycle\n"
        "LABEL &len&end\n"
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
        return ALLOC_ERROR;
    }
    generate_assembly_start();
    return SUCCESS;
}

void free_assembly_code() {
    string_free(assembly_code.main);
    string_free(assembly_code.stash);
    string_free(assembly_code.function_definitions);
    string_free(assembly_code.errors);
}


void end_program() {
    string_append(assembly_code.function_definitions, assembly_code.main->array);
    string_append(assembly_code.function_definitions, assembly_code.errors->array);
    printf("%s", assembly_code.function_definitions->array);
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
    append_frame_to_variable(frame);
    string_append(frame,variable->attribute.string);
}

void append_string_variable_to_assembly(string_t *frame, const char *variable){
    string_append(frame, " ");
    append_frame_to_variable(frame);
    string_append(frame, variable);
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

string_t* define_uniq_variable(string_t *frame, size_t *uniq, char *name){
    
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
    append_string_variable_to_assembly(frame, type);
    string_append(frame, "\n");
}

//void generate_error_labels(){return;};
//void convert_float_to_int(string_t* frame, token_t* operand);

string_t * switch_frame(){
    if (in_function) {
        return  assembly_code.function_definitions;
    } 
    else {
        return assembly_code.main;
    }
}



char *generate_expression(token_t *operand1, token_t *operator, token_t *operand2){
    string_t *switching_output = switch_frame();
    string_t *switching_output_definitions = switch_definitions_frame();

    // -----------------------------------------------------------definice DEFVAR--------------------------------------------------------------
    string_t  * temporary_div1 = define_uniq_variable(switching_output_definitions, &identificator.general, "temporary_div");
    string_t  * temporary_div2 = define_uniq_variable(switching_output_definitions, &identificator.general, "temporary_div");

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
    // -----------------------------------------------------------definice DEFVAR--------------------------------------------------------------

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
    get_type_variable(variable1, switching_output, operand1);

    // type of second operand
    get_type_variable(variable2, switching_output, operand2);

    // types of operands are not equal -> error
    string_append(switching_output, "JUMPIFNEQ %error_label_semantic");
    append_string_variable_to_assembly(switching_output, variable1->array);
    append_string_variable_to_assembly(switching_output, variable2->array);
    string_append(switching_output, "\n");

    // if type is string@nil
    string_append(switching_output, "JUMPIFEQ %error_label_semantic");
    append_string_variable_to_assembly(switching_output, variable1->array);
    string_append(switching_output, " string@nil");
    string_append(switching_output, "\n");


    // if type is nil@nil
    string_append(switching_output, "JUMPIFEQ %error_label_semantic");
    append_string_variable_to_assembly(switching_output, variable1->array);
    string_append(switching_output, " nil@nil");
    string_append(switching_output, "\n");

    // -------------------------------------------------------komparacia typov--------------------------------------------------------------------

    //--------------------------------------------------------vykonavanie expression-------------------------------------------------------------
    switch (operator->type) {

        case TTYPE_LSOREQ:
            //check if operand1 < operand2
            string_append(switching_output, "LS");
            append_token_operands_to_assembly(switching_output, result_eq_1, operand1, operand2);
            //check if operand1 == operand2
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result_eq_2, operand1, operand2);
            //(op1 < op2) or (op1 == op2) => operand1 <= operand2
            string_append(switching_output, "OR");
            append_string_operands_to_assembly(switching_output, result, result_eq_1, result_eq_2);

            break;
        case TTYPE_GTOREQ:
            //check if operand1 < operand2
            string_append(switching_output, "GT");
            append_token_operands_to_assembly(switching_output, result_eq_1, operand1, operand2);
            //check if operand1 == operand2
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result_eq_2, operand1, operand2);
            //(op1 < op2) or (op1 == op2) => operand1 <= operand2
            string_append(switching_output, "OR");
            append_string_operands_to_assembly(switching_output, result, result_eq_1, result_eq_2);
            break;
        case TTYPE_ISEQ:
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            break;
        case TTYPE_LS:
            string_append(switching_output, "LT");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            break;
        case TTYPE_GT:
            string_append(switching_output, "GT");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            break;
        case TTYPE_ADD:

            // checking if bool
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            // if string jump to concatenation
            string_append(switching_output, "JUMPIFEQ ");
            string_append(switching_output, concatenation_label->array);
            append_string_variable_to_assembly(switching_output, variable1->array);
            string_append(switching_output, " string@string\n");

            // + float or int
            string_append(switching_output, "ADD");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            generate_jump(switching_output,"JUMP",end_of_expression->array);


            // Label Concat
            string_append(switching_output, "\nLABEL ");
            string_append(switching_output, concatenation_label->array);

            // concat string + string
            string_append(switching_output, "\nCONCAT");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            identificator.concat_label +=1;
            break;
        case TTYPE_SUB:
            // string - string is bad
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            // bool - bool is bad
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            string_append(switching_output, "SUB");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            break;
        case TTYPE_MUL:
            // string * string is bad
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            // bool * bool is bad
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");
            string_append(switching_output, "MUL");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            break;
        case TTYPE_DIV:
            //  string / string is bad
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            // bool / bool is bad
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            // if float jump to label float
            check_if_op_type_eq(switching_output, variable1->array, "string@float", label_int_dodge->array);


            // so just int passed
            // can't call check if op type eq because int@0 doesnt have LF or GF
            string_append(switching_output, "JUMPIFEQ ");
            string_append(switching_output,"%error_label_0 ");
            append_token_variable_to_assembly(switching_output,operand1);
            string_append(switching_output," int@0\n");

            // now we have to convert our  int operands to float operands

            string_append(switching_output,"INT2FLOAT");
            append_string_variable_to_assembly(switching_output,temporary_div1->array);
            append_token_variable_to_assembly(switching_output,operand1);
            string_append(switching_output,"\n");


            string_append(switching_output,"INT2FLOAT");
            append_string_variable_to_assembly(switching_output,temporary_div2->array);
            append_token_variable_to_assembly(switching_output,operand2);
            string_append(switching_output,"\n");


            //delenie z int to float , potom jump na koniec expressionu

            string_append(switching_output,"DIV");
            append_string_operands_to_assembly(switching_output,result,temporary_div1,temporary_div2);
            string_append(switching_output,"\n");

            string_append(switching_output,"JUMP ");
            string_append(switching_output,end_of_expression->array);
            string_append(switching_output,"\n");



            //float label
            generate_label(switching_output,label_int_dodge->array);

            //delenie nulou pre float
            string_append(switching_output, "\nJUMPIFEQ ");
            string_append(switching_output,"%error_label_0 ");
            append_token_variable_to_assembly(switching_output,operand1);
            string_append(switching_output," float@0x0.0p+0\n");

            string_append(switching_output,"DIV");
            append_token_operands_to_assembly(switching_output,result,operand1,operand2);
            string_append(switching_output,"\n");
            break;
        case TTYPE_IDIV:
            check_if_op_type_eq(switching_output, variable1->array, "string@string", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@float", "%error_label_semantic");
            check_if_op_type_eq(switching_output, variable1->array, "string@bool", "%error_label_semantic");

            string_append(switching_output, "IDIV ");
            append_token_operands_to_assembly(switching_output, result, operand1, operand2);
            break;
       case TTYPE_ISNEQ:
            string_append(switching_output, "EQ");
            append_token_operands_to_assembly(switching_output, result_eq_1, operand1, operand2);

            string_append(switching_output, "EQ");
            append_string_variable_to_assembly(switching_output,result->array);
            append_string_variable_to_assembly(switching_output,result_eq_1->array);
            string_append(switching_output,"bool@false\n");
            break;
       default:
            fprintf(stderr, "THERE IS A PROBLEM\n");
            exit(1); // check error code

    }
    //--------------------------------------------------------vykonavanie expression-------------------------------------------------------------


    generate_label(switching_output,end_of_expression->array);

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
    return final_result;
}

void generate_create_frame(){


    string_t * switchting_output = switch_definitions_frame(); // todo check
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

};

void generate_print(const char* label){

    //toto prerobit na generate_function a generate_call_function, tak aby mi do tych funkcii posielali iba string nie token ? WHAT?
    //toto prerobit na generate_function a generate_call_function, tak aby mi do tych funkcii posielali iba string nie token
    string_append(assembly_code.function_definitions, "\nLABEL !");
    string_append(assembly_code.function_definitions, label);
    string_append(assembly_code.function_definitions, "\n");
    string_append(assembly_code.function_definitions, "PUSHFRAME\n");
    string_append(assembly_code.function_definitions, "DEFVAR LF@%%return_value\n");
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
        string_append(assembly_code.function_definitions, "DEFVAR LF@");
        string_append(assembly_code.function_definitions, check_variable->array);
        string_append(assembly_code.function_definitions, "\n");

        //checking type from passed parameter, generating params according to called params
        create_unic_variable(parameter, &identificator.param_def, "%param");
        string_append(assembly_code.function_definitions, "TYPE LF@");
        string_append(assembly_code.function_definitions, check_variable->array);
        string_append(assembly_code.function_definitions, " LF@");
        string_append(assembly_code.function_definitions, parameter->array);
        string_append(assembly_code.function_definitions, "\n");

        //jumps when passed parameter is nil -> NONE_LABEL
        create_unic_variable(none_label, &identificator.label, "%none_label");
        string_append(assembly_code.function_definitions, "JUMPIFEQ ");
        string_append(assembly_code.function_definitions, none_label->array);
        string_append(assembly_code.function_definitions, " LF@");
        string_append(assembly_code.function_definitions,  check_variable->array);
        string_append(assembly_code.function_definitions, " string@nil\n");

        //parameter wasnt nil -> we can print it
        string_append(assembly_code.function_definitions, "WRITE ");
        string_append(assembly_code.function_definitions, " LF@");
        string_append(assembly_code.function_definitions, parameter->array);
        string_append(assembly_code.function_definitions, "\n");

        //we have to jump over the print of None
        create_unic_variable(process_next_param, &identificator.label, "%next_param");
        string_append(assembly_code.function_definitions, "JUMP ");
        string_append(assembly_code.function_definitions, process_next_param->array);
        string_append(assembly_code.function_definitions, "\n");

        //paramter was None -> jumped here
        string_append(assembly_code.function_definitions, "LABEL ");
        string_append(assembly_code.function_definitions, none_label->array);
        string_append(assembly_code.function_definitions, "\n");

        //print None
        string_append(assembly_code.function_definitions, "WRITE string@None\n");

        //end processing of first parameter
        string_append(assembly_code.function_definitions, "LABEL ");
        string_append(assembly_code.function_definitions, process_next_param->array);
        string_append(assembly_code.function_definitions, "\n");

        /* Ensures there is no space after the last param*/
        if (i < identificator.param_call - 1)
            string_append(assembly_code.function_definitions, "WRITE string@\\032\n");
    }
    //printing \n after printing all parameters
    string_append(assembly_code.function_definitions, "WRITE string@\\010\n");
    //print always returns nil
    string_append(assembly_code.function_definitions,"MOVE LF@%%return_value nil@nil\n");
    generate_function_end();

    string_free(process_next_param);
    string_free(check_variable);
    string_free(parameter);
    string_free(none_label);
}

void generate_function_end() {

    //--------------------- koniec definicie funkcie
    string_append(assembly_code.function_definitions, "POPFRAME \n");
    string_append(assembly_code.function_definitions, "RETURN \n");
    identificator.param_def = 1;
}


void generate_call_function(const char *id){


    // call
    string_t *switching_output = switch_definitions_frame();
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

};


void generate_jumpeq_string_char(string_t * frame , const char * jump , string_t * label , string_t * variable, const char * type ){
    string_append(frame, jump);
    string_append(frame, " ");
    string_append(frame, label->array);
    append_string_variable_to_assembly(frame,variable->array);
    string_append(frame, " ");
    string_append(frame, type);
    string_append(frame, "\n");

}



void generate_def_param(token_t *id){

    // declaration of variable
    string_append(assembly_code.function_definitions, "DEFVAR");
    //printing_frame_to_variable(function_definitions); -> redundant LF@
    append_string_variable_to_assembly(assembly_code.function_definitions, id->attribute.string);
    string_append(assembly_code.function_definitions, "\n");


    // createion of new variable
    string_t *parameter = string_create_init();
    create_unic_variable(parameter, &identificator.param_def, "%param");
    string_append(parameter, "\n");



    //moving value
    string_append(assembly_code.function_definitions, "MOVE");
    append_string_variable_to_assembly(assembly_code.function_definitions, id->attribute.string);
    append_string_variable_to_assembly(assembly_code.function_definitions, parameter->array);
    string_append(assembly_code.function_definitions, "\n");

    string_free(parameter);
};

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
};



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
    create_unic_label(while_int_label, &identificator.while_label, "%while_int_label");
    //uniq label for float comparison
    create_unic_label(while_float_label, &identificator.while_label, "%while_float_label");
    //uniq label for bool comparison
    create_unic_label(while_bool_label, &identificator.while_label, "%while_bool_label");
    //uniq label for jumping to the body of while cycle
    create_unic_label(while_body_label, &identificator.while_label, "%while_body_label");
    //uniq label for jumping at the end of the cycle
    create_unic_label(while_end_label, &identificator.while_label, "%while_end_label");

    //variable which will store type of passed expression
    string_t *id_type = define_uniq_variable(switching_output_definitions, &identificator.expression, "%while_check_type");

    //get type of passed expression and store the type in string id_type
    get_type_variable(id_type, switching_output, expression);

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
};// check labels

void generate_while_label(token_t *expression){
    string_t *switching_output = switch_frame();

    identificator.while_label_cnt++;


    string_t *while_beginning_label = string_create_init();
    create_unic_label(while_beginning_label, &identificator.while_label, "%while_beginning_label");
    string_append(switching_output, "\n#while cycle\n");

    if (identificator.while_label_cnt == 1) {
        identificator.while_label_pos = switching_output->index;
    }


    generate_label(switching_output,while_beginning_label->array);
    //free string
    string_free(while_beginning_label);
}

void generate_while_end(){
    string_t *switching_output = switch_frame();
    string_t *while_end_label = string_create_init();
    string_t *while_beginning_label = string_create_init();

    //while end label and beginning label has to be the same as in generate_while
    create_unic_label(while_end_label, &identificator.while_label, "%while_end_label");
    create_unic_label(while_beginning_label, &identificator.while_label, "%while_beginning_label");


    generate_jump(switching_output,"JUMP",while_beginning_label->array);

    string_append(switching_output, "\n#end of while cycle\n");

    // generating label for while end label
    generate_label(switching_output,while_end_label->array);

    //increase the label uniq variable, so that in next while are going to be different labels
    identificator.while_label += 1;



    /* mystery
    if (identificator.while_label_cnt == 1) {
        if (insert_definitions(switching_output, defvars)) {
            //return 99;
            exit(99);
        }
        string_clear(defvars);
        while_label_pos = 0;
    }
    while_label_cnt--;
    */
    string_free(while_end_label);
    string_free(while_beginning_label);


};

void generate_if(token_t *expression){
    string_t *switching_output = switch_frame();

    string_t *switching_output_definitions = switch_definitions_frame();

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
    create_unic_label(if_int_label, &identificator.if_label, "%if_int_label");
    //uniq label for float comparison
    create_unic_label(if_float_label, &identificator.if_label, "%if_float_label");
    //uniq label for bool comparison
    create_unic_label(if_bool_label, &identificator.if_label, "%if_bool_label");
    //uniq label for jumping to the body of while cycle
    create_unic_label(else_body_label, &identificator.if_label, "%else_body_label");
    //uniq label for jumping at the end of the cycle
    create_unic_label(if_body_label, &identificator.if_label, "%if_body_label");

    string_append(switching_output, "MOVE");
    append_string_variable_to_assembly(switching_output, if_expression->array);
    append_token_variable_to_assembly(switching_output, expression);
    string_append(switching_output, "\n");

    string_t *id_type = define_uniq_variable(switching_output_definitions, &identificator.expression, "%if_check_type");
    //get type of passed parameter
    get_type_variable(id_type, switching_output, expression);


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



};

void generate_else(){
    string_t *switching_output = switch_frame();

    string_t *else_body_label = string_create_init();
    //uniq label for else if
    string_t *else_if_end_label = string_create_init();
    create_unic_label(else_if_end_label, &identificator.if_label, "%else_if_end_label");
    //uniq label for jumping to the body of else
    create_unic_label(else_body_label, &identificator.if_label, "%else_body_label");

    generate_jump(switching_output,"JUMP",else_if_end_label->array);


    string_append(switching_output, "\n#in else body\n");
    generate_label(switching_output,else_body_label->array);

    //uniq for if labels will be set to one
    string_free(else_body_label);
    string_free(else_if_end_label);



};

void generate_elseif_end(){
    string_t *switching_output =switch_frame();

    //else if end label
    string_t *else_if_end_label = string_create_init();
    create_unic_label(else_if_end_label, &identificator.if_label, "%else_if_end_label");

    generate_label(switching_output,else_if_end_label->array);

    identificator.if_label+=1;
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


};


void generate_assign_retvalue(const char *dest){
    string_t *switching_output = switch_frame();
    string_append(switching_output, "MOVE");

    append_string_variable_to_assembly(switching_output,dest);
    string_append(switching_output, " TF@%%return_value");
    string_append(switching_output, "\n");
};


void generate_assign(const char *destination, token_t *content){
    string_t *switching_output = switch_frame();

    string_append(switching_output, "MOVE");

    append_string_variable_to_assembly(switching_output, destination);
    append_token_variable_to_assembly(switching_output, content);
    string_append(switching_output, "\n");


};

void declaration_variable(token_t *variable){

    string_t *switching_output = switch_frame();

    /*
    if (while_label_cnt > 0) {
        switching_output = defvars;
    } else {
        if (in_function) {
            switching_output = function_definitions;
        } else {
            switching_output = output_code;
        }
    }
    */
    string_append(switching_output, "DEFVAR");
    append_token_variable_to_assembly(switching_output, variable);
    string_append(switching_output, "\n");

}

