// Recursive descent parser.
#ifndef PARSER_H
#define PARSER_H

#include "..\ast.h"
#include "..\tokenlist.h"

int parserError();

AstTop* createAst();

int parse(AstTop* top, List* list);

Statement* parseStatement(List* list);

PrintStatement* parsePrint(List* list);

Declaration* parseDeclaration(List* list);

Expression* parseExpression(List* list);

Expression* arithmatic(List* list);

Expression* term(List* list);

Expression* factor(List* list);

#endif