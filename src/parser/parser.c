#include "parser.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool error;
static jmp_buf error_buf;

void parserError() {
  error = true;
  longjmp(error_buf, 1);
}

void synchronize(List* list) {
  Token* next = peek(list);
  while (next->type != T_SEMICOLON && next->type != T_RBRACE &&
         next->type != T_EOF) {
    next = dequeue(list);
    freeToken(next);
    next = peek(list);
  }
}

void expectToken(List* list, Token* token, TokenType type, char* strToFormat,
                 ...) {
  va_list args;
  va_start(args, strToFormat);

  if (token->type == type) {
    dequeue(list);
    freeToken(token);
    va_end(args);
  } else {
    vprintf(strToFormat, args);
    va_end(args);
    parserError();
  }
}

Statement* parse(List* list) {
  error = false;

  Statement* head = NULL;
  Statement* tail = NULL;

  Token* token = peek(list);
  while (token->type != T_EOF) {
    if (!setjmp(error_buf)) {
      Statement* statement = parseStatement(list);

      if (tail == NULL) {
        head = statement;
      } else {
        tail->next = statement;
      }

      tail = statement;
    } else {
      synchronize(list);
      
      token = dequeue(list);
      if (token->type != T_EOF) {
        freeToken(token);
      } else {
        return NULL;
      }
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
    case T_LBRACE:
      dequeue(list);
      freeToken(token);

      result->type = S_BLOCK;
      result->content.blockBody = parseBlock(list);
      break;
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
    case T_IF:
      dequeue(list);
      freeToken(token);

      result->type = S_IF;
      result->content.ifStatement = parseIfStatement(list);
      break;
    case T_WHILE:
      dequeue(list);
      freeToken(token);

      result->type = S_WHILE;
      result->content.whileStatement = parseWhileStatement(list);
      break;
    case T_DO:
      dequeue(list);
      freeToken(token);

      result->type = S_DOWHILE;
      result->content.whileStatement = parseDoWhileStatement(list);
      break;
    default:
      result->type = S_EXPRESSION;
      result->content.expression = parseExpressionStatement(list);
      break;
  }

  return result;
}

Expression* parseExpressionStatement(List* list) {
  Expression* result = parseExpression(list);

  Token* semicolon = peek(list);
  expectToken(list, semicolon, T_SEMICOLON,
              "ERR (exprstmt): Expected ';' at/before '%s' on line %d.\n",
              semicolon->lexeme, semicolon->line);

  return result;
}

Statement* parseBlock(List* list) {
  Statement* head = NULL;
  Statement* tail = NULL;

  Token* next = peek(list);
  while (next->type != T_RBRACE) {
    if (next->type == T_EOF) {
      printf("ERR: Unclosed block, expected '}' before end of file.\n");
      parserError();
    }

    Statement* stmt = parseStatement(list);

    if (tail == NULL) {
      head = stmt;
    } else {
      tail->next = stmt;
    }

    tail = stmt;

    next = peek(list);
  }
  dequeue(list);
  freeToken(next);

  return head;
}

PrintStatement* parsePrint(List* list) {
  PrintStatement* result = (PrintStatement*)malloc(sizeof(PrintStatement));
  result->expression = parseExpression(list);

  Token* semicolon = peek(list);
  expectToken(list, semicolon, T_SEMICOLON,
              "ERR (print): Expected ';' at/before '%s' on line %d.\n",
              semicolon->lexeme, semicolon->line);

  return result;
}

Declaration* parseDeclaration(List* list) {
  Declaration* result = (Declaration*)malloc(sizeof(Declaration));
  result->expression = NULL;

  // will replace when stack lexeme problem is solved.
  Token* identifier = peek(list);
  if (identifier->type != T_IDENTIFIER) {
    printf("ERR: Expected identifier at '%s' on line %d.\n", identifier->lexeme,
           identifier->line);
    parserError();
  }
  dequeue(list);
  result->identifier = identifier;

  Token* next = peek(list);
  if (next->type == T_EQUALS) {
    dequeue(list);
    freeToken(next);

    result->expression = parseExpression(list);
  }

  Token* semicolon = peek(list);
  expectToken(list, semicolon, T_SEMICOLON,
              "ERR (decl): Expected ';' at/before '%s' on line %d.\n",
              semicolon->lexeme, semicolon->line);

  return result;
}

IfStatement* parseIfStatement(List* list) {
  IfStatement* result = (IfStatement*)malloc(sizeof(IfStatement));

  Token* expected;

  expected = peek(list);
  expectToken(list, expected, T_LPAR, "ERR: Expected '(' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->condition = parseExpression(list);

  expected = peek(list);
  expectToken(list, expected, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->body = parseStatement(list);

  expected = peek(list);
  if (expected->type == T_ELSE) {
    dequeue(list);
    result->elseBody = parseStatement(list);
  } else {
    result->elseBody = NULL;
  }

  return result;
}

WhileStatement* parseWhileStatement(List* list) {
  WhileStatement* result = (WhileStatement*)malloc(sizeof(WhileStatement));

  Token* expected;

  expected = peek(list);
  expectToken(list, expected, T_LPAR, "ERR: Expected '(' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->condition = parseExpression(list);

  expected = peek(list);
  expectToken(list, expected, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->body = parseStatement(list);

  return result;
}

WhileStatement* parseDoWhileStatement(List* list) {
  WhileStatement* result = (WhileStatement*)malloc(sizeof(WhileStatement));

  result->body = parseStatement(list);

  Token* expected;

  expected = peek(list);
  expectToken(list, expected, T_WHILE,
              "ERR: Expected 'while' at '%s' on line %d.\n", expected->lexeme,
              expected->line);

  expected = peek(list);
  expectToken(list, expected, T_LPAR, "ERR: Expected '(' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->condition = parseExpression(list);

  expected = peek(list);
  expectToken(list, expected, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  expected = peek(list);
  expectToken(list, expected, T_SEMICOLON,
              "ERR (dowhile): Expected ';' at/before '%s' on line %d.\n",
              expected->lexeme, expected->line);

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

    return result;
  }

  push(list, first);
  return disjunction(list);
}

Expression* disjunction(List* list) {
  Expression* conjunctionRes = conjunction(list);

  Token* op = peek(list);

  while (op->type == T_OR) {
    dequeue(list);

    Expression* res = (Expression*)malloc(sizeof(Expression));
    res->type = E_BINARY;

    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->type = B_OR;
    binEx->left = conjunctionRes;
    binEx->right = conjunction(list);

    res->content.binaryExpression = binEx;

    conjunctionRes = res;
    freeToken(op);  // PROBLEM! Freeing strings initialized without malloc...
    op = peek(list);
  }

  return conjunctionRes;
}

Expression* conjunction(List* list) {
  Expression* inversionRes = inversion(list);

  Token* op = peek(list);

  while (op->type == T_AND) {
    dequeue(list);

    Expression* res = (Expression*)malloc(sizeof(Expression));
    res->type = E_BINARY;

    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->type = B_AND;
    binEx->left = inversionRes;
    binEx->right = inversion(list);

    res->content.binaryExpression = binEx;

    inversionRes = res;
    freeToken(op);  // ERR! Freeing strings initialized without malloc...
    op = peek(list);
  }

  return inversionRes;
}

Expression* inversion(List* list) {
  Expression* head = NULL;
  Expression* tail = NULL;

  Token* token = peek(list);

  while (token->type == T_NOT) {
    freeToken(token);

    Expression* unaryExp = (Expression*)malloc(sizeof(Expression));
    unaryExp->type = E_UNARY;
    UnaryExpression* unary = (UnaryExpression*)malloc(sizeof(UnaryExpression));
    unary->type = U_NOT;
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

  Expression* nakedRes = comparison(list);

  if (tail == NULL) {
    return nakedRes;
  }

  tail->content.unaryExpression->right = nakedRes;
  return head;
}

Expression* comparison(List* list) {
  Expression* arithmaticRes = arithmatic(list);

  Token* token = peek(list);

  if (token->type == T_COMP_EQUALS || token->type == T_COMP_LT ||
      token->type == T_COMP_LTE || token->type == T_COMP_GT ||
      token->type == T_COMP_GTE || token->type == T_COMP_NE) {
    dequeue(list);

    Expression* result = (Expression*)malloc(sizeof(Expression));
    result->type = E_BINARY;
    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->left = arithmaticRes;
    binEx->right = arithmatic(list);

    switch (token->type) {
      case T_COMP_EQUALS:
        binEx->type = B_EQUAL;
        break;
      case T_COMP_LT:
        binEx->type = B_LT;
        break;
      case T_COMP_LTE:
        binEx->type = B_LTE;
        break;
      case T_COMP_GT:
        binEx->type = B_GT;
        break;
      case T_COMP_GTE:
        binEx->type = B_GTE;
        break;
      case T_COMP_NE:
        binEx->type = B_NE;
        break;
    }

    result->content.binaryExpression = binEx;
    return result;
  }

  return arithmaticRes;
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

    freeToken(op);
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
    case T_IDENTIFIER: {
      dequeue(list);
      nakedRes->type = E_VARIABLE;
      VariableExpression* variable =
          (VariableExpression*)malloc(sizeof(VariableExpression));
      variable->identifier = token;
      nakedRes->content.variableExpression = variable;
    } break;

    case T_NUMBER: {
      dequeue(list);
      nakedRes->type = E_LITERAL;
      LiteralExpression* literal =
          (LiteralExpression*)malloc(sizeof(LiteralExpression));
      literal->type = L_NUMBER;
      literal->value.number = atof(token->lexeme);
      nakedRes->content.literalExpression = literal;
      freeToken(token);
    } break;

    case T_LPAR: {
      dequeue(list);
      freeToken(token);
      nakedRes->type = E_GROUPING;
      GroupingExpression* grouping =
          (GroupingExpression*)malloc(sizeof(GroupingExpression));
      grouping->expression = parseExpression(list);
      nakedRes->content.groupingExpression = grouping;

      Token* rightPar = peek(list);
      expectToken(list, rightPar, T_RPAR,
                  "ERR: Expected ')' at '%s' on line %d.\n", rightPar->lexeme,
                  rightPar->line);

    } break;
    case T_STRING: {
      dequeue(list);
      nakedRes->type = E_LITERAL;
      LiteralExpression* literal =
          (LiteralExpression*)malloc(sizeof(LiteralExpression));
      literal->type = L_STRING;
      literal->value.string = token->lexeme;
      nakedRes->content.literalExpression = literal;
      // Don't free lexeme.
      free(token);
    } break;
    default:
      printf("ERR: Unexpected token at '%s' on line %d.\n", token->lexeme,
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