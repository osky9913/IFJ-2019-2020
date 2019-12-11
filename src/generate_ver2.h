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
#include "dynamic_string.h"
#include "scanner.h"
#include "general_stack.h"


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


void convert_ints_to_floats( string_t * frame, string_t * int_to_float, string_t *variable1,
                             token_t *assembly_1_token,token_t * assembly_2_token ,
                             string_t * jump_destination_label, bool is_div_zero, string_t * flag1, string_t * flag2 );


// for every string in generate strings output call string_init
int generate_strings_input_init();

/* call this function for a start of generating assembly,
   return SUCCESS if allocation was successfull
   return ALLOCATION_ERROR if wasn't
*/
int start_program();

string_t * switch_definitions_frame();

// function for  ending generating program ,it will print assembly code 
void end_program();

// free all strings in assembly code 
void free_assembly_code();
// add token variable to frame
void append_token_variable_to_assembly(string_t * frame , token_t * variable);
// add string variable to frame
void append_string_variable_to_assembly(string_t *frame, const char *variable) ;


// call this function in expression for operation like ADD DIV , params are tokens as a operands , result is string
void append_token_operands_to_assembly(string_t *frame, string_t *result, token_t *operand1, token_t *operand2);


// call this function in expression for operation like ADD DIV , params are strings as a operands  , result is string
void append_string_operands_to_assembly(string_t *frame, string_t *result, string_t *operand1, string_t *operand2);

// is returning string from assembly_code
string_t * switch_frame();




//  it generates the expression for every operation
char *generate_expression(token_t *operand2, token_t *operator, token_t *operand1);



// is adding CREATEFRAME to the code
void generate_create_frame();

// is adding popframe and return to function
void generate_function_end() ;


// generating the label , pushframe and the returnvalue variable
void generate_function(token_t *id);

// ???
void generate_print(const char * label );


// adding CALL ! to assembly code
void generate_call_function(const char *id);

// generating DEFVAR to new paramter and calling move
void generate_def_param(token_t *id);

//
void generate_call_param(token_t *id);



void generate_while(token_t * expression);

void generate_while_label();

void generate_while_end();

void generate_if(token_t *expression);
void generate_else();

void generate_elseif_end();

void generate_assign_to_retvalue(const char *return_result);

void generate_jumpeq_string_string(string_t * frame , const char * jump , string_t * label , string_t * variable1, string_t * variable2 );

void generate_jumpeq_string_char(string_t * frame , const char * jump , string_t * label , string_t * variable, const char * type );

void generate_assign_retvalue(const char *dest);

void generate_jump(string_t * frame,const char * jump_instruction , const char * destination_label );

void generate_label(string_t * frame,const char * label);


void generate_assign(const char *destination, token_t *content);

void declaration_variable(token_t *variable);

/**
 * These two functions take the content of the stash code string, and
 * append it to the main or the function code string. Then, they clear the stash.
 */
int concat_main_stash();
int concat_function_stash();


#endif //GENERATE_2_H


