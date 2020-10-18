// Responsible for evaluation functionality.
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "..\ast.h"

// Exits the program when evaluator encounters an error.
void runtimeError();

// Takes in a pointer to a statement, evaluates it and each following statement
// by calling evaluateStatement.
void evaluate(Statement* firstStatement);

// Takes in a statement pointer and evaluates, calls evaluateExpression.
void evaluateStatement(Statement* statement);

// Takes in a expression pointer and evaluates it, calls itself recursively.
double evaluateExpression(Expression* expression);

#endif