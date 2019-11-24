#ifndef GENERATE_H
#define GENERATE_H

#include <string.h>
#include "dynamic_string.h"
#include "scanner.h"


void start_program();

void end_program();

void generate_function(const char *name_of_function);

string_t *genenarte_expression(token_t *operand1, token_t *operator, token_t *operand2, string_t *result);


#endif //GENERATE_H
