/**
 *	@file   parser.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Parser 
 */

#include "parser.h"

bool in_function;
token_t curr_token;
token_t token_stash[2];


int init_resources() {
    dent_stack = indent_stack_init();
    if (!dent_stack) return ERROR_INTERNAL;
    in_function = false;
    curr_token.type = TTYPE_EOF;
    token_stash[0].type = TTYPE_EOF;
    token_stash[1].type = TTYPE_EOF;
}

void free_resources() {
    next_token(true);
    next_token(true);
    next_token(true);
    indent_stack_free(dent_stack);
}

int r_program() {
    int retvalue = 0;
    lex_check(next_token(false));

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
    int retvalue;
    if (curr_token.type == TTYPE_ID) return SUCCESS;
    else retvalue = UNEXPECTED_TOKEN;
    next_token(false);
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
    return SUCCESS;
}

int r_function_def() {
    int retvalue = UNEXPECTED_TOKEN;
    in_function = true;

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_ID) return UNEXPECTED_TOKEN;
    lex_check(next_token(false));
    if (curr_token.type != TTYPE_LTBRAC) return UNEXPECTED_TOKEN;  /* ) */

    lex_check(next_token(false)); // TODO: fix param list
    retvalue = r_param_list_def(); /* <param_list_def> */
    if (retvalue != SUCCESS) return retvalue;

    /* params_list has already loaded this token */
    if (curr_token.type != TTYPE_RTBRAC) return UNEXPECTED_TOKEN; /* ) */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_COLUMN) return UNEXPECTED_TOKEN; /* : */
    lex_check(next_token(false));
    if (curr_token.type != TTYPE_EOL) return UNEXPECTED_TOKEN; /* EOL */
    lex_check(next_token(false));
    if (curr_token.type != TTYPE_INDENT) return UNEXPECTED_TOKEN; /* INDENT */

    lex_check(next_token(false));
    retvalue = r_statement(); /* <statement> */
    if (retvalue != SUCCESS) return retvalue;

    lex_check(next_token(false));
    retvalue = r_statement_list(); /* <statement_list> */
    if (retvalue != SUCCESS) return retvalue;

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_DEDENT) return UNEXPECTED_TOKEN; /* DEDENT */
    
    in_function = false;
    return retvalue;
}

int r_param_list_def() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_ID) { /* id */
        lex_check(next_token(false));
        retvalue = r_params_def(); /* <params_def> */
    }

    return retvalue;
}
int r_params_def() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_COMMA) { /* , */
        lex_check(next_token(false));
        if (curr_token.type == TTYPE_ID) { /* id */
            lex_check(next_token(false));
            retvalue = r_params_def(); /* <params_def> */
        } else {
            retvalue = UNEXPECTED_TOKEN;
        }
    }

    return retvalue;
}
int r_param_list() {
    int retvalue = SUCCESS;

    switch (curr_token.type) {
        case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
        case TTYPE_DOCSTR: case TTYPE_DOUBLE: case TTYPE_NONE: /* TERMS */
            lex_check(next_token(false));
            retvalue = r_params(); /* <params> */
        default:
            break;
    }

    return retvalue;
}
int r_params() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_COMMA) { /* , */
        lex_check(next_token(false));

        switch (curr_token.type) {
            case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
            case TTYPE_DOCSTR: case TTYPE_DOUBLE: case TTYPE_NONE: /* TERMS */
                lex_check(next_token(false));
                retvalue = r_params(); /* <params> */
                break;

            default:
                retvalue = UNEXPECTED_TOKEN;
                break;
        }
    }

    return retvalue;
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
    if (curr_token.type == TTYPE_STR
            || curr_token.type == TTYPE_DOCSTR
            || curr_token.type == TTYPE_ID)
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

void lex_check(int retcode) {
    if (retcode != SUCCESS) {
        free_resources();
        fprintf(stderr, "Lex error %d\n", retcode);
        exit(retcode);
    }
}
