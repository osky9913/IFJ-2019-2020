#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dynamic_string.h"
#include "scanner.h"

//TODO  INDENT/DEDENT
//      cache for one token?
//      allocation/deallocation

#define LEX_ERROR 1
#define LEX_SUCCES 0

/**
 * [converts hexadecimal number from escape sequence to decimal to char and appends it at the end]
 * @param f       [source file]
 * @param Tstring [token's string attribute]
 */
void hexa_escape(FILE* f, t_string* Tstring){
    long hexa;
    t_string tmp;
    int c;
    c = getc(f);
    if(isxdigit(c)){
        string_init(&tmp);
        string_append(&tmp, '0');
        string_append(&tmp, 'x');
        string_append(&tmp, c);
        c = getc(f);
        if(isxdigit(c)){
            string_append(&tmp, c);
            hexa = strtol(tmp.array, NULL, 16);
            string_append(Tstring, hexa);
        }
        else{
            ungetc(c, f);
            hexa = strtol(tmp.array, NULL, 16);
            string_append(Tstring, hexa);
        }
    }
    else{
        string_append(Tstring, '\\');
        ungetc(c, f);
    }
    string_free(&tmp);                                      
}

/**
 * [scans through the code, recognises different tokens, ignores comments]
 * @param  f     [source file]
 * @param  token [processed token]
 * @return       [returns 0 on succes, else 1]
 */
int get_token(FILE* f, Ttoken* token){
    int state = 0;
    int c;
    t_string tmp;
    t_string* Tstring = &token->attr.string;

    while(1){
        c = getc(f);
        switch(state){
            //default state
            case 0:   
                switch(c){
                    //comments
                    case '#':
                        state = 1;
                        break;
                    case '"':
                        string_init(Tstring);
                        string_init(&tmp);
                        string_append(&tmp, c);
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
                        string_init(&tmp);
                        string_append(&tmp, c);
                        token->type = TTYPE_DIV;
                        state = 5;
                        break;
                    case '<':
                        string_init(&tmp);
                        string_append(&tmp, c);
                        token->type = TTYPE_LS;
                        state = 5;
                        break;
                    case '>':
                        string_init(&tmp);
                        string_append(&tmp, c);
                        token->type = TTYPE_GT;
                        state = 5;
                        break;
                    case '=':
                        string_init(&tmp);
                        string_append(&tmp, c);
                        token->type = TTYPE_ASSIGN;
                        state = 5;
                        break;
                    case '!':
                        string_init(&tmp);
                        string_append(&tmp, c);
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
                        string_init(Tstring);
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
                        token->type = TTYPE_EOL;
                        return LEX_SUCCES;
                    case ':':
                        token->type = TTYPE_COLUMN;
                        return LEX_SUCCES;
                    default:
                        //identifiers
                        if(isalpha(c) || c == '_'){
                            string_init(Tstring);
                            token->type = TTYPE_ID;
                            string_append(Tstring, c);
                            state = 7;
                        }
                        //numbers
                        if(isdigit(c)){
                            string_init(&tmp);
                            string_append(&tmp, c);
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
                if(c == '"')
                    string_append(&tmp, c);
                else
                    return LEX_ERROR;
                //multiline string, searching for """
                if((strcmp(tmp.array, "\"\"\"")) == 0){
                    state = 3;
                    token->type = TTYPE_DOCSTR;
                    string_clear(&tmp);
                    break;
                }
                break;
            case 3:
                if(c == '"') {
                    string_append(&tmp, c);
                }
                //EOF appeared before enclosure of multiline string
                else if(c == EOF) {
                    ungetc(c, f);
                    string_free(&tmp);
                    return LEX_ERROR;
                }
                else if(c == '\\'){
                    c = getc(f);
                    if(c == '"'){
                        string_append(Tstring, c);
                    }
                    else if(c == '\''){
                        string_append(Tstring, c);
                    }
                    else if(c == 't'){
                        string_append(Tstring, '\t');
                    }
                    else if(c == '\\'){
                        string_append(Tstring, c);
                    }
                    else if(c == 'x'){
                        hexa_escape(f, Tstring);
                    }
                    else{
                        ungetc(c, f);
                        string_append(Tstring, '\\');
                    }
                }
                else {
                    string_clear(&tmp);
                    string_append(Tstring, c);        
                }
                if((strcmp(tmp.array, "\"\"\"")) == 0){
                    state = 0;
                    token->type = TTYPE_STR;
                    string_free(&tmp);
                    return LEX_SUCCES;
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
                else{
                    return LEX_ERROR;
                }
                return LEX_SUCCES;
            //possible multicharacter operators (//, <= ...)
            case 5:
                if(c == '/'){
                    if((strcmp(tmp.array, "//")) == 0){
                        token->type = TTYPE_IDIV;
                        break;
                    }
                    //unknown operator (e.g </)
                    else{
                        ungetc(c, f);
                        string_free(&tmp);
                        return LEX_ERROR;
                    }
                }
                else if(c == '='){
                    string_append(&tmp, c);
                    if((strcmp(tmp.array, "<=")) == 0){
                        token->type = TTYPE_LSOREQ;
                    }
                    else if((strcmp(tmp.array, ">=")) == 0){
                        token->type = TTYPE_GTOREQ;
                    }
                    else if((strcmp(tmp.array, "==")) == 0){
                        token->type = TTYPE_ISEQ;
                    }
                    else if((strcmp(tmp.array, "!=")) == 0){
                        token->type = TTYPE_ISNEQ;
                    }
                    else{
                        string_free(&tmp);
                        return LEX_ERROR;
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
                else{
                    string_free(&tmp);
                    return LEX_ERROR;
                }
                string_free(&tmp);
                return LEX_SUCCES;
            //exponents or float values 
            case 6:           
                //decimals and exponent numbers
                if(c == '.'){
                    if((str_find_char(&tmp, c))){
                        string_clear(&tmp);
                        return LEX_ERROR;
                    }
                    else{
                        string_append(&tmp, c);
                        token->type = TTYPE_DOUBLE;
                    }
                }
                else if(c == 'e' || c == 'E'){
                    if(str_find_char(&tmp, c)){
                        string_clear(&tmp);
                        return LEX_ERROR;
                    }
                    else{
                        if(str_find_char(&tmp, '.'))
                            token->type = TTYPE_DOUBLE;
                        //e.g 203ebola 23e-blabla
                        string_append(&tmp, c);
                        state = 9;
                    }
                }
                //+ -  can stand in middle of number after exponent only (e.g 10e-2)
                else if(c == '+' || c == '-'){
                    if(tmp.array[tmp.index-1] == 'e'){
                        string_append(&tmp, c);
                    }
                    else if(tmp.array[tmp.index-1] == 'E'){
                        string_append(&tmp, c);
                    }
                    else{
                        string_free(&tmp);
                        return LEX_ERROR;
                    }
                }
                else if(isdigit(c)){
                    string_append(&tmp, c);
                }
                else{
                    ungetc(c, f);
                    if(token->type == TTYPE_INT)
                        token->attr.integer = strtol(tmp.array, NULL, 10);
                    if(token->type == TTYPE_DOUBLE)
                        token->attr.decimal = strtod(tmp.array, NULL);
                    string_free(&tmp);
                    return LEX_SUCCES;
                }
                break;
            //identifiers or keywords
            case 7:
                if(isalnum(c) || c == '_'){
                    string_append(Tstring, c);
                }
                else{
                    ungetc(c, f);
                    if((strcmp(Tstring->array, "def")) == 0){    
                        string_clear(Tstring);
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_DEF;
                    }
                    else if((strcmp(Tstring->array, "else")) == 0){
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_ELSE;
                    }
                    else if((strcmp(Tstring->array, "if")) == 0){
                        string_clear(Tstring);
                        token->type = TTYPE_KEYWORD; 
                        token->attr.keyword = KEY_IF;
                    }
                    else if((strcmp(Tstring->array, "None")) == 0){ 
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_NONE;
                    }
                    else if((strcmp(Tstring->array, "pass")) == 0){ 
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_PASS;
                    }
                    else if((strcmp(Tstring->array, "return")) == 0){
                        string_clear(Tstring); 
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_RETURN;
                    }
                    else if((strcmp(Tstring->array, "while")) == 0){
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_WHILE;
                    }
                    return LEX_SUCCES;
                }
                break;
            //string 
            case 8:  
                if(c == '\''){
                    return LEX_SUCCES;
                }
                else if(c == '\n' || c == EOF){
                    ungetc(c, f);
                    string_clear(Tstring);
                    return LEX_ERROR;
                }
                //escape sequence
                else if(c == '\\'){
                    c = getc(f);
                    if(c == '"'){
                        string_append(Tstring, c);
                    }
                    else if(c == '\''){
                        string_append(Tstring, c);
                    }
                    else if(c == 'n'){
                        string_append(Tstring, '\n');
                    }
                    else if(c == 't'){
                        string_append(Tstring, '\t');
                    }
                    else if(c == '\\'){
                        string_append(Tstring, c);
                    }
                    else if(c == 'x'){
                        hexa_escape(f, Tstring);
                    }
                    else{
                        ungetc(c, f);
                        string_append(Tstring, '\\');
                    }
                }
                else{
                    string_append(Tstring, c);
                }
                break;
                case 9:
                    if(c == '-' || c == '+'){
                        string_append(&tmp, c);
                        state = 6;
                    }
                    else if(isdigit(c)){
                        string_append(&tmp, c);
                        state = 6;
                    }
                    else{
                        ungetc(c ,f);
                        if(tmp.array[tmp.index-1] == 'e'){
                            ungetc('e', f);
                        }
                        else{
                            ungetc('E', f);
                        }
                        if(token->type == TTYPE_INT)
                            token->attr.integer = strtol(tmp.array, NULL, 10);
                        if(token->type == TTYPE_DOUBLE)
                            token->attr.decimal = strtod(tmp.array, NULL);
                        string_free(&tmp);
                        return LEX_SUCCES;
                    }
                    break;

        }
    }
}
