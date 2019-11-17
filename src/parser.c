/**
 *	@file   parser.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Parser 
 */

#include "parser.h"

bool in_function = false;
token_t curr_token = {.type =TTYPE_EOF};
token_t token_stash[2] = {{.type = TTYPE_EOF}, {.type = TTYPE_EOF}};


int init_resources() {
    return 0;
}
void free_resources() {
}

int r_program() {
    int retvalue = 0;
    next_token(false);

    switch (curr_token.type) {
        case TTYPE_EOF:
            retvalue = SUCCESS; // End of input - EOF represents the '$' token
            break;

        case TTYPE_KEYWORD:
            switch (curr_token.attribute.keyword) {
                case KEY_DEF:
                    if ((retvalue = r_function_def()) == SUCCESS) {
                        retvalue = r_program();
                    }
                    break;

                case KEY_IF: case KEY_PASS: case KEY_WHILE:
                    if ((retvalue = r_statement()) == SUCCESS) {
                        retvalue = r_program();
                    }
                    break;

                case KEY_RETURN:
                    retvalue = UNEXPECTED_TOKEN;

                default:
                    break;
            }
            break;

        case TTYPE_ID: case TTYPE_STR: case TTYPE_DOCSTR:
        case TTYPE_INT: case TTYPE_DOUBLE: case TTYPE_NONE:
        case TTYPE_LTBRAC:
            if ((retvalue = r_statement()) == SUCCESS) {
                retvalue = r_program();
            }
            break;

        default:
            retvalue = UNEXPECTED_TOKEN;
            break;
    }
    return retvalue;
}
int r_statement() {
    int retvalue = 0;
   // token_type_t type = current_token.type;
   // token_type_t att = current_token.attribute;

   // switch (type) {
   //     case TTYPE_ID:
   //         break;
   //     default:
   //         retvalue = UNEXPECTED_TOKEN;
   //         break;
   // }
    return retvalue;
}
int r_statement_list() {
}
int r_function_def() {
    int retvalue = 0;
    //next_token();
    //if (type != TTYPE_ID) return UNEXPECTED_TOKEN;

    //next_token();
    
    return retvalue;
}
int r_param_list() {
}
int r_params() {
}
int r_if_else() {
}
int r_cycle() {
}
int r_function_ret() {
}
int r_retvalue() {
}
int r_value() {
}
int r_rest() {
}
int r_function_call() {
}
int r_term() {
}


int next_token(bool load_from_stash) {

    if (load_from_stash) {
        if (token_stash[0].type != TTYPE_EOF) {
            curr_token = token_stash[0];
            token_stash[0].type = TTYPE_EOF;
            return SUCCESS;
        } else if (token_stash[1].type != TTYPE_EOF) {
            curr_token = token_stash[1];
            token_stash[1].type = TTYPE_EOF;
            return SUCCESS;
        }
    } 

    /* Free token string attribute if loading with get_token */
    if (curr_token.type == TTYPE_STR || curr_token.type == TTYPE_DOCSTR || curr_token.type == TTYPE_ID)
        free(curr_token.attribute.string);

    int retvalue = get_token(stdin, &curr_token); // Get next token
    if (retvalue != 0)
        printf("token error\n"); // free resources, exit
    return retvalue;
}

void unget_token() {
    if (token_stash[0].type == TTYPE_EOF) {
        token_stash[0] = curr_token;
    } else {
        token_stash[1] = curr_token;
    }
}
