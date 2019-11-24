#ifndef GENERATE_H
#define GENERATE_H

#include <string.h>
#include "dynamic_string.h"
#include "scanner.h"


void start_program();

void end_program();

void generate_function(const char *name_of_function);

void genenarte_expression(token_t *operand1, char *operator, token_t *operand2, char *result);


#endif //GENERATE_H
