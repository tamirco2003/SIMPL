#include "evaluator.h"

#include <stdlib.h>

#include "dictionary.h"

void runtimeError() { exit(1); }

static DictEntry** dictionary;

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
      evaluateDeclaration(statement->content.declaration);
      break;
    case S_EXPRESSION:
      evaluateExpression(statement->content.expression);
      break;
    case S_PRINT:
      evaluatePrint(statement->content.printStatement);
      break;
  }
}

void evaluateDeclaration(Declaration* declaration) {
  if (getFromDict(dictionary, declaration->identifier->lexeme) != NULL) {
    printf("ERR: Redefined variable '%s' on line %d.",
           declaration->identifier->lexeme, declaration->identifier->line);
    runtimeError();
  }

  LiteralExpression* value = NULL;
  if (declaration->expression != NULL) {
    value = evaluateExpression(declaration->expression);
  }
  setDict(dictionary, declaration->identifier->lexeme, value);
}

void evaluatePrint(PrintStatement* printStatement) {
  LiteralExpression* value = evaluateExpression(printStatement->expression);

  switch (value->type) {
    case L_NUMBER:
      printf("%f\n", value->value.number);
      break;
  }
}

LiteralExpression* evaluateExpression(Expression* expression) {
  switch (expression->type) {
    case E_BINARY:
      return evaluateBinaryExpression(expression->content.binaryExpression);
    case E_UNARY:
      return evaluateUnaryExpression(expression->content.unaryExpression);
    case E_GROUPING:
      return evaluateExpression(
          expression->content.groupingExpression->expression);
    case E_LITERAL:
      return expression->content.literalExpression;

      // printf("WARNING: SHOULD NOT PRINT (Literal post-evaluation)\n");
      // break;

    case E_ASSIGNMENT:
      return evaluateAssignment(expression->content.assignmentExpression);
      // {
      //   Token* identifier =
      //   expression->content.assignmentExpression->identifier;

      //   DictEntry* var = getFromDict(
      //       dictionary,
      //       expression->content.assignmentExpression->identifier->lexeme);
      //   if (var == NULL) {
      //     printf("ERR: Unrecognized identifier '%s' on line %d.",
      //            identifier->lexeme, identifier->line);
      //     runtimeError();
      //   }

      //   double value = evaluateExpression(
      //       expression->content.assignmentExpression->expression);
      //   var->value = value;
      //   return value;

      //   printf("WARNING: SHOULD NOT PRINT (Assignment
      //   post-evaluation)\n"); break;
      // }

    case E_VARIABLE:
      return evaluateVariable(expression->content.variableExpression);
      // {
      //   Token* identifier =
      //   expression->content.assignmentExpression->identifier; DictEntry*
      //   var = getFromDict(dictionary, identifier->lexeme);

      //   if (var == NULL) {
      //     printf("ERR: Unrecognized identifier '%s' on line %d.",
      //            identifier->lexeme, identifier->line);
      //     runtimeError();
      //   }

      //   return var->value;

      //   printf("WARNING: SHOULD NOT PRINT (Variable post-evaluation)\n");
      //   break;
      // }
  }

  printf(
      "WARNING: SHOULD NOT PRINT (EXPRESSION post-evaluation - returning "
      "0!)\n");
  return 0;
}

LiteralExpression* evaluateBinaryExpression(
    BinaryExpression* binaryExpression) {
  LiteralExpression* left = evaluateExpression(binaryExpression->left);
  LiteralExpression* right = evaluateExpression(binaryExpression->right);

  LiteralExpression* result =
      (LiteralExpression*)malloc(sizeof(LiteralExpression));

  if (left->type == L_NUMBER && right->type == L_NUMBER) {
    result->type = L_NUMBER;
    switch (binaryExpression->type) {
      case B_ADD:
        result->value.number = left->value.number + right->value.number;
        break;
      case B_SUB:
        result->value.number = left->value.number - right->value.number;
        break;
      case B_MUL:
        result->value.number = left->value.number * right->value.number;
        break;
      case B_DIV:
        result->value.number = left->value.number / right->value.number;
        break;
      case B_MOD:
        result->value.number =
            (int)left->value.number % (int)right->value.number;
        break;
    }

    // free(binaryExpression);
    // free(left);
    // free(right);
    return result;
  }

  printf("ERR: Bad binary thingy.\n");
  runtimeError();
  return NULL;
}

LiteralExpression* evaluateUnaryExpression(UnaryExpression* unaryExpression) {
  LiteralExpression* value = evaluateExpression(unaryExpression->right);

  LiteralExpression* result =
      (LiteralExpression*)malloc(sizeof(LiteralExpression));

  if (value->type == L_NUMBER) {
    result->type = L_NUMBER;
    switch (unaryExpression->type) {
      case U_MINUS:
        result->value.number = -value->value.number;
    }

    // free(unaryExpression);
    // free(value);
    return result;
  }

  printf("ERR: Cannot apply unary operator on non-number types.\n");
  runtimeError();
  return NULL;
}

LiteralExpression* evaluateAssignment(AssignmentExpression* assignment) {
  Token* identifier = assignment->identifier;

  DictEntry* var = getFromDict(dictionary, identifier->lexeme);

  if (var == NULL) {
    printf("ERR: Unrecognized identifier '%s' on line %d.", identifier->lexeme,
           identifier->line);
    runtimeError();
  }

  LiteralExpression* value = evaluateExpression(assignment->expression);
  var->value = value;
  return value;
}

LiteralExpression* evaluateVariable(VariableExpression* variable) {
  Token* identifier = variable->identifier;
  DictEntry* var = getFromDict(dictionary, identifier->lexeme);

  if (var == NULL) {
    printf("ERR: Unrecognized identifier '%s' on line %d.", identifier->lexeme,
           identifier->line);
    runtimeError();
  }

  return var->value;
}