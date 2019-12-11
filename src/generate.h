/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   generate_ver2.h
 * @author Martin Osvald - xosval03, Marek Ziska - xziska03
 * @brief A header file for the code generator module.
 */

#ifndef __GENERATE_2_H__
#define __GENERATE_2_H__


#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "dynamic_string.h"
#include "scanner.h"
#include "general_stack.h"
#include "parser.h"
#include "errors.h"


// structure for implementing generator for output
typedef struct generate_strings_output{
    string_t *main;
    string_t *function_definitions;
    string_t *errors;
    string_t * stash;
    string_t * print;
} generate_strings_output_t;


// structure for iterating/generating unique variables/labels id 
typedef struct unique_id{
    int general  ;
    int param_call ;
    int param_def ;
    int expression ;
    int general_label ;
    int while_label ;
    int if_label ;
    int concat_label ;
    int label;
} unique_id_t;

typedef struct labels_stacks {
    stack_general_t * while_labels_stack;
    stack_general_t * if_labels_stack;
}  labels_stacks_t;

/**
 * @brief adding code for convert 2 ints to float
 * @param frame for string, label int to float dogde , type variable 1 , for dodge int_to_float_label , token1 tokent2 , boolean for checking zero , and flags to say if they convert
 * @return bool
 */
void convert_ints_to_floats( string_t * frame, string_t * int_to_float, string_t *variable1,
                             token_t *assembly_1_token,token_t * assembly_2_token ,
                             string_t * jump_destination_label, bool is_div_zero, string_t * flag1, string_t * flag2 );



/**
 * @brief for every string in generate strings output call string_init
 * @param
 * @return success
 */
int generate_strings_input_init();


/**
* @brief call this function for a start of generating assembly,
* @return  SUCCESS if allocation was successfull
*          ALLOCATION_ERROR if wasn't
*/

int start_program();

/**
* @brief function to declare to witch output/frame you should append string  for defwars and while
* @return
*/
string_t * switch_definitions_frame();

/**
* @brief function for ending generating program ,it will print assembly code
*/
void end_program();

/**
 * @brief free all strings in assembly code
 */
void free_assembly_code();

/**
 * @brief add token variable to frame
 * @param frame , tokent * variable
 */
void append_token_variable_to_assembly(string_t * frame , token_t * variable);


/**
 * @brief add string variable to frame
 * @param frame , string * variable
 */
void append_string_variable_to_assembly(string_t *frame, const char *variable) ;

/**
 * @brief call this function in expression for operation like ADD DIV , params are tokens as a operands , result is string
 * @param frame
 * @param result
 * @param operand1
 * @param operand2
 */
void append_token_operands_to_assembly(string_t *frame, string_t *result, token_t *operand1, token_t *operand2);


/**
 * @ call this function in expression for operation like ADD DIV , params are strings as a operands  , result is string
 * @param frame
 * @param result
 * @param operand1
 * @param operand2
 */
void append_string_operands_to_assembly(string_t *frame, string_t *result, string_t *operand1, string_t *operand2);

//
/**
 * @return is returning string from assembly_code
 */
string_t * switch_frame();




//
/**
 * it generates the expression for every operation
 * @param operand2
 * @param operator
 * @param operand1
 * @return
 */
char *generate_expression(token_t *operand2, token_t *operator, token_t *operand1);


/**
 * @briefis adding CREATEFRAME to the code
 */
void generate_create_frame();

/**
 *@brief is adding popframe and return to function
 */
void generate_function_end() ;


/**
 * @brief generating the label function , pushframe and the returnvalue variable
 */
void generate_function(token_t *id);

/**
 * @brief custom solution , generate as many labels as paramters and print every parametes
 * @param label
 */
void generate_print(const char * label );


/**
 * @brief adding CALL ! to assembly code
 */
void generate_call_function(const char *id);

/**
 * @brief generating DEFVAR to new paramter and calling move
 */

void generate_def_param(token_t *id);

/**
 * @brief generating variable while calling function
 */
void generate_call_param(token_t *id);


/**
 * @brief generating while with result expression
 */
void generate_while(token_t * expression);


/**
 * @brief generate label for while start
 */
void generate_while_label();


/**
 * @brief generate label for end
 */
void generate_while_end();



/**
 * @brief generate if label  for start with expression
 */
void generate_if(token_t *expression);

/**
 * @brief generate else after if
 */

void generate_else();


/**
 * @brief generate label  after if and end
 */
void generate_elseif_end();



/**
 * generate assing from function return result to return_vales
 * @param return_result
 */
void generate_assign_to_retvalue(const char *return_result);



/**
 * @brief generate jumpifeq label variable1 variable2 from strings types
 * @param frame
 * @param jump
 * @param label
 * @param variable1
 * @param variable2
 */
void generate_jumpeq_string_string(string_t * frame , const char * jump , string_t * label , string_t * variable1, string_t * variable2 );


/**
 * @brief generate jumpifeq label variable1 variable2 from tokens types
 * @param frame
 * @param jump
 * @param label
 * @param variable1
 * @param variable2
 */
void generate_jumpeq_string_char(string_t * frame , const char * jump , string_t * label , string_t * variable, const char * type );


/**
 * Generate assign result to destination
 * @param dest
 */
void generate_assign_retvalue(const char *dest);



/**
 * Generate JUMP label
 * @param frame
 * @param jump_instruction
 * @param destination_label
 */
void generate_jump(string_t * frame,const char * jump_instruction , const char * destination_label );


/**
 * Generate label declaration
 * @param frame
 * @param label
 */
void generate_label(string_t * frame,const char * label);

/**
 * Generate MOVE destination , content
 * @param frame
 * @param label
 */
void generate_assign(const char *destination, token_t *content);


/**
 * DEFVAR for variable
 * @param frame
 * @param label
 */
void declaration_variable(token_t *variable);

/**
 * These two functions take the content of the stash code string, and
 * append it to the main or the function code string. Then, they clear the stash.
 */

/**
 * Appending stash to main
 * @return
 */
int concat_main_stash();

/**
 * Appending stash to function
 * @return
 */
int concat_function_stash();


#endif //GENERATE_2_H


