// Responsible for pretty prints for debugging.
#ifndef DEBUGPRINTS_H
#define DEBUGPRINTS_H

#include "..\parser\parser.h"
#include "..\scanner\scanner.h"

// Takes in token pointer and pretty prints the token.
void printToken(Token* token);

// Takes in statement pointer and pretty prints the statement and it's contents.
// Calls printDeclaration and printExpression accordingly.
void printStatement(Statement* stmt, int depth);

void printBlockStatement(Statement* stmt, int depth);

// Takes in declaration pointer and depth, pretty prints the declaration and its
// contents indented according to depth. Calls printExpression.
void printDeclaration(Declaration* decl, int depth);

void printIfStatement(IfStatement* stmt, int depth);

void printWhileStatement(WhileStatement* stmt, int depth);

void printDoWhileStatement(WhileStatement* stmt, int depth);

// Takes in expression pointer and depth, pretty prints the expression and its
// contents indented according to depth. Calls printLiteral, printAssignment,
// printBinaryExpression, printUnaryExpression, and itself accordingly.
void printExpression(Expression* expr, int depth);

// Takes in literal expression pointer and depth, pretty prints the literal and
// its contents indented according to depth.
void printLiteral(LiteralExpression* literal, int depth);

// Takes in assignment pointer and depth, pretty prints the assignment and its
// contents indented according to depth. Calls printExpression accordingly.
void printAssignment(AssignmentExpression* assignment, int depth);

// Takes in binary expression pointer and depth, pretty prints the binary
// expression and its contents indented according to depth. Calls
// printExpression accordingly.
void printBinaryExpression(BinaryExpression* binaryExpression, int depth);

// Takes in unary expression pointer and depth, pretty prints the unary
// expression and its contents indented according to depth. Calls
// printExpression accordingly.
void printUnaryExpression(UnaryExpression* unaryExpression, int depth);

#endif