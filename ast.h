// Abstract syntax tree implementation.
#ifndef AST_H
#define AST_H

#include "tokens.h"

typedef struct _top AstTop;
typedef struct _statement Statement;
typedef struct _declaration Declaration;
typedef struct _printStatement PrintStatement;
typedef struct _expression Expression;
typedef struct _binaryExpression BinaryExpression;
typedef struct _unaryExpression UnaryExpression;
typedef struct _groupingExpression GroupingExpression;
typedef struct _literalExpression LiteralExpression;
typedef struct _assignmentExpression AssignmentExpression;
typedef struct _variableExpression VariableExpression;

struct _top {
  Statement* statement;
};

struct _statement {
  enum StatementType {
    S_EXPRESSION,
    S_DECLARATION,
    S_PRINT,
  } type;

  union StatementContent {
    Expression* expression;
    Declaration* declaration;
    PrintStatement* printStatement;
  } content;

  Statement* next;
};

struct _declaration {
  Token* identifier;
  Expression* expression;
};

struct _printStatement {
  Expression* expression;
};

struct _expression {
  enum ExpressionType {
    E_BINARY,
    E_UNARY,
    E_GROUPING,
    E_LITERAL,
    E_ASSIGNMENT,
    E_VARIABLE,
  } type;

  union ExpressionContent {
    BinaryExpression* binaryExpression;
    UnaryExpression* unaryExpression;
    GroupingExpression* groupingExpression;
    LiteralExpression* literalExpression;
    AssignmentExpression* assignmentExpression;
    VariableExpression* variableExpression;
  } content;
};

struct _binaryExpression {
  enum BinExpType {
    B_ADD,
    B_SUB,
    B_MUL,
    B_DIV,
    B_MOD,
  } type;
  Expression* left;
  // Token* op;
  Expression* right;
};

struct _unaryExpression {
  enum UnxpType {
    U_MINUS,
  } type;
  // Token* op;
  Expression* right;
};

struct _groupingExpression {
  Expression* expression;
};

struct _literalExpression {
  union LiteralValue {
    double number;
  } value;
};

struct _assignmentExpression {
  Token* left;
  Expression* right;
};

struct _variableExpression {
  Token* value;
};

#endif