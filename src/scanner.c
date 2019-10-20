#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "dynamic_string.h"
#include "scanner.h"

//TODO  INDENT/DEDENT
//      
// 

#define LEX_ERROR 1
#define LEX_SUCCES 0

/**
 * [scans through the code, recognises different tokens, ignores comments]
 * @param  Tstring  [token->attr.string, saves names of identifiers, used for comparison to parse different tokens]
 * @param  f     [source file]
 * @param  token [processed token]
 * @return       [returns 0 on succes, else 1]
 */
int get_token(FILE* f, Ttoken* token){
    int state = 0;
    int c;
    t_string tmp;
    t_string* Tstring = &token->attr.string;
  
    string_init(Tstring);

    if(Tstring->array != NULL)
       string_clear(Tstring);

    while(1){
        c = getc(f);
        switch(state){
            //default state
            case 0:     
                if(isspace(c)) {
                    state = 0;
                }
                else{
                    switch(c){
                        //comments
                        case '#':
                            state = 1;
                            break;
                        case '"':
                            string_init(&tmp);
                            string_append(&tmp, c);
                            state = 2;
                            break;
                        //operators
                        case '+':
                            string_append(Tstring, c);
                            token->type = TTYPE_ADD;
                            state = 4;
                            break;
                        case '-':
                            string_append(Tstring, c);
                            token->type = TTYPE_SUB;
                            state = 4;
                            break;
                        case '*':
                            string_append(Tstring, c);
                            token->type = TTYPE_MUL;
                            state = 4;
                            break;
                        case '/':
                            string_append(Tstring, c);
                            token->type = TTYPE_DIV;
                            state = 5;
                            break;
                        case '<':
                            string_append(Tstring, c);
                            token->type = TTYPE_LS;
                            state = 5;
                            break;
                        case '>':
                            string_append(Tstring, c);
                            token->type = TTYPE_GT;
                            state = 5;
                            break;
                        case '=':
                            string_append(Tstring, c);
                            token->type = TTYPE_ASSIGN;
                            state = 5;
                            break;
                        case '!':
                            string_append(Tstring, c);
                            token->type = TTYPE_EXCL;
                            state = 5;
                            break;
                        case '(':
                            string_append(Tstring, c);
                            token->type = TTYPE_LTBRAC;
                            return LEX_SUCCES;
                        case ')':
                            string_append(Tstring, c);
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
                            string_append(Tstring, c);
                            token->type = TTYPE_COMMA;
                            return LEX_SUCCES;
                        case '\n':
                            token->type = TTYPE_EOL;
                            return LEX_SUCCES;
                        default:
                            //identifiers
                            if(isalpha(c) || c == '_'){
                                token->type = TTYPE_ID;
                                string_append(Tstring, c);
                                state = 7;
                            }
                            //numbers
                            if(isdigit(c)){
                                string_append(Tstring, c);
                                token->type = TTYPE_INT;
                                state = 6;
                            }
                            break;

                    }
                }
                break;
            //inline comments
            case 1:
                if(c == '\n')
                    state = 0;
                if(c == EOF)
                    return LEX_ERROR;

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
                if((strcmp(tmp.array, "\"\"\"")) == 0){
                    state = 0;
                    string_free(&tmp);
                    return LEX_SUCCES;
                }
                if(c == '"') {
                    string_append(&tmp, c);
                }
                //EOF appeared before enclosure of multiline string
                else if(c == EOF) {
                    string_free(&tmp);
                    return LEX_ERROR;
                }
                else {
                    string_append(Tstring, c);        
                }
                break;

            //one character operator
            case 4:
                if(isdigit(c)){
                    //number starting with + or - 
                    if(token->type == TTYPE_ADD || token->type == TTYPE_SUB){
                        string_append(Tstring, c);
                        token->type = TTYPE_INT;
                        state = 6;
                        break;
                    }
                    //after operator / or * was number -> error?? 
                    else{
                        return LEX_ERROR;
                    }
                }
                if(isspace(c)){
                    return LEX_SUCCES;
                }
                else{
                    string_append(Tstring, c);
                    return LEX_ERROR;
                }

            //possible multicharacter operators (//, <= ...)
            case 5:
                if(isspace(c)){
                    return LEX_SUCCES;
                }
                if(c == '/'){
                    string_append(Tstring, c);
                    if((strcmp(Tstring->array, "//")) == 0){
                        token->type = TTYPE_IDIV;
                        break;
                    }
                    //unknown operator (e.g </)
                    else{
                        string_append(Tstring, c);
                        return LEX_ERROR;
                    }
                }
                else if(c == '='){
                    string_append(Tstring, c);
                    if((strcmp(Tstring->array, "<=")) == 0){
                        token->type = TTYPE_LSOREQ;
                    }
                    else if((strcmp(Tstring->array, ">=")) == 0){
                        token->type = TTYPE_GTOREQ;
                    }
                    else if((strcmp(Tstring->array, "==")) == 0){
                        token->type = TTYPE_ISEQ;
                    }
                    else if((strcmp(Tstring->array, "!=")) == 0){
                        token->type = TTYPE_ISNEQ;
                    }
                    else{
                        return LEX_ERROR;
                    }
                    break;

                }
                //unknown operators (e.g *a, </)
                else{
                    string_append(Tstring, c);
                    return LEX_ERROR;
                }
                break;

            //exponents or float values 
            case 6:
                //(e.g if a < 0: )
                if(isspace(c) || c == ':' || c == EOF){
                    char* ptr = NULL;
                    if(token->type == TTYPE_INT)
                        token->attr.integer = strtol(Tstring->array, &ptr, 10);
                    if(token->type == TTYPE_DOUBLE)
                        token->attr.decimal = strtod(Tstring->array, &ptr);
                    return LEX_SUCCES;
                }
                //decimals and exponent numbers
                if(c == '.' || c == 'e' || c == 'E'){
                    if(str_find_char(Tstring, c)){
                        string_clear(Tstring);
                        return LEX_ERROR;
                    }
                    else{
                        string_append(Tstring, c);
                        token->type = TTYPE_DOUBLE;
                        break;
                    }
                }
                //+ -  can stand in middle of number after exponent only (e.g 10e-2)
                else if(c == '+' || c == '-'){
                    if(Tstring->array[Tstring->index-1] == 'e'){
                        string_append(Tstring, c);
                        break;
                    }
                    else if(Tstring->array[Tstring->index-1] == 'E'){
                        string_append(Tstring, c);
                        break;
                    }
                }
                else if(isdigit(c)){
                    string_append(Tstring, c);
                    break;
                }
                else{
                    return LEX_ERROR;
                }
            //identifiers or keywords
            case 7:
                //grc, mozno nejak inak
                if(isspace(c) || c == EOF){
                    if(c == EOF)
                        ungetc(c, f);

                    if((strcmp(Tstring->array, "def")) == 0){    
                        string_clear(Tstring);
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_DEF;
                        return LEX_SUCCES; 
                    }
                    else if((strcmp(Tstring->array, "else")) == 0){
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_ELSE;
                        return LEX_SUCCES; 
                    }
                    else if((strcmp(Tstring->array, "if")) == 0){
                        string_clear(Tstring);
                        token->type = TTYPE_KEYWORD; 
                        token->attr.keyword = KEY_IF;
                        return LEX_SUCCES; 
                    }
                    else if((strcmp(Tstring->array, "None")) == 0){ 
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_NONE;
                        return LEX_SUCCES; 
                    }
                    else if((strcmp(Tstring->array, "pass")) == 0){ 
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_PASS;
                        return LEX_SUCCES; 
                    }
                    else if((strcmp(Tstring->array, "return")) == 0){
                        string_clear(Tstring); 
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_RETURN;
                        return LEX_SUCCES; 
                    }
                    else if((strcmp(Tstring->array, "while")) == 0){
                        string_clear(Tstring);  
                        token->type = TTYPE_KEYWORD;
                        token->attr.keyword = KEY_WHILE;
                        return LEX_SUCCES; 
                    }
                    return LEX_SUCCES;
                }
                if(isalnum(c) || c == '_'){
                    string_append(Tstring, c);
                }
                else if(c == '('){
                    ungetc(c, f);
                    return LEX_SUCCES;
                }
                else if(c == ','){
                    ungetc(c, f);
                    return LEX_SUCCES;
                }
                break;
            //string 
            case 8:  
                if(c == '\''){
                    return LEX_SUCCES;
                }
                else if(c == '\n' || c == EOF){
                    return LEX_ERROR;
                }
                else{
                    string_append(Tstring, c);
                }
                break;

        }
    }
}
