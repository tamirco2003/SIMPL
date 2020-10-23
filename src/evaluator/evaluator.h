// Responsible for evaluation functionality.
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "..\ast.h"

// Exits the program when evaluator encounters an error.
void runtimeError();

// Takes in a pointer to a statement, evaluates it and each following statement
// by calling evaluateStatement.
void evaluate(Statement* firstStatement);

// Takes in a statement pointer and evaluates it. Calls evaluateExpression.
// evaluateDeclaration, and evaluatePrint accordingly.
void evaluateStatement(Statement* statement);

// Takes in a declaration pointer and evaluates it. Calls evaluateExpression if
// needed, also accesses dictionary.
void evaluateDeclaration(Declaration* declaration);

// Takes in a print statement pointer and evaluates it. Calls
// evaluateExpression.
void evaluatePrint(PrintStatement* printStatement);

// Takes in a expression pointer, evaluates it, and returns a literal expression
// pointer. Calls evaluateBinaryExpression, evaluateUnaryExpression,
// evaluateAssignment, evaluateVariable, and itself.
LiteralExpression* evaluateExpression(Expression* expression);

// Takes in a binary expression pointer, evaluates it, and returns a literal
// expression pointer. Calls evaluateExpression.
LiteralExpression* evaluateBinaryExpression(BinaryExpression* binaryExpression);

// Takes in a unary expression pointer, evaluates it, and returns a literal
// expression pointer. Calls evaluateExpression.
LiteralExpression* evaluateUnaryExpression(UnaryExpression* unaryExpression);

// Takes in an assignment expression pointer, evaluates it, and returns a
// literal expression pointer. Calls evaluateExpression, also accesses
// dictionary.
LiteralExpression* evaluateAssignment(AssignmentExpression* assignment);

// Takes in a variable expression pointer, evaluates it, and returns a literal
// expression pointer.
LiteralExpression* evaluateVariable(VariableExpression* variable);

#endif