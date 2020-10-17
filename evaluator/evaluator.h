#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "..\ast.h"

void runtimeError();

void evaluate(Statement* firstStatement);
void evaluateStatement(Statement* statement);
double evaluateExpression(Expression* expression);

#endif