/**
 * @file scanner.c
 * @brief This module implements all parts of lexical analysis.
 * @author xziska03
 */

#include "scanner.h"
#include "errors.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>


//global variable to determine if we are on new line(time to evaluate indent/dedent)
//  -1 -> when beginning of the file
//   0 -> when LA found token thats not EOL
//   1 -> when LA found token that is EOL
int new_line = -1;

//global variable do determine if there are more dedent tokens to be generated
int indents_to_pop = 0; 

//actual line indentation 
int indent = 0;


int calculate_dent(FILE* f, int* c){
    indent = 0;
    stack_general_item_t* stack_top = NULL;
    //ignore newlines indentation
    if(*c == '\n'){
        stack_top = stack_general_top(dent_stack);
        return *(int*)stack_top->data;
    }
    while(isspace(*c)){
        if(*c == '\n'){
            indent = 0;
            *c = getc(f);
        }
        else{
            indent++;
            *c = getc(f);
        }
    }
    //calculated indentation of comments, have to ignore it
    if(*c == '#'){
        ungetc(*c, f);
        stack_top = stack_general_top(dent_stack);
        return *(int*)stack_top->data;
    }
    if(*c == '"'){
        ungetc(*c, f);
        stack_top = stack_general_top(dent_stack);
        return *(int*)stack_top->data;
    }
    return indent;
}

int finish_free_resources(int exit_code, token_t* token, string_t* tmp, string_t* token_string){
    if(token->type == TTYPE_STR || token->type == TTYPE_DOCSTR || token->type == TTYPE_ID){
        token->attribute.string = string_copy_data(token_string);
    }
    if(token->type == TTYPE_EOL){
        new_line = 1;
    } else {
        new_line = 0;
    }
    string_free(token_string);
    string_free(tmp);

    return exit_code;
}

void hexa_escape(FILE* f, string_t* token_string){
    long hexa;
    string_t* tmp = string_create_init();
    int c;
    c = getc(f);
    if(isxdigit(c)){
        string_append_char(tmp, '0');
        string_append_char(tmp, 'x');
        string_append_char(tmp, c);
        c = getc(f);
        if(isxdigit(c)){
            string_append_char(tmp, c);
            hexa = strtol(tmp->array, NULL, 16);
            string_append_char(token_string, hexa);
        }
        else{
            ungetc(c, f);
            hexa = strtol(tmp->array, NULL, 16);
            string_append_char(token_string, hexa);
        }
    }
    else{
        string_append_char(token_string, '\\');
        ungetc(c, f);
    }

    string_free(tmp);                                   
}

int process_dedents(){
    int pop_indent;     //indentation sequence that will be removed from indent_stack
    stack_general_item_t* stack_top = stack_general_top(dent_stack);
    int stack_top_int = *(int*)stack_top->data;

    while(indent != stack_top_int){
        pop_indent = stack_top_int;//indent_stack_top(dent_stack);
        stack_pop(dent_stack);

        if(stack_empty(dent_stack)){
            fprintf(stderr, "Indentation error: Indentation in commands sequence was not correct!\n");
            return 0;
        }
        stack_top = stack_general_top(dent_stack);
        stack_top_int = *(int*)stack_top->data;
        //after first indent was popped, dedent indentation was equal to top of stack(there is no more indents to be popped)
        if(indent == stack_top_int){
            indents_to_pop = 0;
            return 1;
        }
        //indentation of dedent is smaller than one or more indents indentation(there is more indents to be popped from stack)
        if(pop_indent != stack_top_int){
            indents_to_pop = 1;
            return 1;
        }
    }
    return 0;
}

int get_token(FILE* f, token_t* token){
    int state = 0;
    int c;
    string_t* token_string = string_create_init();
    string_t* tmp = string_create_init();
    stack_general_item_t* stack_top = NULL;
    int stack_top_int;

    //at the beginning of the file put zero on top of stack
    if(new_line == -1){
        stack_general_push_int(dent_stack, 0);
    }

    //there is more dedents to be generated
    if(indents_to_pop){
        token->type = TTYPE_DEDENT;

        if(process_dedents()){
            return finish_free_resources(SUCCESS, token, tmp, token_string);
        }
        else{
            indents_to_pop = 0; //len kvoli tomu aby sa to nezacykliklo, odstranit z finalnej verzie
            return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
        }
    }

    while(1){
        c = getc(f); 
        switch(state){
            //default state
            case 0:
                //indent dedent
                if(new_line == 1){
                    indent = calculate_dent(f, &c);
                    stack_top = stack_general_top(dent_stack);
                    if(stack_top){
                        stack_top_int = *(int*)stack_top->data;
                    }
                    else{
                        stack_top_int = -1;
                    }

                    if(indent > stack_top_int){
                        //processed non-whitespace character we have to get it back
                        ungetc(c, f);
                        stack_general_push_int(dent_stack, indent);
                        token->type = TTYPE_INDENT;
                        return finish_free_resources(SUCCESS, token, tmp, token_string); 
                    }
                    else if(indent < stack_top_int){
                        //processed non-whitespace character we have to get it back
                        ungetc(c, f);
                        token->type = TTYPE_DEDENT;

                        if(process_dedents()){
                            return finish_free_resources(SUCCESS, token, tmp, token_string);
                        }
                        else{
                            return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                        }
                    }
                }
                switch(c){
                    //comments
                    case '#':
                        state = 1;
                        break;
                    case '"':
                        string_append_char(tmp, c);
                        state = 2;
                        break;
                    //operators
                    case '+':
                        token->type = TTYPE_ADD;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case '-':
                        token->type = TTYPE_SUB;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case '*':
                        token->type = TTYPE_MUL;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case '/':
                        string_append_char(tmp, c);
                        token->type = TTYPE_DIV;
                        state = 4;
                        break;
                    case '<':
                        string_append_char(tmp, c);
                        token->type = TTYPE_LS;
                        state = 4;
                        break;
                    case '>':
                        string_append_char(tmp, c);
                        token->type = TTYPE_GT;
                        state = 4;
                        break;
                    case '=':
                        string_append_char(tmp, c);
                        token->type = TTYPE_ASSIGN;
                        state = 4;
                        break;
                    case '!':
                        string_append_char(tmp, c);
                        state = 4;
                        break;
                    case '(':
                        token->type = TTYPE_LTBRAC;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case ')':
                        token->type = TTYPE_RTBRAC;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case '\'':
                        token->type = TTYPE_STR;
                        state = 7;
                        break;
                    case EOF:
                        token->type = TTYPE_EOF;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case ',':
                        token->type = TTYPE_COMMA;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    case '\n':
                        if(new_line){
                            state = 0;
                        }
                        else{
                            token->type = TTYPE_EOL;
                            return finish_free_resources(SUCCESS, token, tmp, token_string);
                        }
                        break;

                    case ':':
                        token->type = TTYPE_COLUMN;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    default:
                        //identifiers
                        if(isalpha(c) || c == '_'){
                            token->type = TTYPE_ID;
                            string_append_char(token_string, c);
                            state = 6;
                        }
                        //numbers
                        else if(isdigit(c)){
                            token->type = TTYPE_INT;
                            string_append_char(tmp, c);
                            state = 5;
                        }
                        else if(isspace(c)) {   
                            state = 0;
                        }
                        else{
                            fprintf(stderr, "Lexical analysis error: Unknown operator!\n");
                            return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                        }
                        break;
                    } 
                break;           
            //inline comments
            case 1:
                if (c == '\n') {
                    //in case of comment after non-eol token, there is an eol token to be generated
                    if(new_line != 1){
                        ungetc(c, f);
                    }
                    state = 0;
                }
                if(c == EOF){
                    state = 0;
                    ungetc(c, f);
                }
                break;
            //multiline string
            case 2:
                //after first or second " different char appeared -> error 
                if(c == '"'){
                    string_append_char(tmp, c);
                }
                else{
                    fprintf(stderr, "Lexical analysis error: Quotation marks for documentation string were not written correctly!\n");
                    return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);             
                }
                //multiline string, searching for """
                if((strcmp(tmp->array, "\"\"\"")) == 0){
                    state = 3;
                    token->type = TTYPE_DOCSTR;
                    string_clear(tmp);
                }
                break;
            case 3:
                if(c == '"') {
                    string_append_char(tmp, c);
                }
                //EOF appeared before enclosure of multiline string
                else if(c == EOF) {
                    ungetc(c, f);
                    fprintf(stderr, "Lexical analysis error: EOF appeared before enclosure of multiline string!\n");
                    return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                }
                //escape sequences
                else if(c == '\\'){
                    c = getc(f);
                    if(c == '"'){
                        string_append_char(token_string, c);
                    }
                    else if(c == '\''){
                        string_append_char(token_string, c);
                    }
                    else if(c == 't'){
                        string_append_char(token_string, '\t');
                    }
                    else if(c == '\\'){
                        string_append_char(token_string, c);
                    }
                    else if(c == 'x'){
                        hexa_escape(f, token_string);
                    }
                    else{
                        ungetc(c, f);
                        string_append_char(token_string, '\\');
                    }
                }
                //clear string which compares end of docstring, continue appending to string
                else {
                    string_clear(tmp);
                    string_append_char(token_string, c);        
                }
                //compare if we approached end of docstring
                if((strcmp(tmp->array, "\"\"\"")) == 0){
                    state = 0;
                    if(new_line){
                        string_clear(tmp);
                        string_clear(token_string);
                        break;
                    }
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                break;
            //possible multicharacter operators (//, <= ...)
            case 4:
                //integer division
                if(c == '/'){
                    string_append_char(tmp, c);
                    if((strcmp(tmp->array, "//")) == 0){
                        token->type = TTYPE_IDIV;
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                    //unknown operator (e.g </)
                    else{
                        ungetc(c, f);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                }
                //compare operators
                else if(c == '='){
                    string_append_char(tmp, c);
                    if((strcmp(tmp->array, "<=")) == 0){
                        token->type = TTYPE_LSOREQ;
                    }
                    else if((strcmp(tmp->array, ">=")) == 0){
                        token->type = TTYPE_GTOREQ;
                    }
                    else if((strcmp(tmp->array, "==")) == 0){
                        token->type = TTYPE_ISEQ;
                    }
                    else if((strcmp(tmp->array, "!=")) == 0){
                        token->type = TTYPE_ISNEQ;
                    }
                    else{
                        ungetc(c, f);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                }
                else{
                    ungetc(c, f);
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                return finish_free_resources(SUCCESS, token, tmp, token_string);
            //exponents or float values 
            case 5:      
                //number with floating point
                if(c == '.'){
                    //bad order in number (eg. 10e32.3)
                    if(str_find_char(tmp, 'e')){
                        fprintf(stderr, "Lexical analysis error : Wrong order of exponent and floating point in number!\n");
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    //searching for previous occurances of '.'
                    if((str_find_char(tmp, c))){ 
                        fprintf(stderr, "Lexical analysis error : Bad formatting of float number!\n");
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    else{
                        string_append_char(tmp, c);
                        token->type = TTYPE_DOUBLE;
                    }
                }
                //exponent numbers
                else if(c == 'e' || c == 'E'){
                     //searching for previous occurances of 'e'
                    if(str_find_char(tmp, c)){
                        fprintf(stderr, "Lexical analysis error : Bad formatting of number with exponent!\n");
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    else{
                        //e.g 203ebola 23e-blabla
                        string_append_char(tmp, c);
                        token->type = TTYPE_DOUBLE;
                        state = 8;
                    }
                }
                //+ -  can stand in middle of number after exponent only (e.g 10e-2)
                else if(c == '+' || c == '-'){
                    //if '+'/'-' appeared after int
                    if(token->type == TTYPE_INT){
                        ungetc(c ,f);
                        token->attribute.integer = strtol(tmp->array, NULL, 10);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                    //latest char in tmp string is '.', return it to file stream and finish
                    if(tmp->array[tmp->index-1] == '.'){
                        ungetc('.', f);
                    }
                    //latest char in tmp string is 'e'/'E', continue
                    if(tmp->array[tmp->index-1] == 'e'){
                        string_append_char(tmp, c);
                    }
                    else if(tmp->array[tmp->index-1] == 'E'){
                        string_append_char(tmp, c);
                    }
                    else{
                        ungetc(c, f);
                        token->attribute.decimal = strtod(tmp->array, NULL);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                }
                else if(isdigit(c)){
                    string_append_char(tmp, c);
                    break;
                }
                else{
                    //after '.' or 'e' 'E' non-numeric number appeared
                    if(tmp->array[tmp->index-1] == '.'){
                        ungetc('.', f);
                        token->type = TTYPE_INT;
                    }
                    if(tmp->array[tmp->index-1] == 'e'){
                        ungetc('e', f);
                    }
                    if(tmp->array[tmp->index-1] == 'E'){
                        ungetc('E', f);
                    }
                    ungetc(c, f);
                    if(token->type == TTYPE_INT){
                        token->attribute.integer = strtol(tmp->array, NULL, 10);
                    }
                    if(token->type == TTYPE_DOUBLE){
                        token->attribute.decimal = strtod(tmp->array, NULL);
                    }
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                break;
            //identifiers or keywords
            case 6:
                if(isalnum(c) || c == '_'){
                    string_append_char(token_string, c);
                }
                else{
                    ungetc(c, f);
                    if((strcmp(token_string->array, "def")) == 0){    
                        string_clear(token_string);
                        token->type = TTYPE_KEYWORD;
                        token->attribute.keyword = KEY_DEF;
                    }
                    else if((strcmp(token_string->array, "else")) == 0){
                        string_clear(token_string);  
                        token->type = TTYPE_KEYWORD;
                        token->attribute.keyword = KEY_ELSE;
                    }
                    else if((strcmp(token_string->array, "if")) == 0){
                        string_clear(token_string);
                        token->type = TTYPE_KEYWORD; 
                        token->attribute.keyword = KEY_IF;
                    }
                    else if((strcmp(token_string->array, "None")) == 0){ 
                        string_clear(token_string);  
                        token->type = TTYPE_KEYWORD;
                        token->attribute.keyword = KEY_NONE;
                    }
                    else if((strcmp(token_string->array, "pass")) == 0){ 
                        string_clear(token_string);  
                        token->type = TTYPE_KEYWORD;
                        token->attribute.keyword = KEY_PASS;
                    }
                    else if((strcmp(token_string->array, "return")) == 0){
                        string_clear(token_string); 
                        token->type = TTYPE_KEYWORD;
                        token->attribute.keyword = KEY_RETURN;
                    }
                    else if((strcmp(token_string->array, "while")) == 0){
                        string_clear(token_string);  
                        token->type = TTYPE_KEYWORD;
                        token->attribute.keyword = KEY_WHILE;
                    }
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                break;
            //string 
            case 7:  
                if(c == '\''){
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                else if(c == '\n' || c == EOF){
                    ungetc(c, f);
                    fprintf(stderr, "Lexical analysis error : String was not enclosed!\n");
                    return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);  
                }
                //escape sequence
                else if(c == '\\'){
                    c = getc(f);
                    if(c == '"'){
                        string_append_char(token_string, c);
                    }
                    else if(c == '\''){
                        string_append_char(token_string, c);
                    }
                    else if(c == 'n'){
                        string_append_char(token_string, '\n');
                    }
                    else if(c == 't'){
                        string_append_char(token_string, '\t');
                    }
                    else if(c == '\\'){
                        string_append_char(token_string, c);
                    }
                    else if(c == 'x'){
                        hexa_escape(f, token_string);
                    }
                    else{
                        ungetc(c, f);
                        string_append_char(token_string, '\\');
                    }
                }
                else{
                    string_append_char(token_string, c);
                }
                break;
                case 8:
                    if(c == '-' || c == '+'){
                        string_append_char(tmp, c);
                        state = 5;
                    }
                    else if(isdigit(c)){
                        string_append_char(tmp, c);
                        state = 5;
                    }
                    else{
                        ungetc(c ,f);
                        if(tmp->array[tmp->index-1] == 'e'){
                            ungetc('e', f);
                            token->type = TTYPE_INT;
                        }
                        else if(tmp->array[tmp->index-1] == 'E'){
                            ungetc('E', f);
                            token->type = TTYPE_INT;
                        }
                        if(token->type == TTYPE_INT){
                            token->attribute.integer = strtol(tmp->array, NULL, 10);
                        }
                        if(token->type == TTYPE_DOUBLE){
                            token->attribute.decimal = strtod(tmp->array, NULL);
                        }
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                    break;

        }
    }
}
