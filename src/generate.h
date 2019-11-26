#ifndef GENERATE_H
#define GENERATE_H

#include <string.h>
#include "dynamic_string.h"
#include "scanner.h"


void start_program();

void token_generator(token_t *operand);

void adding_operands(string_t *frame, string_t *result, token_t *operand1, token_t *operand2);

void end_program();
#endif //GENERATE_H
