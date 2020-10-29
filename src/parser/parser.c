#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool error;
static bool panic;

// void parserError() { exit(1); }
void parserError() {
  error = true;
  panic = true;
}

void synchronize(List* list) {
  while (dequeue(list)->type != T_SEMICOLON && peek(list)->type != T_EOF)
    ;
  panic = false;
}

Statement* parse(List* list) {
  error = false;
  panic = false;

  Statement* head = NULL;
  Statement* tail = NULL;

  Token* token = peek(list);
  while (token->type != T_EOF) {
    Statement* statement = parseStatement(list);

    if (panic) {
      synchronize(list);
    } else {
      if (tail == NULL) {
        head = statement;
      } else {
        tail->next = statement;
      }

      tail = statement;
    }

    token = peek(list);
  }

  free(list);

  if (error) {
    return NULL;
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
      freeToken(token);

      result->type = S_PRINT;
      result->content.printStatement = parsePrint(list);
      break;
    case T_LET:
      dequeue(list);
      freeToken(token);

      result->type = S_DECLARATION;
      result->content.declaration = parseDeclaration(list);
      break;
    default:
      result->type = S_EXPRESSION;
      result->content.expression = parseExpression(list);
      break;
  }

  if (panic) {
    return NULL;
  }

  Token* semicolon = peek(list);
  if (semicolon->type != T_SEMICOLON) {
    printf("ERR: Expected ';' at/before '%s' on line %d.\n", semicolon->lexeme,
           semicolon->line);
    parserError();
    return NULL;
  }
  dequeue(list);
  freeToken(semicolon);

  return result;
}

PrintStatement* parsePrint(List* list) {
  PrintStatement* result = (PrintStatement*)malloc(sizeof(PrintStatement));
  result->expression = parseExpression(list);

  // Works without this if statement, does returning null matter if panic is on?
  // Sometimes yes, otherwise function would continue.
  if (panic) {
    return NULL;
  }

  return result;
}

Declaration* parseDeclaration(List* list) {
  Declaration* result = (Declaration*)malloc(sizeof(Declaration));
  result->expression = NULL;

  Token* identifier = peek(list);
  if (identifier->type != T_IDENTIFIER) {
    printf("ERR: Expected identifier at '%s' on line %d.\n", identifier->lexeme,
           identifier->line);
    parserError();
    return NULL;
  }
  dequeue(list);
  result->identifier = identifier;

  Token* next = peek(list);
  if (next->type == T_EQUALS) {
    dequeue(list);
    freeToken(next);

    result->expression = parseExpression(list);

    if (panic) {
      return NULL;
    }
  }

  return result;
}

Expression* parseExpression(List* list) {
  Expression* result = (Expression*)malloc(sizeof(Expression));

  Token* first = dequeue(list);
  Token* next = peek(list);
  if (next->type == T_EQUALS) {
    dequeue(list);
    freeToken(next);

    result->type = E_ASSIGNMENT;

    AssignmentExpression* assign =
        (AssignmentExpression*)malloc(sizeof(AssignmentExpression));

    assign->identifier = first;
    assign->expression = parseExpression(list);
    result->content.assignmentExpression = assign;

    if (panic) {
      return NULL;
    }

    return result;
  }

  push(list, first);
  return arithmatic(list);
}

Expression* arithmatic(List* list) {
  Expression* termRes = term(list);

  if (panic) {
    return NULL;
  }

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

    if (panic) {
      return NULL;
    }

    res->content.binaryExpression = binEx;

    termRes = res;

    freeToken(op);
    op = peek(list);
  }

  return termRes;
}

Expression* term(List* list) {
  Expression* factorRes = factor(list);

  if (panic) {
    return NULL;
  }

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

    if (panic) {
      return NULL;
    }

    res->content.binaryExpression = binEx;

    factorRes = res;

    freeToken(op);
    op = peek(list);
  }

  return factorRes;
}

Expression* factor(List* list) {
  Expression* head = NULL;
  Expression* tail = NULL;

  Token* token = peek(list);

  while (token->type == T_MINUS) {
    freeToken(token);

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

    dequeue(list);
    token = peek(list);
  }

  Expression* nakedRes = (Expression*)malloc(sizeof(Expression));

  switch (token->type) {
    case T_IDENTIFIER:
      dequeue(list);
      nakedRes->type = E_VARIABLE;
      VariableExpression* variable =
          (VariableExpression*)malloc(sizeof(VariableExpression));
      variable->identifier = token;
      nakedRes->content.variableExpression = variable;
      break;
    case T_NUMBER:
      dequeue(list);
      nakedRes->type = E_LITERAL;
      LiteralExpression* literal =
          (LiteralExpression*)malloc(sizeof(LiteralExpression));
      literal->value.number = atof(token->lexeme);
      nakedRes->content.literalExpression = literal;
      freeToken(token);
      break;
    case T_LPAR:
      dequeue(list);
      freeToken(token);
      nakedRes->type = E_GROUPING;
      GroupingExpression* grouping =
          (GroupingExpression*)malloc(sizeof(GroupingExpression));
      grouping->expression = arithmatic(list);
      nakedRes->content.groupingExpression = grouping;

      Token* rightPar = peek(list);
      if (rightPar->type != T_RPAR) {
        printf("ERR: Expected ')' at '%s' on line %d.\n", rightPar->lexeme,
               rightPar->line);
        parserError();
        return NULL;
      }
      dequeue(list);
      freeToken(rightPar);
      break;
    default:
      printf("ERR: Unexpected token at '%s' on line %d.\n", token->lexeme,
             token->line);
      parserError();
      return NULL;
      break;
  }

  if (tail == NULL) {
    return nakedRes;
  }

  tail->content.unaryExpression->right = nakedRes;
  return head;
}