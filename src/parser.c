/**
 * Implementace překladače imperativního jazyka IFJ19
 * @file   parser.c
 * @author Simon Sedlacek, xsedla1h
 * @brief The main parser module
 */

#include "parser.h"

/* Parser data global structure */
pdata_t prg = {
    .in_function = false,
    .curr_token.type = TTYPE_EOF,
    .curr_function_def = NULL,
    .curr_function_call = NULL,
    .param_count = 0,
    .undef_symbol = NULL,
    .psa_state = DEFAULT
    };

int r_program() {
    int retvalue = ERROR_SYNTAX;
    next_token(false);

    switch (prg.curr_token.type) {
        case TTYPE_EOF:
            retvalue = SUCCESS; /* End of input - EOF represents the '$' token */
            break;

        case TTYPE_KEYWORD:
            switch (prg.curr_token.attribute.keyword) {
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

                /* A return statement cannot occur in the main program body */
                case KEY_RETURN:
                    fprintf(stderr, "Line %d - Syntax error: 'return' in r_program.\n",
                            scn.line_counter);
                    retvalue = ERROR_SYNTAX;
                    break;

                default:
                    fprintf(stderr, "Line %d - Syntax error: unexpected keyword.\n",
                            scn.line_counter);
                    retvalue = ERROR_SYNTAX;
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
                    "- not a statement or def.\n", scn.line_counter);
            retvalue = ERROR_SYNTAX;
            break;
    }
    /* concatenate the code and the variable declarations */
    if (concat_main_stash()) return ERROR_INTERNAL;
    return retvalue;
}

int r_statement() {
    int retvalue = SUCCESS;

    switch (prg.curr_token.type) {
        case TTYPE_ID: case TTYPE_LTBRAC: case TTYPE_INT: case TTYPE_DOUBLE:
        case TTYPE_STR: case TTYPE_NONE:
            /* id, (, int, dbl, str, none */
            retvalue = r_value();
            break;

        case TTYPE_KEYWORD:
            switch (prg.curr_token.attribute.keyword) {
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
                            scn.line_counter);
                    retvalue = ERROR_SYNTAX;
                    break;
            }
            break;

        default:
            fprintf(stderr, "Line %d - ERROR_SYNTAX in r_statement.\n", scn.line_counter);
            retvalue = ERROR_SYNTAX;
    }

    if (retvalue != SUCCESS) return retvalue;

    next_token(true);
    /* Every statement has to end with EOL */
    if (prg.curr_token.type != TTYPE_EOL) { /* eol */
        fprintf(stderr, "Line %d - ERROR_SYNTAX %d in r_statement - didn't "
                "end with eol.\n", scn.line_counter, prg.curr_token.type);
        retvalue = ERROR_SYNTAX;
    }

    return retvalue;
}
int r_statement_list() {
    int retvalue = SUCCESS;

    switch (prg.curr_token.type) {
        case TTYPE_DEDENT: /* dedent -> eps*/
            return SUCCESS;
            break;

        case TTYPE_ID: /* id */
            retvalue = r_statement();
            break;

        case TTYPE_KEYWORD: 
            /* Care for nested definitions */
            if (prg.curr_token.attribute.keyword != KEY_DEF) { 
                retvalue = r_statement();
            } else {
                retvalue = ERROR_SYNTAX;
            }
            break;

        default:
            fprintf(stderr, "Line %d - ERROR_SYNTAX %d in r_statement_list.\n",
                    scn.line_counter, prg.curr_token.type);
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
    prg.in_function = true;

    /* Append the main program code, we're switching output string */
    if ((retvalue = concat_main_stash()) != SUCCESS) return retvalue;

    next_token(false);
    if (prg.curr_token.type != TTYPE_ID) return ERROR_SYNTAX;

    /* Let's try to add the function to our symtable */
    if ((retvalue = define_function(prg.curr_token.attribute.string)) != SUCCESS)
        return retvalue;

    generate_function(&prg.curr_token);

    next_token(false);
    if (prg.curr_token.type != TTYPE_LTBRAC) return ERROR_SYNTAX;  /* ( */

    next_token(false);
    retvalue = r_param_list_def(); /* <param_list_def> */
    if (retvalue != SUCCESS) return retvalue;

    /* Check parameter count */
    if ((prg.param_count != prg.curr_function_def->attributes.func_att.param_count) &&
            (prg.curr_function_def->attributes.func_att.param_count >= 0)) {
        fprintf(stderr, "Line %d - Semantic error: Wrong parameter "
                "count when defining function '%s'.\n", scn.line_counter,
                prg.curr_function_def->id);
        return ERROR_SEM_PARAM_COUNT;
    }

    /* Store the number of parameters for the function */
    prg.curr_function_def->attributes.func_att.param_count = prg.param_count;
    prg.param_count = 0;

    /* params_list has already loaded this token */
    if (prg.curr_token.type != TTYPE_RTBRAC) return ERROR_SYNTAX; /* ) */

    next_token(false);
    if (prg.curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */
    next_token(false);
    if (prg.curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* EOL */
    next_token(false);
    if (prg.curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* INDENT */

    next_token(false);
    retvalue = r_statement(); /* <statement> */
    if (retvalue != SUCCESS) return retvalue;

    next_token(false);
    retvalue = r_statement_list(); /* <statement_list> */
    if (retvalue != SUCCESS) return retvalue;

    generate_function_end();

    if (prg.curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* DEDENT */

    /* Append the function program code, we're switching output string */
    if ((retvalue = concat_function_stash()) != SUCCESS) return retvalue;
    
    prg.in_function = false;
    symtable_clear_all(&prg.table_local);

    return retvalue;
}

int r_param_list_def() {
    int retvalue = SUCCESS;

    if (prg.curr_token.type == TTYPE_ID) { /* id */
        
        /* Add the parameters to the local symtable */
        if ((retvalue = check_and_add_parameter_def(prg.curr_token.attribute.string))
                != SUCCESS) {
            return retvalue;
        }

        generate_def_param(&prg.curr_token);

        prg.param_count++;
        next_token(false);
        retvalue = r_params_def(); /* <params_def> */
    }

    return retvalue;
}
int r_params_def() {
    int retvalue = SUCCESS;

    if (prg.curr_token.type == TTYPE_COMMA) { /* , */
        prg.param_count++;
        next_token(false);

        if (prg.curr_token.type == TTYPE_ID) { /* id */
            /* Add the parameters to the local symtable */
            if ((retvalue = check_and_add_parameter_def(prg.curr_token.attribute.string))
                    != SUCCESS) {
                return retvalue;
            }

            generate_def_param(&prg.curr_token);

            next_token(false);
            retvalue = r_params_def(); /* <params_def> */

        } else {
            fprintf(stderr, "Line %d - ERROR_SYNTAX in r_params_def.\n", scn.line_counter);
            retvalue = ERROR_SYNTAX;
        }
    }

    return retvalue;
}

int r_param_list() {
    int retvalue = SUCCESS;

    switch (prg.curr_token.type) {
        case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
        case TTYPE_DOUBLE: case TTYPE_NONE: /* TERMS */

            /* Check if the parameter was a defined variable */
            if (check_parameter_valid(prg.curr_token, prg.undef_symbol ? prg.undef_symbol->id : NULL))
                return ERROR_SEM_DEFINITION;

            generate_call_param(&prg.curr_token);

            prg.param_count++;
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

    if (prg.curr_token.type == TTYPE_COMMA) { /* , or eps */
        prg.param_count++;
        next_token(false);

        switch (prg.curr_token.type) {
            case TTYPE_ID: case TTYPE_INT: case TTYPE_STR:
            case TTYPE_DOUBLE: case TTYPE_NONE: /* TERMS */

                /* Check if the parameter was a defined variable */
                if (check_parameter_valid(prg.curr_token,
                            prg.undef_symbol ? prg.undef_symbol->id : NULL))
                    return ERROR_SEM_DEFINITION;

                generate_call_param(&prg.curr_token);

                next_token(false);
                retvalue = r_params(); /* <params> */
                break;

            default:
                fprintf(stderr, "Line %d - ERROR_SYNTAX in r_params - "
                        "wrong parameter.\n", scn.line_counter);
                retvalue = ERROR_SYNTAX;
                break;
        }
    }

    return retvalue;
}

int r_if_else() {
    int retvalue = SUCCESS;
    prg.psa_state = IF;
    
    if ((retvalue = psa(prg.undef_symbol ? prg.undef_symbol->id : NULL)) != SUCCESS) return retvalue; /* if expr */

    prg.psa_state = DEFAULT;

    next_token(true);
    if (prg.curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */

    next_token(false);
    if (prg.curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* eol */

    next_token(false);
    if (prg.curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* indent */

    next_token(false);
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    next_token(false);
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (prg.curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* dedent */

    next_token(false);
    if (prg.curr_token.type != TTYPE_KEYWORD) return ERROR_SYNTAX;
    if (prg.curr_token.attribute.keyword != KEY_ELSE) return ERROR_SYNTAX; /* else */

    next_token(true);
    if (prg.curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */

    next_token(false);
    if (prg.curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* eol */

    next_token(false);
    if (prg.curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* indent */

    generate_else();

    next_token(false);
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    next_token(false);
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (prg.curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* dedent */

    generate_elseif_end();

    return retvalue;
}

int r_cycle() {
    int retvalue = SUCCESS;
    prg.psa_state = WHILE;
    
    generate_while_label();

    if ((retvalue = psa(prg.undef_symbol ? prg.undef_symbol->id : NULL)) != SUCCESS) return retvalue; /* while expr */

    prg.psa_state = DEFAULT;

    next_token(true);
    if (prg.curr_token.type != TTYPE_COLUMN) return ERROR_SYNTAX; /* : */

    next_token(false);
    if (prg.curr_token.type != TTYPE_EOL) return ERROR_SYNTAX; /* eol */

    next_token(false);
    if (prg.curr_token.type != TTYPE_INDENT) return ERROR_SYNTAX; /* indent */

    next_token(false);
    if ((retvalue = r_statement()) != SUCCESS) return retvalue; /* statement */

    next_token(false);
    if ((retvalue = r_statement_list()) != SUCCESS) return retvalue; /* statement_list */
    if (prg.curr_token.type != TTYPE_DEDENT) return ERROR_SYNTAX; /* dedent */

    generate_while_end();

    return retvalue;
}

int r_function_ret() {
    if (!prg.in_function)
        return ERROR_SYNTAX; /* return statement cannot exist outside a
                                          function definition */
    prg.psa_state = RETURN;
    next_token(false);
    return r_retvalue();
}

int r_retvalue() {
    int retvalue = ERROR_SYNTAX;

    if (prg.curr_token.type == TTYPE_EOL) { /* eps */
        unget_token();
        retvalue = SUCCESS;

    } else { /* Call psa, I want no part in this... */
        unget_token();
        retvalue = psa(NULL);
    }

    generate_function_end();
    prg.psa_state = DEFAULT;
    return retvalue;
}

int r_value() {
    int retvalue = SUCCESS;

    if (prg.curr_token.type == TTYPE_ID) { /* id, can be function call */
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

    switch (prg.curr_token.type) {
        case TTYPE_LTBRAC:
            unget_token();

            /* the statement is a function call, let's check the semantics */
            next_token(true); /* First load the last token */
            if ((retvalue = check_function_call(prg.curr_token.attribute.string)) != SUCCESS)
                return retvalue;
               
            generate_create_frame();

            next_token(true);
            if((retvalue = r_function_call()) != SUCCESS) return retvalue;

            generate_call_function(prg.curr_function_call->id);

            /* Check parameter count */
            if((retvalue = check_parameter_count_call(prg.param_count)) != SUCCESS)
                return retvalue;
            prg.param_count = 0;

            break;

        case TTYPE_ASSIGN: /* assignment - we need to check if the assigned value
                              comes from an expression or a function call */

            prg.psa_state = ASSIGN;
            next_token(true);


            /* This block ensures that a construction like bar = foo(bar) is not
             * valid before defining bar */
            retvalue = add_symbol_var(prg.curr_token.attribute.string);
            switch (retvalue) {
                /* The first occurence of the symbolt */
                case NEW_VARIABLE:
                    if (prg.in_function)
                        prg.undef_symbol = symtable_search(&prg.table_local,
                                prg.curr_token.attribute.string);
                    else
                        prg.undef_symbol = symtable_search(&prg.table_global,
                                prg.curr_token.attribute.string);
                    
                    prg.undef_symbol->attributes.var_att.defined = false;

                    /* This is a new variable - declare it */
                    declaration_variable(&prg.curr_token); 
                    retvalue = SUCCESS;
                    break;

                case SUCCESS:
                    /* Load the referenced variable from the correct
                     * symtable */
                    if (prg.in_function) 
                        prg.undef_symbol = symtable_search(&prg.table_local,
                                prg.curr_token.attribute.string);
                    else
                        prg.undef_symbol = symtable_search(&prg.table_global,
                                prg.curr_token.attribute.string);
                    break;

                default:
                    return retvalue;
            }

            stash_clear();
            next_token(false);
            unget_token();
            if (prg.curr_token.type == TTYPE_ID) {

                next_token(false);
                if (prg.curr_token.type == TTYPE_LTBRAC) {
                    unget_token();

                    /* the statement is a function call, let's check the semantics */
                    next_token(true);
                    if ((retvalue = check_function_call(prg.curr_token.attribute.string))
                            != SUCCESS)
                        return retvalue;

                    generate_create_frame();

                    next_token(true);
                    if((retvalue = r_function_call()) != SUCCESS) return retvalue;

                    generate_call_function(prg.curr_function_call->id);

                    /* Check parameter count */
                    if ((retvalue = check_parameter_count_call(prg.param_count)))
                        return retvalue;
                    prg.param_count = 0;

                    if (prg.undef_symbol) {
                        generate_assign_retvalue(prg.undef_symbol->id);
                    }

                } else {
                    unget_token(); /* Not a function call */
                    retvalue = psa(prg.undef_symbol ? prg.undef_symbol->id : NULL);
                }

            } else { /* A literal -> expression -> let psa do the work */
                retvalue = psa(prg.undef_symbol ? prg.undef_symbol->id : NULL);
            }

            if (prg.undef_symbol) { /* Reset the undefined symbol attributes */
                prg.undef_symbol->attributes.var_att.defined = true;
                prg.undef_symbol = NULL;
            }

            prg.psa_state = DEFAULT;
            break;

        default: /* expression */
            unget_token();
            retvalue = psa(prg.undef_symbol ? prg.undef_symbol->id : NULL);
            break;
    }

    return retvalue;
}

int r_function_call() {
    int retvalue = ERROR_SYNTAX;
    if (prg.curr_token.type != TTYPE_LTBRAC) return ERROR_SYNTAX; /* ( */

    /* Onto the param list */
    next_token(false);
    if ((retvalue = r_param_list()) != SUCCESS) return retvalue;
    if (prg.curr_token.type != TTYPE_RTBRAC) return ERROR_SYNTAX;

    return retvalue;
}

int next_token(bool load_from_stash) {
    int retvalue;

    /* Free token string attribute if loading with get_token and stash is empty */
    if (stash_empty()
            || (prg.token_stash[0].type == TTYPE_EOF && prg.token_stash[1].type != TTYPE_EOF)) {
        token_free(&prg.curr_token);
    }

    if (load_from_stash) {

        if (prg.token_stash[0].type != TTYPE_EOF) {
            prg.curr_token = prg.token_stash[0];
            prg.token_stash[0].type = TTYPE_EOF;
            return SUCCESS;

        } else if (prg.token_stash[1].type != TTYPE_EOF) {
            prg.curr_token = prg.token_stash[1];
            prg.token_stash[1].type = TTYPE_EOF;
            return SUCCESS;
        }
    } 

    /* At this point, we dont't care about leaks,
     * just free whatever you can and exit */
    retvalue = get_token(&prg.curr_token); /* Get next token */
    if (retvalue != SUCCESS) {
        free_resources();
        fprintf(stderr, "Line %d - Lex error %d\n", scn.line_counter, retvalue);
        exit(retvalue);
    }

    return retvalue;
}

void unget_token() {
    /* always stash the token on the first free index */
    if (prg.token_stash[0].type == TTYPE_EOF) {
        prg.token_stash[0] = prg.curr_token;
    } else {
        prg.token_stash[1] = prg.curr_token;
    }
}

void stash_clear() {
    /* free the tokens, reinit the stash */
    for (int i = 0; i < 2; i++) {
        token_free(&prg.token_stash[i]);
        prg.token_stash[i].type = TTYPE_EOF;
    }
}

void token_free(token_t *token) {
    switch (token->type) {
        /* free the token attribute if it was of type char * */
        case TTYPE_STR: case TTYPE_ID: case TTYPE_INT:
        case TTYPE_DOUBLE:
            free(token->attribute.string);
            token->attribute.string = NULL;
            break;

        default:
            break;
    }
}

bool stash_empty() {
    return prg.token_stash[0].type == TTYPE_EOF && prg.token_stash[1].type == TTYPE_EOF;
}

int init_resources() {

    /* Initialize the indent stack for scanner */
    scn.dent_stack = stack_general_init();
    if (!scn.dent_stack) return ERROR_INTERNAL;

    /* Initialize the token stash */
    prg.token_stash[0].type = TTYPE_EOF;
    prg.token_stash[0].attribute.string = NULL;
    prg.token_stash[1].type = TTYPE_EOF;
    prg.token_stash[1].attribute.string = NULL;

    /* Initialize symbol tables */
    symtable_init(&prg.table_global);
    symtable_init(&prg.table_local);

    if (add_built_in_functions()) return ERROR_INTERNAL;

    if (start_program()) return ERROR_INTERNAL;
    return SUCCESS;
}

void free_resources() {
    token_free(&prg.curr_token);
    token_free(&prg.token_stash[0]);
    token_free(&prg.token_stash[1]);

    stack_free(scn.dent_stack);
    stash_clear();

    symtable_clear_all(&prg.table_global);
    symtable_clear_all(&prg.table_local);

    free_assembly_code();
}
