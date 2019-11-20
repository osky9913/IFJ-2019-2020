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
    dent_stack = stack_general_init();
    if (!dent_stack) return ERROR_INTERNAL;

    in_function = false;

    curr_token.type = TTYPE_EOF;

    token_stash[0].type = TTYPE_EOF;
    token_stash[0].attribute.string = NULL;
    token_stash[1].type = TTYPE_EOF;
    token_stash[1].attribute.string = NULL;
    return SUCCESS;
}

void free_resources() {
    next_token(true);
    next_token(true);
    next_token(true);
    stack_free(dent_stack);
    stash_clear();
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
                    fprintf(stderr, "UNEXPECTED_TOKEN 'return' in r_program.\n");
                    retvalue = UNEXPECTED_TOKEN;
                    break;

                case KEY_NONE:
                    if ((retvalue = r_statement()) == SUCCESS) {
                        retvalue = r_program();
                    }
                    break;

                default:
                    break;
            }
            break;

        case TTYPE_ID: case TTYPE_STR:
        case TTYPE_INT: case TTYPE_DOUBLE:
        case TTYPE_LTBRAC:
            if ((retvalue = r_statement()) == SUCCESS) {
                retvalue = r_program();
            }
            break;

        default:
            fprintf(stderr, "UNEXPECTED_TOKEN in r_program - not a statement or def.\n");
            retvalue = UNEXPECTED_TOKEN;
            break;
    }
    return retvalue;
}

int r_statement() {
    int retvalue = SUCCESS;

    switch (curr_token.type) {
        case TTYPE_ID: case TTYPE_LTBRAC: case TTYPE_INT: case TTYPE_DOUBLE:
        case TTYPE_STR:
            /* id, (, int, dbl, str, none */
            retvalue = r_value();
            break;

        case TTYPE_KEYWORD:
            switch (curr_token.attribute.keyword) {
                case KEY_RETURN:
                    retvalue = r_function_ret(); /* return */
                    break;

                case KEY_IF:
                    return r_if_else(); /* if-else */
                    break;

                case KEY_WHILE:
                    return r_cycle(); /* while */
                    break;

                case KEY_PASS:
                    retvalue = SUCCESS; /* pass */
                    break;

                case KEY_NONE:
                    retvalue = r_value();
                    break;

                default:
                    fprintf(stderr, "UNEXPECTED_TOKEN 'None' in r_statement.\n");
                    retvalue = UNEXPECTED_TOKEN;
                    break;
            }
            break;

        default:
            fprintf(stderr, "UNEXPECTED_TOKEN in r_statement.\n");
            retvalue = UNEXPECTED_TOKEN;
    }

    lex_check(next_token(true));
    if (curr_token.type != TTYPE_EOL) { /* eol */
        fprintf(stderr, "UNEXPECTED_TOKEN %d in r_statement - didn't end with eol.\n",
                curr_token.type);
        retvalue = UNEXPECTED_TOKEN;
    }

    return retvalue;
}
int r_statement_list() {
    int retvalue = SUCCESS;

    switch (curr_token.type) {
        case TTYPE_DEDENT: /* dedent -> eps*/
            return SUCCESS;
            break;

        case TTYPE_ID: /* id */
            retvalue = r_statement();
            break;

        case TTYPE_KEYWORD: 
            if (curr_token.attribute.keyword != KEY_DEF) {
                retvalue = r_statement();
            } else {
                retvalue = UNEXPECTED_TOKEN;
            }
            break;

        default:
            fprintf(stderr, "UNEXPECTED_TOKEN %d in r_statement_list.\n", curr_token.type);
            retvalue = UNEXPECTED_TOKEN;
            break;
    }

    if (retvalue == SUCCESS) {
        lex_check(next_token(false));
        retvalue = r_statement_list();
    }
    return retvalue;
}

int r_function_def() {
    int retvalue = UNEXPECTED_TOKEN;
    in_function = true;

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_ID) return UNEXPECTED_TOKEN;
    lex_check(next_token(false));
    if (curr_token.type != TTYPE_LTBRAC) return UNEXPECTED_TOKEN;  /* ( */

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
            fprintf(stderr, "UNEXPECTED_TOKEN in r_params_def.\n");
            retvalue = UNEXPECTED_TOKEN;
        }
    }

    return retvalue;
}

int r_param_list() {
    int retvalue = SUCCESS;

    switch (curr_token.type) {
        case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
        case TTYPE_DOUBLE: /* TERMS */
            lex_check(next_token(false));
            retvalue = r_params(); /* <params> */
            break;

        case TTYPE_KEYWORD:
            if (curr_token.attribute.keyword == KEY_NONE) { /* None */
                lex_check(next_token(false));
                retvalue = r_params(); /* <params> */
            }
            break;

        default:
            break;
    }

    return retvalue;
}

int r_params() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_COMMA) { /* , or eps */
        lex_check(next_token(false));

        switch (curr_token.type) {
            case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
            case TTYPE_DOUBLE: /* TERMS */
                lex_check(next_token(false));
                retvalue = r_params(); /* <params> */
                break;

            case TTYPE_KEYWORD:
                if (curr_token.attribute.keyword == KEY_NONE) { /* None */
                    lex_check(next_token(false));
                    retvalue = r_params(); /* <params> */
                }
                break;

            case TTYPE_RTBRAC:
                retvalue = SUCCESS;
                break;

            default:
                fprintf(stderr, "UNEXPECTED_TOKEN in r_params - wrong parameter.\n");
                retvalue = UNEXPECTED_TOKEN;
                break;
        }
    }

    return retvalue;
}

int r_if_else() {
    int retvalue = SUCCESS;

    if ((retvalue = psa()) != SUCCESS) return retvalue; /* if expr */

    lex_check(next_token(true));
    if (curr_token.type != TTYPE_COLUMN) return UNEXPECTED_TOKEN; /* : */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_EOL) return UNEXPECTED_TOKEN; /* eol */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_INDENT) return UNEXPECTED_TOKEN; /* indent */

    lex_check(next_token(false));
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    lex_check(next_token(false));
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (curr_token.type != TTYPE_DEDENT) return UNEXPECTED_TOKEN; /* dedent */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_KEYWORD) return UNEXPECTED_TOKEN;
    if (curr_token.attribute.keyword != KEY_ELSE) return UNEXPECTED_TOKEN; /* else */

    lex_check(next_token(true));
    if (curr_token.type != TTYPE_COLUMN) return UNEXPECTED_TOKEN; /* : */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_EOL) return UNEXPECTED_TOKEN; /* eol */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_INDENT) return UNEXPECTED_TOKEN; /* indent */

    lex_check(next_token(false));
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    lex_check(next_token(false));
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (curr_token.type != TTYPE_DEDENT) return UNEXPECTED_TOKEN; /* dedent */

    return retvalue;
}

int r_cycle() {
    int retvalue = SUCCESS;

    if ((retvalue = psa()) != SUCCESS) return retvalue; /* while expr */

    lex_check(next_token(true));
    if (curr_token.type != TTYPE_COLUMN) return UNEXPECTED_TOKEN; /* : */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_EOL) return UNEXPECTED_TOKEN; /* eol */

    lex_check(next_token(false));
    if (curr_token.type != TTYPE_INDENT) return UNEXPECTED_TOKEN; /* indent */

    lex_check(next_token(false));
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    lex_check(next_token(false));
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (curr_token.type != TTYPE_DEDENT) return UNEXPECTED_TOKEN; /* dedent */

    return retvalue;
}

int r_function_ret() {
    if (!in_function)
        return RETURN_IN_PROGRAM_BODY; /* return statement cannot exist outside a
                                          function definition */
    lex_check(next_token(false));
    return r_retvalue();
}

int r_retvalue() {
    int retvalue = UNEXPECTED_TOKEN;

    if (curr_token.type == TTYPE_EOL) { /* eps */
        unget_token();
        retvalue = SUCCESS;

    } else { /* Call psa, I want no part in this... */
        unget_token();
        retvalue = psa();
    }

    return retvalue;
}

int r_value() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_ID) { /* id, can be function call */
        unget_token();
        lex_check(next_token(false));
        retvalue = r_rest();

    } else { /* expression */
        unget_token();
        retvalue = psa();
    }

    return retvalue;
}

int r_rest() {
    int retvalue = UNEXPECTED_TOKEN;

    switch (curr_token.type) {
        case TTYPE_LTBRAC:
            stash_clear();
            retvalue = r_function_call();
            break;

        case TTYPE_ASSIGN: /* assignment - we need to check if the assigned value
                              comes from an expression or a function call */

            stash_clear();
            lex_check(next_token(false));
            unget_token();
            if (curr_token.type == TTYPE_ID) {

                lex_check(next_token(false));
                if (curr_token.type == TTYPE_LTBRAC) {
                    stash_clear();
                    retvalue = r_function_call();

                } else {
                    unget_token();
                    retvalue = psa();
                }

            } else {
                retvalue = psa();
            }

            break;

        default: /* expression */
            unget_token();
            retvalue = psa();
            break;
    }

    return retvalue;
}

int r_function_call() {
    int retvalue = UNEXPECTED_TOKEN;
    if (curr_token.type != TTYPE_LTBRAC) return UNEXPECTED_TOKEN;

    lex_check(next_token(false));
    if ((retvalue = r_param_list()) != SUCCESS) return retvalue;
    if (curr_token.type != TTYPE_RTBRAC) return UNEXPECTED_TOKEN;

    return retvalue;
}

int r_term() {
    int retvalue = UNEXPECTED_TOKEN;

    if (curr_token.type == TTYPE_ID) { /* id */
        retvalue = SUCCESS;

    } else { /* probably literal */
        retvalue = r_literal();
    }

    return retvalue;
}

int r_literal() {
    int retvalue = UNEXPECTED_TOKEN;
    switch (curr_token.type) {
        case TTYPE_INT: case TTYPE_DOUBLE: case TTYPE_STR:
            retvalue = SUCCESS;
            break;

        case TTYPE_KEYWORD:
            if (curr_token.attribute.keyword == KEY_NONE)
                retvalue = SUCCESS;
            break;

        default:
            fprintf(stderr, "UNEXPECTED_TOKEN in r_literal - not a literal .\n");
            retvalue = UNEXPECTED_TOKEN;
            break;
    }

    return retvalue;
}


int next_token(bool load_from_stash) {
    int retvalue;

    /* Free token string attribute if loading with get_token and stash is empty */
    if (stash_empty()
            || (token_stash[0].type == TTYPE_EOF && token_stash[1].type != TTYPE_EOF)) {
        token_free(&curr_token);
    }

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


    retvalue = get_token(&curr_token); // Get next token
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

void stash_clear() {
    for (int i = 0; i < 2; i++) {
        token_free(&token_stash[i]);
        token_stash[i].type = TTYPE_EOF;
    }
}

void token_free(token_t *token) {
        if (token->type == TTYPE_STR || token->type == TTYPE_ID) {

            free(token->attribute.string);
            token->attribute.string = NULL;
        }
}

bool stash_empty() {
    return token_stash[0].type == TTYPE_EOF && token_stash[1].type == TTYPE_EOF;
}

void lex_check(int retcode) {
    if (retcode != SUCCESS) {
        free_resources();
        fprintf(stderr, "Lex error %d\n", retcode);
        exit(ERROR_LEXICAL);
    }
}
