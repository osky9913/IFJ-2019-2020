/**
 *	@file   parser.c
 *	@author Simon Sedlacek, xsedla1h
 *	@date
 *	@brief Parser 
 */

#include "parser.h"

/* Some global variables */
bool in_function = false;
token_t curr_token = { .type = TTYPE_EOF };
token_t token_stash[2];
symbol_t *curr_function_def = NULL;
symbol_t *curr_function_call = NULL;
int param_count = 0;
symbol_t *undef_symbol = NULL; // maybe make global
psa_state_t psa_state = DEFAULT;

int init_resources() {
    /* Initialize the indent stack for scanner */
    dent_stack = stack_general_init();
    if (!dent_stack) return ERROR_INTERNAL;

    /* Initialize the token stash */
    token_stash[0].type = TTYPE_EOF;
    token_stash[0].attribute.string = NULL;
    token_stash[1].type = TTYPE_EOF;
    token_stash[1].attribute.string = NULL;

    /* Initialize symbol tables */
    symtable_init(&table_global);
    symtable_init(&table_local);

    if (add_built_in_functions()) return ERROR_INTERNAL;

    start_program();
    return SUCCESS;
}

void free_resources() {
    token_free(&curr_token);
    token_free(&token_stash[0]);
    token_free(&token_stash[1]);

    stack_free(dent_stack);
    stash_clear();

    symtable_clear_all(&table_global);
    symtable_clear_all(&table_local);

    free_finals_string();
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
                    fprintf(stderr, "Line %d - ERROR_SYNTAX 'return' in r_program.\n",
                            line_counter);
                    retvalue = ERROR_SYNTAX;
                    break;

                default:
                    break;
            }
            break;

        case TTYPE_ID: case TTYPE_STR:
        case TTYPE_INT: case TTYPE_DOUBLE:
        case TTYPE_LTBRAC: case TTYPE_NONE:
            if ((retvalue = r_statement()) == SUCCESS) {
                retvalue = r_program();
            }
            break;

        default:
            fprintf(stderr, "Line %d - ERROR_SYNTAX in r_program "
                    "- not a statement or def.\n", line_counter);
            retvalue = ERROR_SYNTAX;
            break;
    }
    return retvalue;
}

int r_statement() {
    int retvalue = SUCCESS;

    switch (curr_token.type) {
        case TTYPE_ID: case TTYPE_LTBRAC: case TTYPE_INT: case TTYPE_DOUBLE:
        case TTYPE_STR: case TTYPE_NONE:
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

                case KEY_WHILE:
                    return r_cycle(); /* while */

                case KEY_PASS:
                    retvalue = SUCCESS; /* pass */
                    break;

                default:
                    fprintf(stderr, "Line %d - ERROR_SYNTAX 'None' in r_statement.\n",
                            line_counter);
                    retvalue = ERROR_SYNTAX;
                    break;
            }
            break;

        default:
            fprintf(stderr, "Line %d - ERROR_SYNTAX in r_statement.\n", line_counter);
            retvalue = ERROR_SYNTAX;
    }

    if (retvalue != SUCCESS) return retvalue;

    next_token(true);
    if (curr_token.type != TTYPE_EOL) { /* eol */
        fprintf(stderr, "Line %d - ERROR_SYNTAX %d in r_statement - didn't "
                "end with eol.\n", line_counter, curr_token.type);
        retvalue = ERROR_SYNTAX;
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
                retvalue = ERROR_SYNTAX;
            }
            break;

        default:
            fprintf(stderr, "Line %d - ERROR_SYNTAX %d in r_statement_list.\n",
                    line_counter, curr_token.type);
            retvalue = ERROR_SYNTAX;
            break;
    }

    if (retvalue == SUCCESS) {
        next_token(false);
        retvalue = r_statement_list();
    }
    return retvalue;
}

int r_function_def() {
    int retvalue = ERROR_SYNTAX;
    in_function = true;

    next_token(false);
    if (curr_token.type != TTYPE_ID) return ERROR_SYNTAX;

    /* Let's try to add the function to our symtable */
    if ((retvalue = define_function(curr_token.attribute.string)) != SUCCESS)
        return retvalue;

    generate_function(&curr_token);

    next_token(false);
    if (curr_token.type != TTYPE_LTBRAC) return ERROR_SYNTAX;  /* ( */

    next_token(false);
    retvalue = r_param_list_def(); /* <param_list_def> */
    if (retvalue != SUCCESS) return retvalue;

    /* Check parameter count */
    if ((param_count != curr_function_def->attributes.func_att.param_count) &&
            (curr_function_def->attributes.func_att.param_count >= 0)) {
        fprintf(stderr, "Line %d - Semantic error: Wrong parameter "
                "count when defining function '%s'.\n", line_counter,
                curr_function_def->id);
        return ERROR_SEM_PARAM_COUNT;
    }

    /* Store the number of parameters for the function */
    curr_function_def->attributes.func_att.param_count = param_count;
    param_count = 0;

    /* params_list has already loaded this token */
    if (curr_token.type != TTYPE_RTBRAC) return ERROR_SYNTAX; /* ) */

    next_token(false);
    if (curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */
    next_token(false);
    if (curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* EOL */
    next_token(false);
    if (curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* INDENT */

    next_token(false);
    retvalue = r_statement(); /* <statement> */
    if (retvalue != SUCCESS) return retvalue;

    next_token(false);
    retvalue = r_statement_list(); /* <statement_list> */
    if (retvalue != SUCCESS) return retvalue;

    if (curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* DEDENT */
    
    in_function = false;
    symtable_clear_all(&table_local);

    return retvalue;
}

int r_param_list_def() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_ID) { /* id */
        
        /* Add the parameters to the local symtable */
        if ((retvalue = check_and_add_parameter_def(curr_token.attribute.string))
                != SUCCESS) {
            return retvalue;
        }

        generate_def_param(&curr_token);

        param_count++;
        next_token(false);
        retvalue = r_params_def(); /* <params_def> */
    }

    return retvalue;
}
int r_params_def() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_COMMA) { /* , */
        param_count++;
        next_token(false);

        if (curr_token.type == TTYPE_ID) { /* id */
            /* Add the parameters to the local symtable */
            if ((retvalue = check_and_add_parameter_def(curr_token.attribute.string))
                    != SUCCESS) {
                return retvalue;
            }

            generate_def_param(&curr_token);

            next_token(false);
            retvalue = r_params_def(); /* <params_def> */

        } else {
            fprintf(stderr, "Line %d - ERROR_SYNTAX in r_params_def.\n", line_counter);
            retvalue = ERROR_SYNTAX;
        }
    }

    return retvalue;
}

int r_param_list() {
    int retvalue = SUCCESS;

    switch (curr_token.type) {
        case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
        case TTYPE_DOUBLE: case TTYPE_NONE: /* TERMS */

            /* Check if the parameter was a defined variable */
            if (check_parameter_valid(curr_token, undef_symbol ? undef_symbol->id : NULL))
                return ERROR_SEM_DEFINITION;

            generate_call_param(&curr_token);

            param_count++;
            next_token(false);
            retvalue = r_params(); /* <params> */
            break;

        default:
            break;
    }

    return retvalue;
}

int r_params() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_COMMA) { /* , or eps */
        param_count++;
        next_token(false);

        switch (curr_token.type) {
            case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
            case TTYPE_DOUBLE: case TTYPE_NONE: /* TERMS */

                /* Check if the parameter was a defined variable */
                if (check_parameter_valid(curr_token,
                            undef_symbol ? undef_symbol->id : NULL))
                    return ERROR_SEM_DEFINITION;

                generate_call_param(&curr_token);

                next_token(false);
                retvalue = r_params(); /* <params> */
                break;

            default:
                fprintf(stderr, "Line %d - ERROR_SYNTAX in r_params - "
                        "wrong parameter.\n", line_counter);
                retvalue = ERROR_SYNTAX;
                break;
        }
    }

    return retvalue;
}

int r_if_else() {
    int retvalue = SUCCESS;
    psa_state = IF;
    
    //GEN if head

    if ((retvalue = psa(undef_symbol ? undef_symbol->id : NULL)) != SUCCESS) return retvalue; /* if expr */

    psa_state = DEFAULT;

    next_token(true);
    if (curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */

    next_token(false);
    if (curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* eol */

    next_token(false);
    if (curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* indent */

    next_token(false);
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    next_token(false);
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* dedent */

    next_token(false);
    if (curr_token.type != TTYPE_KEYWORD) return ERROR_SYNTAX;
    if (curr_token.attribute.keyword != KEY_ELSE) return ERROR_SYNTAX; /* else */

    next_token(true);
    if (curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */

    next_token(false);
    if (curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* eol */

    next_token(false);
    if (curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* indent */

    generate_else();

    next_token(false);
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    next_token(false);
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* dedent */

    // GEN ELSE END 

    return retvalue;
}

int r_cycle() {
    int retvalue = SUCCESS;
    psa_state = WHILE;
    
    generate_while_lable();

    if ((retvalue = psa(undef_symbol ? undef_symbol->id : NULL)) != SUCCESS) return retvalue; /* while expr */

    psa_state = DEFAULT;

    next_token(true);
    if (curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */

    next_token(false);
    if (curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* eol */

    next_token(false);
    if (curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* indent */

    next_token(false);
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    next_token(false);
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* dedent */

    generate_while_end();

    return retvalue;
}

int r_function_ret() {
    if (!in_function)
        return ERROR_SYNTAX; /* return statement cannot exist outside a
                                          function definition */
    psa_state = RETURN;
    next_token(false);
    return r_retvalue();
}

int r_retvalue() {
    int retvalue = ERROR_SYNTAX;

    if (curr_token.type == TTYPE_EOL) { /* eps */
        unget_token();
        retvalue = SUCCESS;

    } else { /* Call psa, I want no part in this... */
        unget_token();
        retvalue = psa(NULL);
    }

    generate_function_end();
    psa_state = DEFAULT;
    return retvalue;
}

int r_value() {
    int retvalue = SUCCESS;

    if (curr_token.type == TTYPE_ID) { /* id, can be function call */
        unget_token();
        next_token(false);
        retvalue = r_rest();

    } else { /* expression */
        unget_token();
        retvalue = psa(NULL);
    }

    return retvalue;
}

int r_rest() {
    int retvalue = ERROR_SYNTAX;

    switch (curr_token.type) {
        case TTYPE_LTBRAC:
            unget_token();

            /* the statement is a function call, let's check the semantics */
            next_token(true); /* First load the last token */
            if ((retvalue = check_function_call(curr_token.attribute.string)) != SUCCESS)
                return retvalue;
               
            generate_create_frame();

            next_token(true);
            if((retvalue = r_function_call()) != SUCCESS) return retvalue;

            generate_call_function(&curr_function_call);

            /* Check parameter count */
            if((retvalue = check_parameter_count_call(param_count)) != SUCCESS)
                return retvalue;
            param_count = 0;

            break;

        case TTYPE_ASSIGN: /* assignment - we need to check if the assigned value
                              comes from an expression or a function call */

            psa_state = ASSIGN;
            next_token(true);

            declaration_variable(&curr_token);

            /* This block ensures that a construction like bar = foo(bar) is not
             * valid before defining bar */
            retvalue = add_symbol_var(curr_token.attribute.string);
            switch (retvalue) {
                case NEW_VARIABLE:
                    if (in_function)
                        undef_symbol = symtable_search(&table_local,
                                curr_token.attribute.string);
                    else
                        undef_symbol = symtable_search(&table_global,
                                curr_token.attribute.string);
                    
                    undef_symbol->attributes.var_att.defined = false;

                    retvalue = SUCCESS;
                    break;

                case SUCCESS:
                    if (in_function)
                        undef_symbol = symtable_search(&table_local,
                                curr_token.attribute.string);
                    else
                        undef_symbol = symtable_search(&table_global,
                                curr_token.attribute.string);
                    break;

                default:
                    return retvalue;
            }

            stash_clear();
            next_token(false);
            unget_token();
            if (curr_token.type == TTYPE_ID) {

                next_token(false);
                if (curr_token.type == TTYPE_LTBRAC) {
                    unget_token();

                    /* the statement is a function call, let's check the semantics */
                    next_token(true);
                    if ((retvalue = check_function_call(curr_token.attribute.string))
                            != SUCCESS)
                        return retvalue;

                    generate_create_frame();

                    next_token(true);
                    if((retvalue = r_function_call()) != SUCCESS) return retvalue;

                    generate_call_function(&curr_function_call);

                    /* Check parameter count */
                    if ((retvalue = check_parameter_count_call(param_count)))
                        return retvalue;
                    param_count = 0;

                } else {
                    unget_token();
                    retvalue = psa(undef_symbol ? undef_symbol->id : NULL);
                }

            } else {
                retvalue = psa(undef_symbol ? undef_symbol->id : NULL);
            }

            if (undef_symbol) {
                undef_symbol->attributes.var_att.defined = true;
                undef_symbol = NULL;
            }

            psa_state = DEFAULT;
            break;

        default: /* expression */
            unget_token();
            retvalue = psa(undef_symbol ? undef_symbol->id : NULL);
            break;
    }

    return retvalue;
}

int r_function_call() {
    int retvalue = ERROR_SYNTAX;
    if (curr_token.type != TTYPE_LTBRAC) return ERROR_SYNTAX;

    next_token(false);
    if ((retvalue = r_param_list()) != SUCCESS) return retvalue;
    if (curr_token.type != TTYPE_RTBRAC) return ERROR_SYNTAX;

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
    if (retvalue != SUCCESS) {
        free_resources();
        fprintf(stderr, "Line %d - Lex error %d\n", line_counter, retvalue);
        exit(ERROR_LEXICAL);
    }

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
