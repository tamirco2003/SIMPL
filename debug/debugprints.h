// For pretty prints and debugging.
#ifndef DEBUGPRINTS_H
#define DEBUGPRINTS_H

#include "..\parser\parser.h"
#include "..\scanner\scanner.h"

void printToken(Token* token);
void printStatement(Statement* stmt);
void printDeclaration(Declaration* decl, int depth);
void printExpression(Expression* expr, int depth);
void printAssignment(AssignmentExpression* assignment, int depth);
void printBinaryExpression(BinaryExpression* binaryExpression, int depth);
void printUnaryExpression(UnaryExpression* unaryExpression, int depth);

#endif