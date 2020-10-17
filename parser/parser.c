#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

int parserError() { exit(1); }

Statement* parse(List* list) {
  Statement* head = NULL;
  Statement* tail = NULL;

  Token* token = peek(list);
  while (token->type != T_EOF) {
    Statement* statement = parseStatement(list);

    if (tail == NULL) {
      head = statement;
    } else {
      tail->next = statement;
    }

    tail = statement;

    token = peek(list);
  }

  return head;
}

Statement* parseStatement(List* list) {
  Statement* result = (Statement*)malloc(sizeof(Statement));
  result->next = NULL;

  Token* token = peek(list);

  switch (token->type) {
    case T_PRINT:
      dequeue(list);
      result->type = S_PRINT;
      result->content.printStatement = parsePrint(list);
      break;
    case T_LET:
      dequeue(list);
      result->type = S_DECLARATION;
      result->content.declaration = parseDeclaration(list);
      break;
    default:
      result->type = S_EXPRESSION;
      result->content.expression = parseExpression(list);
      break;
  }

  Token* semicolon = dequeue(list);
  if (semicolon->type != T_SEMICOLON) {
    printf("\n\n%d\n\n", semicolon->type);
    printf("\nERR: Expected ';' at/before '%s' on line %d.", semicolon->lexeme,
           semicolon->line);
    parserError();
  }

  return result;
}

PrintStatement* parsePrint(List* list) {
  PrintStatement* result = (PrintStatement*)malloc(sizeof(PrintStatement));
  result->expression = parseExpression(list);

  return result;
}

Declaration* parseDeclaration(List* list) {
  Declaration* result = (Declaration*)malloc(sizeof(Declaration));
  result->expression = NULL;

  Token* identifier = dequeue(list);
  if (identifier->type != T_IDENTIFIER) {
    printf("\nERR: Expected identifier at '%s' on line %d.\n",
           identifier->lexeme, identifier->line);
    parserError();
  }
  result->identifier = identifier;

  Token* next = peek(list);
  if (next->type == T_EQUALS) {
    dequeue(list);
    result->expression = parseExpression(list);
  } else if (next->type != T_SEMICOLON) {
    printf("\nERR: Unexpected token at '%s' on line %d.\n", identifier->lexeme,
           identifier->line);
    parserError();
  }

  return result;
}

Expression* parseExpression(List* list) {
  Expression* result = (Expression*)malloc(sizeof(Expression));

  Token* first = dequeue(list);
  Token* next = peek(list);
  if (next->type == T_EQUALS) {
    // SPLIT INTO "ASSIGNMENT" FUNCTION?
    dequeue(list);
    result->type = E_ASSIGNMENT;

    AssignmentExpression* assign =
        (AssignmentExpression*)malloc(sizeof(AssignmentExpression));

    assign->identifier = first;
    assign->expression = parseExpression(list);
    result->content.assignmentExpression = assign;

    return result;
  }

  push(list, first);

  return arithmatic(list);
}

Expression* arithmatic(List* list) {
  Expression* termRes = term(list);
  Token* op = peek(list);

  while (op->type == T_PLUS || op->type == T_MINUS) {
    dequeue(list);

    Expression* res = (Expression*)malloc(sizeof(Expression));
    res->type = E_BINARY;
    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->left = termRes;

    switch (op->type) {
      case T_PLUS:
        binEx->type = B_ADD;
        break;
      case T_MINUS:
        binEx->type = B_SUB;
        break;
    }

    binEx->right = term(list);

    res->content.binaryExpression = binEx;

    termRes = res;
    op = peek(list);
  }

  return termRes;
}

Expression* term(List* list) {
  Expression* factorRes = factor(list);
  Token* op = peek(list);

  while (op->type == T_STAR || op->type == T_SLASH || op->type == T_PERCENT) {
    dequeue(list);

    Expression* res = (Expression*)malloc(sizeof(Expression));
    res->type = E_BINARY;
    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->left = factorRes;

    switch (op->type) {
      case T_STAR:
        binEx->type = B_MUL;
        break;
      case T_SLASH:
        binEx->type = B_DIV;
        break;
      case T_PERCENT:
        binEx->type = B_MOD;
        break;
    }

    binEx->right = factor(list);

    res->content.binaryExpression = binEx;

    factorRes = res;
    op = peek(list);
  }

  return factorRes;
}

Expression* factor(List* list) {
  Expression* head = NULL;
  Expression* tail = NULL;

  Token* token = dequeue(list);

  while (token->type == T_MINUS) {
    Expression* unaryExp = (Expression*)malloc(sizeof(Expression));
    unaryExp->type = E_UNARY;
    UnaryExpression* unary = (UnaryExpression*)malloc(sizeof(UnaryExpression));
    unary->type = U_MINUS;
    unaryExp->content.unaryExpression = unary;

    if (head == NULL) {
      head = unaryExp;
      tail = unaryExp;
    } else {
      tail->content.unaryExpression->right = unaryExp;
      tail = unaryExp;
    }

    token = dequeue(list);
  }

  // while (token->type == T_MINUS) {
  //   Expression* unaryExp = (Expression*)malloc(sizeof(Expression));
  //   // result = (Expression*)malloc(sizeof(Expression));
  //   unaryExp->type = E_UNARY;
  //   UnaryExpression* unary =
  //   (UnaryExpression*)malloc(sizeof(UnaryExpression)); unary->type = U_MINUS;
  //   unaryExp->content.unaryExpression = unary;

  //   if (result == NULL) {
  //     result = unaryExp;
  //   } else {
  //     result->content.unaryExpression->right = unaryExp;
  //     result = unaryExp;
  //     // unary->right = result;
  //     // result = unaryExp;
  //   }

  //   token = dequeue(list);
  // }

  Expression* nakedRes = (Expression*)malloc(sizeof(Expression));

  switch (token->type) {
    case T_IDENTIFIER:
      nakedRes->type = E_VARIABLE;
      VariableExpression* variable =
          (VariableExpression*)malloc(sizeof(VariableExpression));
      variable->value = token;
      nakedRes->content.variableExpression = variable;
      break;
    case T_NUMBER:
      nakedRes->type = E_LITERAL;
      LiteralExpression* literal =
          (LiteralExpression*)malloc(sizeof(LiteralExpression));
      literal->value.number = atof(token->lexeme);
      nakedRes->content.literalExpression = literal;
      break;
    case T_LPAR:
      // SPLIT INTO GROUPING FUNCTION?
      nakedRes->type = E_GROUPING;
      GroupingExpression* grouping =
          (GroupingExpression*)malloc(sizeof(GroupingExpression));
      grouping->expression = arithmatic(list);
      nakedRes->content.groupingExpression = grouping;

      Token* rightPar = dequeue(list);
      if (rightPar->type != T_RPAR) {
        printf("\nERR: Expected '(' at '%s' on line %d.\n", rightPar->lexeme,
               rightPar->line);
        parserError();
      }
      break;
    default:
      printf("\nERR: Unexpected token at '%s' on line %d.\n", token->lexeme,
             token->line);
      parserError();
      break;
  }

  if (tail == NULL) {
    return nakedRes;
  }

  tail->content.unaryExpression->right = nakedRes;
  return head;
}

// CHECK NULL FOR EVERYTHING SO I CAN DO ERRORS?
// Maybe create an error function that prints and exits.
// But I don't wanna exit ech.