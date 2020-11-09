#include "debugprints.h"

#include <stdio.h>

// String representations of token types.
static const char* const TokenTypeString[] = {
    "PLUS",       "MINUS",     "STAR",   "SLASH",   "LPAR",
    "RPAR",       "SEMICOLON", "EQUALS", "PERCENT", "NUMBER",
    "IDENTIFIER", "STRING",    "PRINT",  "LET",     "EOF"};

void printToken(Token* token) {
  if (token->type == T_NUMBER || token->type == T_IDENTIFIER) {
    printf("%s ", token->lexeme);
  } else if (token->type == T_STRING) {
    printf("\"%s\" ", token->lexeme);
  } else {
    printf("%s ", TokenTypeString[token->type]);
    if (token->type == T_SEMICOLON) {
      printf("\n");
    }
  }
}

void printStatement(Statement* stmt) {
  printf("Statement : ");
  switch (stmt->type) {
    case S_EXPRESSION:
      printf("Expression {\n");
      printExpression(stmt->content.expression, 1);
      break;
    case S_DECLARATION:
      printf("Declaration {\n");
      printDeclaration(stmt->content.declaration, 1);
      break;
    case S_PRINT:
      printf("Print {\n");
      printExpression(stmt->content.printStatement->expression, 1);
      break;
  }

  printf("}\n\n");
}

void printDeclaration(Declaration* decl, int depth) {
  printf("%*cDeclaration {\n", depth * 4, ' ');
  printf("%*cIdentifier: %s\n", (depth + 1) * 4, ' ', decl->identifier->lexeme);

  if (decl->expression != NULL) {
    printExpression(decl->expression, depth + 1);
  }

  printf("%*c}\n", depth * 4, ' ');
}

void printExpression(Expression* expr, int depth) {
  printf("%*cExpression {\n", depth * 4, ' ');

  switch (expr->type) {
    case E_BINARY:
      printBinaryExpression(expr->content.binaryExpression, depth + 1);
      break;
    case E_UNARY:
      printUnaryExpression(expr->content.unaryExpression, depth + 1);
      break;
    case E_GROUPING:
      printf("%*cGrouping {\n", (depth + 1) * 4, ' ');
      printExpression(expr->content.groupingExpression->expression, depth + 2);
      printf("%*c}\n", (depth + 1) * 4, ' ');
      break;
    case E_LITERAL:
      // printf("%*cLiteral: %f\n", (depth + 1) * 4, ' ',
      //        expr->content.literalExpression->value.number);
      printLiteral(expr->content.literalExpression, depth + 1);
      break;
    case E_ASSIGNMENT:
      printAssignment(expr->content.assignmentExpression, depth + 1);
      break;
    case E_VARIABLE:
      printf("%*cLexeme: %s\n", (depth + 1) * 4, ' ',
             expr->content.variableExpression->identifier->lexeme);
      break;
  }

  printf("%*c}\n", depth * 4, ' ');
}

void printLiteral(LiteralExpression* literal, int depth) {
  printf("%*cLiteral: ", depth * 4, ' ');
  switch (literal->type) {
    case L_UNDEFINED:
      printf("undefined\n");
      break;
    case L_NUMBER:
      printf("%f\n", literal->value.number);
      break;
    case L_STRING:
      printf("\"%s\"\n", literal->value.string);
      break;
  }
}

void printAssignment(AssignmentExpression* assignment, int depth) {
  printf("%*cAssignment {\n", depth * 4, ' ');
  printf("%*cIdentifier: %s\n", (depth + 1) * 4, ' ',
         assignment->identifier->lexeme);
  printExpression(assignment->expression, depth + 1);
  printf("%*c}\n", depth * 4, ' ');
}

void printBinaryExpression(BinaryExpression* binaryExpression, int depth) {
  printf("%*cBinary : ", depth * 4, ' ');

  switch (binaryExpression->type) {
    case B_ADD:
      printf("Add {\n");
      break;
    case B_SUB:
      printf("Sub {\n");
      break;
    case B_MUL:
      printf("Mul {\n");
      break;
    case B_DIV:
      printf("Div {\n");
      break;
    case B_MOD:
      printf("Mod {\n");
      break;
  }

  printf("%*cLeft {\n", (depth + 1) * 4, ' ');
  printExpression(binaryExpression->left, depth + 2);
  printf("%*c}\n", (depth + 1) * 4, ' ');

  printf("%*cRight {\n", (depth + 1) * 4, ' ');
  printExpression(binaryExpression->right, depth + 2);
  printf("%*c}\n", (depth + 1) * 4, ' ');

  printf("%*c}\n", depth * 4, ' ');
}

void printUnaryExpression(UnaryExpression* unaryExpression, int depth) {
  printf("%*cUnary : ", depth * 4, ' ');

  switch (unaryExpression->type) {
    case U_MINUS:
      printf("Minus {\n");
      break;
  }

  printExpression(unaryExpression->right, depth + 1);
}