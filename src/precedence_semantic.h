/**
 *	@file   precedence_semantic.h
 *	@author Marek Sarvas, xsarva00
 *	@brief Header file for precedence semantic analysis
 */

#ifndef INFTOPOST
#define INFTOPOST

#include "scanner.h"
#include "parser.h"
#include "dynamic_string.h"
#include "errors.h"
#include "precedence_analysis.h"
#include "general_stack.h"
#include "symtable.h"
#include "generate_ver2.h"
#include <string.h>
#include <stdbool.h>

#define INITLENGTH 20 //max length of postfix array of tokens

typedef struct{
    token_t *arr;
    int maxLen;
    int currLen;
}t_array;

/**
 * Make a deep copy of token, allocate memory for token and string as attribute(if it's type string or id
 * @param arrayToCopy pointer to array into which token will be copied
 * @param originalToken Token to copy
 * @return SUCCESS when token is copied successfully, ALLOC_ERROR on allocation error.
 */
int copyTokenToArray(t_array* arrayToCopy, const token_t* originalToken);

/**
 * Initialize memory for t_array struct and alloc array of tokens in it.
 * @param newArr Pointer to t_array struct which will be allocated and initialized.
 * @return SUCCESS when array is created successfully, ALLOC_ERROR on allocation error.
 */
int tokenArrCreateInit(t_array* newArr);

/**
 * Resize size of given array by 100%.
 * @param toresizeArr Array to be resized.
 * @return SUCCESS if reallocation is okay, ALLOC_ERROR on reallocation error.
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

/**
 * Pops all items from postfix stack to postfix expression until left bracket occures on top of the stack.
 * @param s Postfix stack of operators.
 * @param postfixArr Array of final postfix expression.
 */
void untilLeftPar(stack_general_t* s, t_array* postfixArr);

/**
 * Do operations based on given operator in infix to postfix algorithm.
 * @param s Postfix stack of operators.
 * @param newToken New operator from infix expression.
 * @param postfixArr Array of final postfix expression.
 */
void doOperation(stack_general_t* s, token_t* newToken, t_array* postfixArr);

/**
 * Free memory allocated for array of tokens.
 * @param toDelete Array of tokens to be deleted.
 */
void freeArray(t_array* toDelete);
void printArray(t_array* toPrint);

/**
 * Get priority of operator for the infix to postfix algorithm.
 * @param token
 * @return 0 if relational operator, 1 if + or -, 2 if '*' '/' '//' otherwise -1
 */
int getPriority(const token_t* token);

/**
 * Checks if given token is operator.
 * @param token Token which will be check if it's operator.
 * @return True if token is operator, false if not.
 */
bool isOperator(const token_t* token);

/**
 * Evaluate postfix expression and calls code generator for generating expression and/or assignment.
 * @param postfix Array of tokens representing postfix expression.
 * @param assignmentID Id of variable into which the result of postfix expression will be stored, if it's null
 * expression is not assignment.
 * @return SUCCESS on successful evaluation of postfix expression, ERROR_SEM_DEFINITION when variable in expression
 * is not defined, or semantic error based on return value of checkSemantic function.
 */
int postfixEval(t_array* postfix, const char* assignmentID);

/**
 * Semantic analysis during postfix evaluation, checks two operands and operator.
 * @param operand1 First operand in semi-result of postfix expression.
 * @param operand2 Second operand in semi-result of postfix expression.
 * @param operator
 * @return ERROR_DIV_ZERO when there is dividing by 0 in expression, ERROR_SEM_TYPE when two types of operands are
 * incompatible for given operator, ERROR_SEM_OTHER otherwise.
 */
int checkSemantic(token_t *operand1, token_t *operand2, token_t *operator);

/**
 * Create and allocate memory for new token representing variable(TTYPE_ID)
 * @param name Id of variable represented by the new token.
 * @return Pointer to newly created token, NULL on allocation error.
 */
token_t* tokenGen(char* name);

/**
 * Loop through array of postfix expression and if there is variable checks if it's been defined before.
 * @param postfix Array of tokens representing postfix expression.
 * @param assignmentID Id of variable into which result will be assigned, if it's NULL it's not assignment and nothing happens.
 * @return
 */
int checkDefinedVarInPostfix(t_array* postfix,  const char* assignmentID);

/**
 * Free all memory allocated for the stack of tokens.
 * @param stackToFree Stack of tokens.
 */
void freeTokenStack(stack_general_t* stackToFree);

/**
 * Free memory allocated for semantic analysis.
 * @param infix Array of tokens representing infix expression.
 * @param postfix Array of tokens representing postfix expression.
 * @param s Helping stack for infix to postfix algorithm.
 */
void freePsaResources(t_array* infix, t_array* postfix, stack_general_t* s);
#endif