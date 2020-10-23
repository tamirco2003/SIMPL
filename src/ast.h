// Responsible for abstract syntax tree data structure.
#ifndef AST_H
#define AST_H

#include "tokens.h"

// Declarations and aliases for abstract syntax tree structs.
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

// Defines statement. Includes type, type-relevant content, and next statement.
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

// Defines declaration. Includes identifier and expression.
struct _declaration {
  Token* identifier;
  Expression* expression;
};

// Defines print. Includes expression.
struct _printStatement {
  Expression* expression;
};

// Defines expression. Includes type and type-relevant content.
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

// Defines binary expression. Includes type, left side expression, and right
// side expression.
struct _binaryExpression {
  enum BinExpType {
    B_ADD,
    B_SUB,
    B_MUL,
    B_DIV,
    B_MOD,
  } type;
  Expression* left;
  Expression* right;
};

// Defines unary expression. Includes type and left side expression.
struct _unaryExpression {
  enum UnxpType {
    U_MINUS,
  } type;
  Expression* right;
};

// Defines grouping expression. Includes expression.
struct _groupingExpression {
  Expression* expression;
};

// Defines literals. Includes value.
struct _literalExpression {
  enum LiteralType {
    L_NUMBER,
  } type;
  union LiteralValue {
    double number;
  } value;
};

// Defines assignment. Includes identifier and expression.
struct _assignmentExpression {
  Token* identifier;
  Expression* expression;
};

// Defines variable. Includes identifier.
struct _variableExpression {
  Token* identifier;
};

#endif