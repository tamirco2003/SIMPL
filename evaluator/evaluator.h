// Responsible for evaluation functionality.
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "..\ast.h"

// Exits the program when evaluator encounters an error.
void runtimeError();

void evaluate(Statement* firstStatement);
void evaluateStatement(Statement* statement);
double evaluateExpression(Expression* expression);

#endif