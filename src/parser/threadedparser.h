// Responsible for parsing functionality, Recursive Descent algorithm.
#ifndef PARSER_H
#define PARSER_H

#include "..\ast.h"
// #include "..\tokenlist.h"

// Sets error flag to true and longjmps when error is encountered.
void parserError();

// Removes and frees tokens until semicolon, right brace, or end of file.
void synchronize();

// Takes in token list, token pointer, token type, and printf parameters. If
// token is not of type type, prints and errors out. Otherwise, dequeues and
// frees the token.
void expectToken(Token* token, TokenTeeeepe type, char* strToFormat, ...);

// Main parsing function. Takes in token list and calls parseStatement, or calls
// synchronize if in error recovery mode.
unsigned __stdcall parse(void* pArguments);

// Statement parsing function. Takes in token list, resolves statement type, and
// calls parseBlock, parsePrint, parseDeclaration, parseIfStatement,
// parseWhileStatement, parseDoWhileStatement, and parseExpressionStatement
// accordingly.
Statement* parseStatement();

// Expression statement parsing function. Takes in token list, calls
// parseExpression, and expects a semicolon.
Expression* parseExpressionStatement();

// Block statement parsing function. Takes in token list and works similarly to
// parse by creating a linked list of statements. Calls parseStatement.
Statement* parseBlock();

// Print statement parsing function. Takes in token list and calls
// parseExpression.
PrintStatement* parsePrint();

// Declaration parsing function. Takes in token list, resolves identifier, and
// calls parseExpression if finds an assignment.
Declaration* parseDeclaration();

// If statement parsing function. Takes in token list,
IfStatement* parseIfStatement();

WhileStatement* parseWhileStatement();

WhileStatement* parseDoWhileStatement();

// Main expression parsing function. Takes in token list, calls itself if
// assignment is found, otherwise calls disjunction.
Expression* parseExpression();

Expression* disjunction();

Expression* conjunction();

Expression* inversion();

Expression* comparison();

// Top level arithmatic parsing function, handles addition and subtraction.
// Takes in token list, calls term, and checks for addition and subtraction.
Expression* arithmatic();

// Second level arithmatic parsing function, handles multiplication and
// division. Takes in token list, calls factor, and checks for multiplication
// and division.
Expression* term();

// Bottom level arithmatic parsing function, handles negation, identifiers,
// literals, and grouping. Calls arithmatic if grouping is found.
Expression* factor();

#endif