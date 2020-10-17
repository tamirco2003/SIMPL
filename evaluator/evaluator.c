#include "evaluator.h"

#include <stdlib.h>

#include "dictionary.h"

void runtimeError() { exit(1); }

DictEntry** dictionary;

void evaluate(Statement* statement) {
  dictionary = createDict();

  while (statement != NULL) {
    evaluateStatement(statement);
    statement = statement->next;
  }
}

void evaluateStatement(Statement* statement) {
  switch (statement->type) {
    case S_DECLARATION:
      Declaration* declaration = statement->content.declaration;
      double value = 0;
      if (declaration->expression != NULL) {
        value = evaluateExpression(declaration->expression);
      }
      setDict(dictionary, declaration->identifier->lexeme, value);

      // CAN REDEFINE VARS! SUPER PROBLEMATIC!
      break;
    case S_EXPRESSION:
      evaluateExpression(statement->content.expression);
      break;
    case S_PRINT:
      printf("%f\n",
             evaluateExpression(statement->content.printStatement->expression));
      break;
  }
}

double evaluateExpression(Expression* expression) {
  switch (expression->type) {
    case E_BINARY: {
      double leftValue =
          evaluateExpression(expression->content.binaryExpression->left);
      double rightValue =
          evaluateExpression(expression->content.binaryExpression->right);

      switch (expression->content.binaryExpression->type) {
        case B_ADD:
          return leftValue + rightValue;
        case B_SUB:
          return leftValue - rightValue;
        case B_MUL:
          return leftValue * rightValue;
        case B_DIV:
          return leftValue / rightValue;
        case B_MOD:
          return (int)leftValue % (int)rightValue;
      }

      printf("WARNING: SHOULD NOT PRINT (Binary post-evaluation)\n");
      break;
    }

    case E_UNARY: {
      double value =
          evaluateExpression(expression->content.unaryExpression->right);
      switch (expression->content.unaryExpression->type) {
        case U_MINUS:
          return -value;
      }

      printf("WARNING: SHOULD NOT PRINT (Unary post-evaluation)\n");
      break;
    }

    case E_GROUPING:
      return evaluateExpression(
          expression->content.groupingExpression->expression);

      printf("WARNING: SHOULD NOT PRINT (Grouping post-evaluation)\n");
      break;

    case E_LITERAL:
      return expression->content.literalExpression->value.number;

      printf("WARNING: SHOULD NOT PRINT (Literal post-evaluation)\n");
      break;

    case E_ASSIGNMENT: {
      Token* identifier = expression->content.assignmentExpression->identifier;

      DictEntry* var = getFromDict(
          dictionary,
          expression->content.assignmentExpression->identifier->lexeme);
      if (var == NULL) {
        printf("ERR: Unrecognized identifier '%s' on line %d.",
               identifier->lexeme, identifier->line);
        runtimeError();
      }

      double value = evaluateExpression(
          expression->content.assignmentExpression->expression);
      var->value = value;
      return value;

      printf("WARNING: SHOULD NOT PRINT (Assignment post-evaluation)\n");
      break;
    }

    case E_VARIABLE: {
      Token* identifier = expression->content.assignmentExpression->identifier;
      DictEntry* var = getFromDict(dictionary, identifier->lexeme);

      if (var == NULL) {
        printf("ERR: Unrecognized identifier '%s' on line %d.",
               identifier->lexeme, identifier->line);
        runtimeError();
      }

      return var->value;

      printf("WARNING: SHOULD NOT PRINT (Variable post-evaluation)\n");
      break;
    }
  }

  printf(
      "WARNING: SHOULD NOT PRINT (EXPRESSION post-evaluation - returning "
      "0!)\n");
  return 0;
}