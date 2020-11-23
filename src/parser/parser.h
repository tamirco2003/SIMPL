// Responsible for parsing functionality, Recursive Descent algorithm.
#ifndef PARSER_H
#define PARSER_H

#include "..\ast.h"
#include "..\tokenlist.h"

// Sets error flag to true and longjmps when error is encountered.
void parserError();

// Removes and frees tokens until semicolon, right brace, or end of file.
void synchronize(List* list);

// Takes in token list, token pointer, token type, and printf parameters. If
// token is not of type type, prints and errors out. Otherwise, dequeues and
// frees the token.
void expectToken(List* list, Token* token, TokenType type, char* strToFormat,
                 ...);

// Main parsing function. Takes in token list and calls parseStatement, or calls
// synchronize if in error recovery mode.
Statement* parse(List* list);

// Statement parsing function. Takes in token list, resolves statement type, and
// calls parseBlock, parsePrint, parseDeclaration, parseIfStatement,
// parseWhileStatement, parseDoWhileStatement, and parseExpressionStatement
// accordingly.
Statement* parseStatement(List* list);

// Expression statement parsing function. Takes in token list, calls
// parseExpression, and expects a semicolon.
Expression* parseExpressionStatement(List* list);

// Block statement parsing function. Takes in token list and works similarly to
// parse by creating a linked list of statements. Calls parseStatement.
Statement* parseBlock(List* list);

// Print statement parsing function. Takes in token list and calls
// parseExpression.
PrintStatement* parsePrint(List* list);

// Declaration parsing function. Takes in token list, resolves identifier, and
// calls parseExpression if finds an assignment.
Declaration* parseDeclaration(List* list);

// If statement parsing function. Takes in token list, 
IfStatement* parseIfStatement(List* list);

WhileStatement* parseWhileStatement(List* list);

WhileStatement* parseDoWhileStatement(List* list);

// Main expression parsing function. Takes in token list, calls itself if
// assignment is found, otherwise calls disjunction.
Expression* parseExpression(List* list);

Expression* disjunction(List* list);

Expression* conjunction(List* list);

Expression* inversion(List* list);

Expression* comparison(List* list);

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