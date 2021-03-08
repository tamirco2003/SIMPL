#include "threadedparser.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// #include "..\interfaces\statementqueue.h"
#include "..\interfaces\threadlist.h"

static ListStruct* tokenQueue = NULL;
static ListStruct* statementQueue = NULL;

static bool error;
static jmp_buf error_buf;

void parserError() {
  error = true;
  longjmp(error_buf, 1);
}

void synchronize() {
  Token* next = peekItem(tokenQueue)->token;
  while (next->type != T_SEMICOLON && next->type != T_RBRACE &&
         next->type != T_EOF) {
    next = dequeueItem(tokenQueue)->token;
    freeToken(next);
    next = peekItem(tokenQueue)->token;
  }
}

void expectToken(Token* token, TokenTeeeepe type, char* strToFormat, ...) {
  va_list args;
  va_start(args, strToFormat);

  if (token->type == type) {
    dequeueItem(tokenQueue);
    freeToken(token);
    va_end(args);
  } else {
    vprintf(strToFormat, args);
    va_end(args);
    parserError();
  }
}

unsigned __stdcall parse(void* queuesArg) {
  ListStruct** queues = (ListStruct**)queuesArg;
  tokenQueue = queues[0];
  statementQueue = queues[1];
  error = false;

  Token* token = peekItem(tokenQueue)->token;
  while (token->type != T_EOF) {
    if (!setjmp(error_buf)) {
      Statement* statement = parseStatement();

      ListItem* item = (ListItem*)malloc(sizeof(ListItem));
      item->statement = statement;
      enqueueItem(statementQueue, item);
    } else {
      // On error, stop evaluation.
      ListItem* item = (ListItem*)malloc(sizeof(ListItem));
      item->statement = (Statement*)malloc(sizeof(Statement));
      item->statement->type = S_END;
      enqueueItem(statementQueue, item);

      synchronize();

      token = dequeueItem(tokenQueue)->token;
      if (token->type != T_EOF) {
        freeToken(token);
      } else {
        return 1;
      }
    }

    token = peekItem(tokenQueue)->token;
  }

  //   free(list);
  // Add queue "closing"?

  ListItem* item = (ListItem*)malloc(sizeof(ListItem));
  item->statement = (Statement*)malloc(sizeof(Statement));
  item->statement->type = S_END;
  enqueueItem(statementQueue, item);

  if (error) {
    return 1;
  }

  printf("Finished parsing!\n");
  return 0;
}

Statement* parseStatement() {
  Statement* result = (Statement*)malloc(sizeof(Statement));
  result->next = NULL;

  Token* token = peekItem(tokenQueue)->token;

  switch (token->type) {
    case T_LBRACE:
      dequeueItem(tokenQueue)->token;
      freeToken(token);

      result->type = S_BLOCK;
      result->content.blockBody = parseBlock();
      break;
    case T_PRINT:
      dequeueItem(tokenQueue)->token;
      freeToken(token);

      result->type = S_PRINT;
      result->content.printStatement = parsePrint();
      break;
    case T_LET:
      dequeueItem(tokenQueue)->token;
      freeToken(token);

      result->type = S_DECLARATION;
      result->content.declaration = parseDeclaration();
      break;
    case T_IF:
      dequeueItem(tokenQueue)->token;
      freeToken(token);

      result->type = S_IF;
      result->content.ifStatement = parseIfStatement();
      break;
    case T_WHILE:
      dequeueItem(tokenQueue)->token;
      freeToken(token);

      result->type = S_WHILE;
      result->content.whileStatement = parseWhileStatement();
      break;
    case T_DO:
      dequeueItem(tokenQueue)->token;
      freeToken(token);

      result->type = S_DOWHILE;
      result->content.whileStatement = parseDoWhileStatement();
      break;
    default:
      result->type = S_EXPRESSION;
      result->content.expression = parseExpressionStatement();
      break;
  }

  return result;
}

Expression* parseExpressionStatement() {
  Expression* result = parseExpression();

  Token* semicolon = peekItem(tokenQueue)->token;
  expectToken(semicolon, T_SEMICOLON,
              "ERR (exprstmt): Expected ';' at/before '%s' on line %d.\n",
              semicolon->lexeme, semicolon->line);

  return result;
}

Statement* parseBlock() {
  Statement* head = NULL;
  Statement* tail = NULL;

  Token* next = peekItem(tokenQueue)->token;
  while (next->type != T_RBRACE) {
    if (next->type == T_EOF) {
      printf("ERR: Unclosed block, expected '}' before end of file.\n");
      parserError();
    }

    Statement* stmt = parseStatement();

    if (tail == NULL) {
      head = stmt;
    } else {
      tail->next = stmt;
    }

    tail = stmt;

    next = peekItem(tokenQueue)->token;
  }
  dequeueItem(tokenQueue)->token;
  freeToken(next);

  return head;
}

PrintStatement* parsePrint() {
  PrintStatement* result = (PrintStatement*)malloc(sizeof(PrintStatement));
  result->expression = parseExpression();

  Token* semicolon = peekItem(tokenQueue)->token;
  expectToken(semicolon, T_SEMICOLON,
              "ERR (print): Expected ';' at/before '%s' on line %d.\n",
              semicolon->lexeme, semicolon->line);

  return result;
}

Declaration* parseDeclaration() {
  Declaration* result = (Declaration*)malloc(sizeof(Declaration));
  result->expression = NULL;

  // will replace when stack lexeme problem is solved.
  Token* identifier = peekItem(tokenQueue)->token;
  if (identifier->type != T_IDENTIFIER) {
    printf("ERR: Expected identifier at '%s' on line %d.\n", identifier->lexeme,
           identifier->line);
    parserError();
  }
  dequeueItem(tokenQueue)->token;
  result->identifier = identifier;

  Token* next = peekItem(tokenQueue)->token;
  if (next->type == T_EQUALS) {
    dequeueItem(tokenQueue)->token;
    freeToken(next);

    result->expression = parseExpression();
  }

  Token* semicolon = peekItem(tokenQueue)->token;
  expectToken(semicolon, T_SEMICOLON,
              "ERR (decl): Expected ';' at/before '%s' on line %d.\n",
              semicolon->lexeme, semicolon->line);

  return result;
}

IfStatement* parseIfStatement() {
  IfStatement* result = (IfStatement*)malloc(sizeof(IfStatement));

  Token* expected;

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_LPAR, "ERR: Expected '(' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->condition = parseExpression();

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->body = parseStatement();

  expected = peekItem(tokenQueue)->token;
  if (expected->type == T_ELSE) {
    dequeueItem(tokenQueue)->token;
    result->elseBody = parseStatement();
  } else {
    result->elseBody = NULL;
  }

  return result;
}

WhileStatement* parseWhileStatement() {
  WhileStatement* result = (WhileStatement*)malloc(sizeof(WhileStatement));

  Token* expected;

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_LPAR, "ERR: Expected '(' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->condition = parseExpression();

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->body = parseStatement();

  return result;
}

WhileStatement* parseDoWhileStatement() {
  WhileStatement* result = (WhileStatement*)malloc(sizeof(WhileStatement));

  result->body = parseStatement();

  Token* expected;

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_WHILE, "ERR: Expected 'while' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_LPAR, "ERR: Expected '(' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  result->condition = parseExpression();

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
              expected->lexeme, expected->line);

  expected = peekItem(tokenQueue)->token;
  expectToken(expected, T_SEMICOLON,
              "ERR (dowhile): Expected ';' at/before '%s' on line %d.\n",
              expected->lexeme, expected->line);

  return result;
}

Expression* parseExpression() {
  Expression* result = (Expression*)malloc(sizeof(Expression));

  ListItem* firstItem = dequeueItem(tokenQueue);
  Token* first = firstItem->token;
  Token* next = peekItem(tokenQueue)->token;
  if (next->type == T_EQUALS) {
    dequeueItem(tokenQueue)->token;
    freeToken(next);

    result->type = E_ASSIGNMENT;

    AssignmentExpression* assign =
        (AssignmentExpression*)malloc(sizeof(AssignmentExpression));

    assign->identifier = first;
    assign->expression = parseExpression();
    result->content.assignmentExpression = assign;

    return result;
  }

  // pushToken(first);
  pushItem(tokenQueue, firstItem);

  return disjunction();
}

Expression* disjunction() {
  Expression* conjunctionRes = conjunction();

  Token* op = peekItem(tokenQueue)->token;

  while (op->type == T_OR) {
    dequeueItem(tokenQueue)->token;

    Expression* res = (Expression*)malloc(sizeof(Expression));
    res->type = E_BINARY;

    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->type = B_OR;
    binEx->left = conjunctionRes;
    binEx->right = conjunction();

    res->content.binaryExpression = binEx;

    conjunctionRes = res;
    freeToken(op);  // PROBLEM! Freeing strings initialized without malloc...
    op = peekItem(tokenQueue)->token;
  }

  return conjunctionRes;
}

Expression* conjunction() {
  Expression* inversionRes = inversion();

  Token* op = peekItem(tokenQueue)->token;

  while (op->type == T_AND) {
    dequeueItem(tokenQueue)->token;

    Expression* res = (Expression*)malloc(sizeof(Expression));
    res->type = E_BINARY;

    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->type = B_AND;
    binEx->left = inversionRes;
    binEx->right = inversion();

    res->content.binaryExpression = binEx;

    inversionRes = res;
    freeToken(op);  // ERR! Freeing strings initialized without malloc...
    op = peekItem(tokenQueue)->token;
  }

  return inversionRes;
}

Expression* inversion() {
  Expression* head = NULL;
  Expression* tail = NULL;

  Token* token = peekItem(tokenQueue)->token;

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

    dequeueItem(tokenQueue)->token;
    token = peekItem(tokenQueue)->token;
  }

  Expression* nakedRes = comparison();

  if (tail == NULL) {
    return nakedRes;
  }

  tail->content.unaryExpression->right = nakedRes;
  return head;
}

Expression* comparison() {
  Expression* arithmaticRes = arithmatic();

  Token* token = peekItem(tokenQueue)->token;

  if (token->type == T_COMP_EQUALS || token->type == T_COMP_LT ||
      token->type == T_COMP_LTE || token->type == T_COMP_GT ||
      token->type == T_COMP_GTE || token->type == T_COMP_NE) {
    dequeueItem(tokenQueue)->token;

    Expression* result = (Expression*)malloc(sizeof(Expression));
    result->type = E_BINARY;
    BinaryExpression* binEx =
        (BinaryExpression*)malloc(sizeof(BinaryExpression));
    binEx->left = arithmaticRes;
    binEx->right = arithmatic();

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

Expression* arithmatic() {
  Expression* termRes = term();

  Token* op = peekItem(tokenQueue)->token;

  while (op->type == T_PLUS || op->type == T_MINUS) {
    dequeueItem(tokenQueue)->token;

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

    binEx->right = term();

    res->content.binaryExpression = binEx;

    termRes = res;

    freeToken(op);
    op = peekItem(tokenQueue)->token;
  }

  return termRes;
}

Expression* term() {
  Expression* factorRes = factor();

  Token* op = peekItem(tokenQueue)->token;

  while (op->type == T_STAR || op->type == T_SLASH || op->type == T_PERCENT) {
    dequeueItem(tokenQueue)->token;

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

    binEx->right = factor();

    res->content.binaryExpression = binEx;

    factorRes = res;

    freeToken(op);
    op = peekItem(tokenQueue)->token;
  }

  return factorRes;
}

Expression* factor() {
  Expression* head = NULL;
  Expression* tail = NULL;

  Token* token = peekItem(tokenQueue)->token;

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

    dequeueItem(tokenQueue)->token;
    token = peekItem(tokenQueue)->token;
  }

  Expression* nakedRes = (Expression*)malloc(sizeof(Expression));

  switch (token->type) {
    case T_IDENTIFIER: {
      dequeueItem(tokenQueue)->token;
      nakedRes->type = E_VARIABLE;
      VariableExpression* variable =
          (VariableExpression*)malloc(sizeof(VariableExpression));
      variable->identifier = token;
      nakedRes->content.variableExpression = variable;
    } break;

    case T_NUMBER: {
      dequeueItem(tokenQueue)->token;
      nakedRes->type = E_LITERAL;
      LiteralExpression* literal =
          (LiteralExpression*)malloc(sizeof(LiteralExpression));
      literal->type = L_NUMBER;
      literal->value.number = atof(token->lexeme);
      nakedRes->content.literalExpression = literal;
      freeToken(token);
    } break;

    case T_LPAR: {
      dequeueItem(tokenQueue)->token;
      freeToken(token);
      nakedRes->type = E_GROUPING;
      GroupingExpression* grouping =
          (GroupingExpression*)malloc(sizeof(GroupingExpression));
      grouping->expression = parseExpression();
      nakedRes->content.groupingExpression = grouping;

      Token* rightPar = peekItem(tokenQueue)->token;
      expectToken(rightPar, T_RPAR, "ERR: Expected ')' at '%s' on line %d.\n",
                  rightPar->lexeme, rightPar->line);

    } break;
    case T_STRING: {
      dequeueItem(tokenQueue)->token;
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