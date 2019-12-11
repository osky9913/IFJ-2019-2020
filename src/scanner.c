/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file scanner.c
 * @author Marek Ziska, xziska03
 * @brief This module implements all parts of lexical analysis.
 */

#include "scanner.h"

/* Scanner data global variable */
sdata_t scn = {
    .line_counter = 1,
    .new_line = -1,
    .indents_to_pop = 0,
    .indent = 0,
    .dent_stack = NULL
    };

int calculate_dent(int* c){
    scn.indent = 0;
    stack_general_item_t* stack_top = NULL;
    //ignore newlines scn.indentation
    if(*c == '\n'){
        stack_top = stack_general_top(scn.dent_stack);
        return *(int*)stack_top->data;
    }
    while(isspace(*c)){
        if(*c == '\n'){
            scn.indent = 0;
            *c = getc(stdin);
        }
        else{
            scn.indent++;
            *c = getc(stdin);
        }
    }
    if(*c == '#' || *c == '"'){
        ungetc(*c, stdin);
        stack_top = stack_general_top(scn.dent_stack);
        return *(int*)stack_top->data;
    }
    return scn.indent;
}

int finish_free_resources(int exit_code, token_t* token, string_t* tmp, string_t* token_string){
    if(token->type == TTYPE_STR || token->type == TTYPE_ID){
        token->attribute.string = string_copy_data(token_string);
    }
    if(token->type == TTYPE_INT) {
        token->attribute.string = string_copy_data(tmp);
    }
    //in case of double we expect format of 123e12 -> needed conversion
    if(token->type == TTYPE_DOUBLE){
        char *ptr = NULL;
        char number_string[100] = {0};
        double convert_exponent = strtod(tmp->array, &ptr);
        string_clear(tmp);
        sprintf(number_string, "%f", convert_exponent);
        string_append(tmp, number_string);
        token->attribute.string = string_copy_data(tmp);
    }
    if(token->type == TTYPE_EOL){
        scn.new_line = 1;
    } else {
        scn.new_line = 0;
    }
    string_free(token_string);
    string_free(tmp);

    return exit_code;
}
int process_dedents(){
    int pop_indent;     //scn.indentation sequence that will be removed from scn.indent_stack
    stack_general_item_t* stack_top = stack_general_top(scn.dent_stack);
    int stack_top_int = *(int*)stack_top->data;


    while(scn.indent != stack_top_int){
        pop_indent = stack_top_int;//scn.indent_stack_top(dent_stack);
        stack_pop(scn.dent_stack);
        if(stack_empty(scn.dent_stack)){
            fprintf(stderr, "line %d: Lexical analysis error: "
                            "scn.indentation in commands sequence was not correct!\n", scn.line_counter);
            return 0;
        }
        stack_top = stack_general_top(scn.dent_stack);
        stack_top_int = *(int*)stack_top->data;
        //after first scn.indent was popped, dedent scn.indentation was equal to top of stack(there is no more scn.indents to be popped)
        if(scn.indent == stack_top_int){
            scn.indents_to_pop = 0;
            return 1;
        }
        //scn.indentation of dedent is smaller than one or more scn.indents scn.indentation(there is more scn.indents to be popped from stack)
        if(pop_indent > stack_top_int){
            scn.indents_to_pop = 1;
            return 1;
        }
    }
    return 0;
}

int get_token(token_t* token){
    int state = 0;
    int c;
    string_t* token_string = string_create_init();
    if(!token_string){return ALLOC_ERROR;}
    string_t* tmp = string_create_init();
    if(!tmp){ return ALLOC_ERROR;}
    stack_general_item_t* stack_top = NULL;
    int stack_top_int;

    //at the beginning of the file put zero on top of stack
    if(scn.new_line == -1){
        stack_general_push_int(scn.dent_stack, 0);
    }

    //there is more dedents to be generated
    if(scn.indents_to_pop){
        token->type = TTYPE_DEDENT;
        if(process_dedents()){
            return finish_free_resources(SUCCESS, token, tmp, token_string);
        }
        else{
            scn.indents_to_pop = 0; //len kvoli tomu aby sa to nezacykliklo, odstranit z finalnej verzie
            return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
        }
    }

    while(1){
        c = getc(stdin); 
        switch(state){
            //default state
            case 0:
                //scn.indent dedent
                if(scn.new_line == 1){
                    scn.indent = calculate_dent(&c);
                    stack_top = stack_general_top(scn.dent_stack);
 
                    stack_top_int = *(int*)stack_top->data;


                    if(scn.indent > stack_top_int){
                        //processed non-whitespace character we have to get it back
                        ungetc(c, stdin);
                        stack_general_push_int(scn.dent_stack, scn.indent);
                        token->type = TTYPE_INDENT;
                        return finish_free_resources(SUCCESS, token, tmp, token_string); 
                    }
                    else if(scn.indent < stack_top_int){
                        //processed non-whitespace character we have to get it back
                        ungetc(c, stdin);
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
                        scn.line_counter++;
                        if(scn.new_line){
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
                            if(c == '0'){
                                state = 9;
                            }
                            else {
                                state = 5;
                            }
                        }
                        else if(isspace(c)) {   
                            state = 0;
                        }
                        else{
                            fprintf(stderr, "line %d : Lexical analysis error: "
                                            "Unknown operator!\n", scn.line_counter);
                            return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                        }
                        break;
                    } 
                break;           
            //inline comments
            case 1:
                if (c == '\n') {
                    //in case of comment after non-eol token, there is an eol token to be generated
                    if(scn.new_line != 1){
                        ungetc(c, stdin);
                    }
                    else{
                        scn.line_counter++; 
                    }
                    state = 0;
                }
                if(c == EOF){
                    state = 0;
                    ungetc(c, stdin);
                }
                break;
            //multiline string
            case 2:
                //after first or second " different char appeared -> error 
                if(c == '"'){
                    string_append_char(tmp, c);
                }
                else{
                    fprintf(stderr, "line %d: Lexical analysis error: "
                                    "Quotation marks for documentation string were not written correctly!\n", scn.line_counter);
                    return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);             
                }
                //multiline string, searching for """
                if((strcmp(tmp->array, "\"\"\"")) == 0){
                    state = 3;
                    token->type = TTYPE_STR;
                    string_clear(tmp);
                }
                break;
            case 3:
                if(c == '"') {
                    string_append_char(tmp, c);
                }
                //EOF appeared before enclosure of multiline string
                else if(c == EOF) {
                    ungetc(c, stdin);
                    fprintf(stderr, "line %d: Lexical analysis error : "
                                    "EOF appeared before enclosure of multiline string!\n", scn.line_counter);
                    return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                }
                else if (c == ' ') {
                    string_append(token_string, "\\032");
                }
                else if (c == '\t') {
                    string_append(token_string, "\\009");
                }
                else if (c == '\n') {
                    string_append(token_string, "\\010");
                }
                else if (c == '#') {
                    string_append(token_string, "\\035");
                }
                else if (c == 11) {
                    string_append(token_string, "\\011");
                }
                //escape sequences
                else if(c == '\\'){
                    c = getc(stdin);
                    switch (c) {
                        case 'x':
                            //hexa to decimal
                            string_append_char(token_string, '\\');
                            unsigned hexa = 0;
                            for (int i = 0; i < 2; i++) {
                                c = getc(stdin);
                                if (isxdigit(c)) {
                                    if (isdigit(c)) {
                                        hexa += (c - '0') * (i == 0 ? 16 : 1);

                                    } else if (isupper(c)) {
                                        hexa += (c - 'A' + 10) * (i == 0 ? 16 : 1);

                                    } else {
                                        hexa += (c - 'a' + 10) * (i == 0 ? 16 : 1);
                                    }

                                } else {
                                    fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Invalid escape sequence!\n", scn.line_counter);
                                    return finish_free_resources(ERROR_LEXICAL,
                                    token, tmp, token_string);  
                                }
                            }

                            char tmp[4] = {0};

                            if (hexa < 100)
                                sprintf(tmp, "0%d", hexa);
                            else 
                                sprintf(tmp, "%d", hexa);

                            string_append(token_string, tmp);
                            break;

                        case '"':
                            string_append(token_string, "\\034");
                            break;

                        case '\'':
                            string_append(token_string, "\\039");
                            break;

                        case '\\':
                            string_append(token_string, "\\092");
                            break;

                        case 'n':
                            string_append(token_string, "\\010");
                            break;

                        case 't':
                            string_append(token_string, "\\009");
                            break;

                        default:
                            string_append(token_string, "\\092");
                            ungetc(c, stdin);
                            break;
                    }
                }
                else if(c == '\n'){
                    scn.line_counter++;
                    string_clear(tmp);
                    string_append_char(token_string, c);
                }
                //clear string which compares end of docstring, continue appending to string
                else {
                    string_clear(tmp);
                    string_append_char(token_string, c);        
                }
                //compare if we approached end of docstring
                if((strcmp(tmp->array, "\"\"\"")) == 0){
                    state = 0;
                    if(scn.new_line){
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
                        ungetc(c, stdin);
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
                        ungetc(c, stdin);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                }
                else{
                    ungetc(c, stdin);
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                return finish_free_resources(SUCCESS, token, tmp, token_string);
            //exponents or float values 
            case 5:      
                //number with floating point
                if(c == '.'){
                    token->type = TTYPE_DOUBLE;
                    //previous occurance of 'e' -> bad order in number (eg. 10e32.3)
                    if(str_find_char(tmp, 'e')){
                        fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Wrong order of exponent and floating point in number!\n", scn.line_counter);
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    //searching for previous occurances of '.'
                    if((str_find_char(tmp, c))){ 
                        fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Bad formatting of floating points in number!\n", scn.line_counter);
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    else{
                        string_append_char(tmp, c);
                    }
                }
                //exponent numbers
                else if(c == 'e' || c == 'E'){
                    token->type = TTYPE_DOUBLE;
                     //searching for previous occurances of 'e'
                    if(str_find_char(tmp, c)){
                        fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Bad formatting of number with exponent!\n", scn.line_counter);
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    else{
                        //e.g 203ebola 23e-blabla
                        string_append_char(tmp, c);
                        state = 8;
                    }
                }
                //+ -  can stand in middle of number after exponent only (e.g 10e-2)
                else if(c == '+' || c == '-'){
                    //if '+'/'-' appeared after int
                    if(token->type == TTYPE_INT){
                        ungetc(c ,stdin);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                    //latest char in tmp string is '.', return it to file stream and finish
                    if(tmp->array[tmp->index-1] == '.'){
                        fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Invalid number format!\n", scn.line_counter);
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    //latest char in tmp string is 'e'/'E', continue
                    if(tmp->array[tmp->index-1] == 'e'){
                        string_append_char(tmp, c);
                    }
                    else if(tmp->array[tmp->index-1] == 'E'){
                        string_append_char(tmp, c);
                    }
                    else{
                        ungetc(c, stdin);
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
                        fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Invalid number format!\n", scn.line_counter);
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    if(tmp->array[tmp->index-1] == 'e'){
                        ungetc('e', stdin);
                    }
                    if(tmp->array[tmp->index-1] == 'E'){
                        ungetc('E', stdin);
                    }
                    ungetc(c, stdin);
                    return finish_free_resources(SUCCESS, token, tmp, token_string);
                }
                break;
            //identifiers or keywords
            case 6:
                if(isalnum(c) || c == '_'){
                    string_append_char(token_string, c);
                }
                else{
                    ungetc(c, stdin);
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
                        token->type = TTYPE_NONE;
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
                else if (c == ' ') {
                    string_append(token_string, "\\032");
                }
                else if (c == '\t') {
                    string_append(token_string, "\\009");
                }
                else if (c == '#') {
                    string_append(token_string, "\\035");
                }
                else if(c == '\n' || c == EOF){
                    ungetc(c, stdin);
                    fprintf(stderr, "line %d: Lexical analysis error : "
                                    "String was not enclosed!\n", scn.line_counter);
                    return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);  
                }
                //escape sequence
                else if(c == '\\'){
                    c = getc(stdin);
                    switch (c) {
                        case 'x':
                            //hexa to decimal
                            string_append_char(token_string, '\\');
                            unsigned hexa = 0;
                            for (int i = 0; i < 2; i++) {
                                c = getc(stdin);
                                if (isxdigit(c)) {
                                    if (isdigit(c)) {
                                        hexa += (c - '0') * (i == 0 ? 16 : 1);

                                    } else if (isupper(c)) {
                                        hexa += (c - 'A' + 10) * (i == 0 ? 16 : 1);

                                    } else {
                                        hexa += (c - 'a' + 10) * (i == 0 ? 16 : 1);
                                    }

                                } else {
                                    fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Invalid escape sequence!\n", scn.line_counter);
                                    return finish_free_resources(ERROR_LEXICAL,
                                    token, tmp, token_string);  
                                }
                            }

                            char tmp[4] = {0};

                            if (hexa < 100)
                                sprintf(tmp, "0%d", hexa);
                            else 
                                sprintf(tmp, "%d", hexa);

                            string_append(token_string, tmp);
                            break;

                        case '"':
                            string_append(token_string, "\\034");
                            break;

                        case '\'':
                            string_append(token_string, "\\039");
                            break;

                        case '\\':
                            string_append(token_string, "\\092");
                            break;

                        case 'n':
                            string_append(token_string, "\\010");
                            break;

                        case 't':
                            string_append(token_string, "\\009");
                            break;

                        default:
                            string_append(token_string, "\\092");
                            ungetc(c, stdin);
                            break;
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
                        ungetc(c ,stdin);
                        if(tmp->array[tmp->index-1] == 'e'){
                            ungetc('e', stdin);
                        }
                        else if(tmp->array[tmp->index-1] == 'E') {
                            ungetc('E', stdin);
                        }
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                    break;
                case 9:
                    //checking bad number formats
                    if(isdigit(c)){
                        //after zero, nonzero number appeared -> error
                        string_append_char(tmp, c);
                        if(c != '0'){
                            state = 10;
                        }
                    }
                    else if(c == '.' || c == 'e' || c == 'E'){
                        token->type = TTYPE_DOUBLE;
                        string_append_char(tmp, c);
                        state = 5;
                    }
                    else{
                        ungetc(c, stdin);
                        return finish_free_resources(SUCCESS, token, tmp, token_string);
                    }
                    break;
                //state for checking invalid integer format 00003 -> invalid , 00003.0 -> valid
                case 10:
                    if(isdigit(c)){
                        string_append_char(tmp, c);
                    }
                    else if(c == '.' || c == 'e' || c == 'E'){
                        token->type = TTYPE_DOUBLE;
                        string_append_char(tmp, c);
                        state = 5;
                    }
                    else{
                        fprintf(stderr, "line %d: Lexical analysis error : "
                                        "Invalid number format!\n", scn.line_counter);
                        return finish_free_resources(ERROR_LEXICAL, token, tmp, token_string);
                    }
                    break;
        }
    }
}
