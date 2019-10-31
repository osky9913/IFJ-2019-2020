/**
 * @file scanner.c
 * @brief This module implements all parts of lexical analysis.
 * @author xziska03
 */

#include "dynamic_string.h"
#include "indent_stack.h"
#include "scanner.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int new_line = 0;

int calculate_dent(FILE* f, int c){
    int dent = 0;
    while(isspace(c)){
        if(c == '\n'){
            return dent;
        }
        else{
            dent++;
        }
        c = getc(f);
    }
    return dent;
}

int finish_free_resources(int exit_code, token_t* token, string_t* tmp, string_t* token_string){
    if(token->type == TTYPE_STR || token->type == TTYPE_DOCSTR || token->type == TTYPE_ID){
        token->attribute.string = string_copy_data(token_string);
    }
    string_free(tmp);
    string_free(token_string);
    return exit_code;
}

void hexa_escape(FILE* f, string_t* token_string){
    long hexa;
    string_t tmp;
    string_init(&tmp);
    int c;
    c = getc(f);
    if(isxdigit(c)){
        string_append_char(&tmp, '0');
        string_append_char(&tmp, 'x');
        string_append_char(&tmp, c);
        c = getc(f);
        if(isxdigit(c)){
            string_append_char(&tmp, c);
            hexa = strtol(tmp.array, NULL, 16);
            string_append_char(token_string, hexa);
        }
        else{
            ungetc(c, f);
            hexa = strtol(tmp.array, NULL, 16);
            string_append_char(token_string, hexa);
        }
    }
    else{
        string_append_char(token_string, '\\');
        ungetc(c, f);
    }
    string_free(&tmp);                                      
}

int get_token(FILE* f, token_t* token){
    int state = 0;
    int c;
    string_t* tmp = string_create_init();
    string_t* token_string = string_create_init();

    while(1){
        c = getc(f);
        //indent dedent
        if(new_line){
            int dent = calculate_dent(f, c);
            if(dent > indent_stack_top(dent_stack)){
                indent_stack_push(dent_stack, dent);
                token->type = TTYPE_INDENT;
                ungetc(c, f);
                new_line = 0;
                return finish_free_resources(LEX_SUCCES, token, tmp, token_string); 
            }
            else if(dent < indent_stack_top(dent_stack)){
                while(dent != indent_stack_top(dent_stack)){
                    indent_stack_pop(dent_stack);
                    if(dent_stack->top == -1){
                        fprintf(stderr, "Indentation error: Indentation in commands sequence was not correct!\n");
                        return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                    }
                }
                token->type = TTYPE_DEDENT;
                new_line = 0;
                return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
            }
            new_line = 0;
        }
        switch(state){

            //default state
            case 0:   
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
                        state = 4;
                        break;
                    case '-':
                        token->type = TTYPE_SUB;
                        state = 4;
                        break;
                    case '*':
                        token->type = TTYPE_MUL;
                        state = 4;
                        break;
                    case '/':
                        string_append_char(tmp, c);
                        token->type = TTYPE_DIV;
                        state = 5;
                        break;
                    case '<':
                        string_append_char(tmp, c);
                        token->type = TTYPE_LS;
                        state = 5;
                        break;
                    case '>':
                        string_append_char(tmp, c);
                        token->type = TTYPE_GT;
                        state = 5;
                        break;
                    case '=':
                        string_append_char(tmp, c);
                        token->type = TTYPE_ASSIGN;
                        state = 5;
                        break;
                    case '!':
                        string_append_char(tmp, c);
                        token->type = TTYPE_EXCL;
                        state = 5;
                        break;
                    case '(':
                        token->type = TTYPE_LTBRAC;
                        return LEX_SUCCES;
                    case ')':
                        token->type = TTYPE_RTBRAC;
                        return LEX_SUCCES;
                    case '\'':
                        token->type = TTYPE_STR;
                        state = 8;
                        break;
                    case EOF:
                        token->type = TTYPE_EOF;
                        return LEX_SUCCES;
                    case ',':
                        token->type = TTYPE_COMMA;
                        return LEX_SUCCES;
                    case '\n':
                        new_line = 1;
                        token->type = TTYPE_EOL;
                        return LEX_SUCCES;
                    case ':':
                        token->type = TTYPE_COLUMN;
                        return LEX_SUCCES;
                    default:
                        //identifiers
                        if(isalpha(c) || c == '_'){
                            token->type = TTYPE_ID;
                            string_append_char(token_string, c);
                            state = 7;
                        }
                        //numbers
                        if(isdigit(c)){
                            string_append_char(tmp, c);
                            token->type = TTYPE_INT;
                            state = 6;
                        }
                        if(isspace(c)) {   
                            state = 0;
                        }
                        break;
                    } 
                break;           
            //inline comments
            case 1:
                if(c == '\n' || c == EOF){
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
                    return finish_free_resources(LEX_ERROR, token, tmp, token_string);             
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
                    return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                }
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
                else {
                    string_clear(tmp);
                    string_append_char(token_string, c);        
                }
                if((strcmp(tmp->array, "\"\"\"")) == 0){
                    state = 0;
                    token->type = TTYPE_STR;
                    return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
                }
                break;

            //one character operator
            case 4:
                if(isalnum(c) || c == '_'){
                    ungetc(c, f);
                }
                else if(isspace(c)){
                    ungetc(c, f);
                }
                else if(c == '('){
                    ungetc(c, f);
                }
                else if(c == ')'){
                    ungetc(c, f);
                }
                else if(c == EOF){

                }
                else{
                    fprintf(stderr, "Lexical analysis error : Unknown operator!\n");
                    return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                }
                return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
            //possible multicharacter operators (//, <= ...)
            case 5:
                if(c == '/'){
                    if((strcmp(tmp->array, "//")) == 0){
                        token->type = TTYPE_IDIV;
                        break;
                    }
                    //unknown operator (e.g </)
                    else{
                        ungetc(c, f);
                        fprintf(stderr, "Lexical analysis error : Unknown operator!\n");
                        return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                    }
                }
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
                        fprintf(stderr, "Lexical analysis error : Unknown operator!\n");
                        return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                    }
                }
                else if(isalnum(c) || c == '_'){
                    ungetc(c, f);
                }
                else if(isspace(c)){
                    ungetc(c, f);
                }
                else if(c == '('){
                    ungetc(c, f);
                }
                else if(c == ')'){
                    ungetc(c, f);
                }
                else{
                    fprintf(stderr, "Lexical analysis error : Unknown operator!\n");
                    return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                }
                return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
            //exponents or float values 
            case 6:           
                //decimals and exponent numbers
                if(c == '.'){
                    if((str_find_char(tmp, c))){ 
                        fprintf(stderr, "Lexical analysis error : Bad formatting of float number!\n");
                        return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                    }
                    else{
                        string_append_char(tmp, c);
                        token->type = TTYPE_DOUBLE;
                    }
                }
                else if(c == 'e' || c == 'E'){
                    token->type = TTYPE_DOUBLE;
                    if(str_find_char(tmp, c)){
                        fprintf(stderr, "Lexical analysis error : Bad formatting of number with exponent!\n");
                        return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                    }
                    else{
                        //e.g 203ebola 23e-blabla
                        string_append_char(tmp, c);
                        state = 9;
                    }
                }
                //+ -  can stand in middle of number after exponent only (e.g 10e-2)
                else if(c == '+' || c == '-'){
                    if(tmp->array[tmp->index-1] == 'e'){
                        string_append_char(tmp, c);
                    }
                    else if(tmp->array[tmp->index-1] == 'E'){
                        string_append_char(tmp, c);
                    }
                    else{
                        fprintf(stderr, "Lexical analysis error : Found '%c' in middle of number!\n", c);
                        return finish_free_resources(LEX_ERROR, token, tmp, token_string);
                    }
                }
                else if(isdigit(c)){
                    string_append_char(tmp, c);
                }
                else{
                    ungetc(c, f);
                    if(token->type == TTYPE_INT){
                        token->attribute.integer = strtol(tmp->array, NULL, 10);
                    }
                    if(token->type == TTYPE_DOUBLE){
                        token->attribute.decimal = strtod(tmp->array, NULL);
                    }
                    return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
                }
                break;
            //identifiers or keywords
            case 7:
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
                    return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
                }
                break;
            //string 
            case 8:  
                if(c == '\''){
                    return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
                }
                else if(c == '\n' || c == EOF){
                    ungetc(c, f);
                    fprintf(stderr, "Lexical analysis error : String was not enclosed!\n");
                    return finish_free_resources(LEX_ERROR, token, tmp, token_string);  
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
                case 9:
                    if(c == '-' || c == '+'){
                        string_append_char(tmp, c);
                        state = 6;
                    }
                    else if(isdigit(c)){
                        string_append_char(tmp, c);
                        state = 6;
                    }
                    else{
                        ungetc(c ,f);
                        if(tmp->array[tmp->index-1] == 'e'){
                            ungetc('e', f);
                        }
                        else if(tmp->array[tmp->index-1] == 'E'){
                            ungetc('E', f);
                        }
                        if(token->type == TTYPE_INT){
                            token->attribute.integer = strtol(tmp->array, NULL, 10);
                        }
                        if(token->type == TTYPE_DOUBLE){
                            token->attribute.decimal = strtod(tmp->array, NULL);
                        }
                        return finish_free_resources(LEX_SUCCES, token, tmp, token_string);
                    }
                    break;

        }
    }
}
