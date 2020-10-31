// Responsible for parsing functionality, Recursive Descent algorithm.
#ifndef PARSER_H
#define PARSER_H

#include "..\ast.h"
#include "..\tokenlist.h"

// Longjmps (TIME TRAVEL!) when error is encountered.
void parserError();

// Main parsing function. Takes in token list and calls parseStatement.
Statement* parse(List* list);

// Statement parsing function. Takes in token list, resolves statement type, and
// calls parsePrint, parseDeclaration, and parseExpression accordingly.
Statement* parseStatement(List* list);

// Print statement parsing function. Takes in token list and calls
// parseExpression.
PrintStatement* parsePrint(List* list);

// Declaration parsing function. Takes in token list, resolves identifier, and
// calls parseExpression if finds an assignment.
Declaration* parseDeclaration(List* list);

// Main expression parsing function. Takes in token list, calls itself if
// assignment is found, otherwise calls arithmatic.
Expression* parseExpression(List* list);

// Top level arithmatic parsing function, handles addition and subtraction.
// Takes in token list, calls term, and checks for addition and subtraction.
Expression* arithmatic(List* list);

// Second level arithmatic parsing function, handles multiplication and
// division. Takes in token list, calls factor, and checks for multiplication
// and division.
Expression* term(List* list);

// Bottom level arithmatic parsing function, handles negation, identifiers,
// literals, and grouping. Calls arithmatic if grouping is found.
Expression* factor(List* list);

#endif