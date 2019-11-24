#ifndef INFTOPOST
#define INFTOPOST

#include "scanner.h"
#include "parser.h"
#include "dynamic_string.h"
#include "errors.h"
#include "precedence_analysis.h"
#include "general_stack.h"
#include "symtable.h"
#include <string.h>
#include <stdbool.h>

#define INITLENGTH 20 //max length of postfix array of tokens

typedef struct{
    token_t *arr;
    int maxLen;
    int currLen;
}t_array;

/**
 * [Make a deep copy of token, allocate memory for token and string as attribute(if it's type string or id)
 * @param arrayToCopy [pointer to array into which token will be copied]
 * @param originalToken [Token to copy]
 * @return [0 on success, 1 on allocation error]
 */
int copyTokenToArray(t_array* arrayToCopy, const token_t* originalToken);

/**
 * [Initialize memory for t_array struct and alloc array of tokens in it]
 * @param newArr [Pointer to t_array struct which will be allocated and initialized]
 * @return [0 on success, 1 on allocation error]
 */
int tokenArrCreateInit(t_array* newArr);

/**
 * [Resize size of given array by 100%]
 * @param toresizeArr [Array to be resized]
 * @return [0 on success, 1 on reallocation error]
 */
int resizeArrayIfNeeded(t_array* toresizeArr);

/**
 * [Changes infix expression in infixArray to postfix expression]
 * @param s [Stack used for infix to postfix algorithm]
 * @param infixArray [Array with infix expression]
 * @param postfixArr [Array with final postfix expression]
 * @return [0 on success, ]
 */
int infixToPostfix(stack_general_t* s, t_array* infixArray, t_array* postfixArr);


void untilLeftPar(stack_general_t* s, t_array* postfixArr);
void doOperation(stack_general_t* s, token_t* newToken, t_array* postfixArr);
void freeArray(t_array* toDelete);
void printArray(t_array* toPrint);
int getPriority(const token_t* token);
bool isOperator(const token_t* token);
int postfixEval(t_array* postfix);
int checkSemantic(token_t *operand1, token_t *operand2, token_t *operator);
token_t* tokenGen(char* name);
int checkDefinedVarInPostfix(t_array* postfix);
#endif