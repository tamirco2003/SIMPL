#include "evaluator.h"

#include <stdlib.h>
#include <string.h>

#include "dictionary.h"
#include "scope.h"

void runtimeError() { exit(1); }

// static DictEntry** dictionary;
static Scope* scope;

void evaluate(Statement* statement) {
  // dictionary = createDict();
  scope = createScope(NULL);

  while (statement != NULL) {
    evaluateStatement(statement);
    statement = statement->next;
  }
}

void evaluateStatement(Statement* statement) {
  switch (statement->type) {
    case S_BLOCK:
      evaluateBlock(statement->content.blockBody);
      break;
    case S_DECLARATION:
      evaluateDeclaration(statement->content.declaration);
      break;
    case S_EXPRESSION:
      evaluateExpression(statement->content.expression);
      break;
    case S_PRINT:
      evaluatePrint(statement->content.printStatement);
      break;
    case S_IF:
      evaluateIf(statement->content.ifStatement);
      break;
    case S_WHILE:
      evaluateWhile(statement->content.whileStatement);
      break;
    case S_DOWHILE:
      evaluateDoWhile(statement->content.whileStatement);
      break;
    default:
      printf("ERR: Unrecognized statement type.\n");
      runtimeError();
      break;
  }
}

void evaluateBlock(Statement* statement) {
  scope = createScope(scope);

  while (statement != NULL) {
    evaluateStatement(statement);
    statement = statement->next;
  }

  scope = destroyScope(scope);
}

void evaluateDeclaration(Declaration* declaration) {
  LiteralExpression* value;

  if (declaration->expression != NULL) {
    value = evaluateExpression(declaration->expression);
  } else {
    value = (LiteralExpression*)malloc(sizeof(LiteralExpression));
    value->type = L_UNDEFINED;
  }

  if (!declareVar(scope, declaration->identifier, value)) {
    printf("ERR: Redefined variable '%s' on line %d.",
           declaration->identifier->lexeme, declaration->identifier->line);
    runtimeError();
  }
}

void evaluatePrint(PrintStatement* printStatement) {
  LiteralExpression* value = evaluateExpression(printStatement->expression);

  switch (value->type) {
    case L_UNDEFINED:
      printf("undefined\n");
      break;
    case L_NUMBER:
      printf("%f\n", value->value.number);
      break;
    case L_STRING:
      printf("%s\n", value->value.string);
      break;
    default:
      printf("ERR: Unrecognized type!\n");
      runtimeError();
  }
}

void evaluateIf(IfStatement* ifStatement) {
  if (isTruthy(evaluateExpression(ifStatement->condition))) {
    evaluateBlock(ifStatement->body);
  } else if (ifStatement->elseBody != NULL) {
    evaluateBlock(ifStatement->elseBody);
  }
}

void evaluateWhile(WhileStatement* whileStatement) {
  while (isTruthy(evaluateExpression(whileStatement->condition))) {
    evaluateBlock(whileStatement->body);
  }
}

void evaluateDoWhile(WhileStatement* whileStatement) {
  do {
    evaluateBlock(whileStatement->body);
  } while (isTruthy(evaluateExpression(whileStatement->condition)));
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
    case E_ASSIGNMENT:
      return evaluateAssignment(expression->content.assignmentExpression);
    case E_VARIABLE:
      return evaluateVariable(expression->content.variableExpression);
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

  if (binaryExpression->type == B_OR) {
    if (isTruthy(left)) {
      return left;
    }

    return right;
  } else if (binaryExpression->type == B_AND) {
    if (isTruthy(left)) {
      return right;
    }

    return left;
  } else if (binaryExpression->type == B_EQUAL) {
    result->type = L_NUMBER;

    if (left->type != right->type) {
      result->value.number = 0;
    } else if (left->type == L_UNDEFINED) {
      result->value.number = 1;
    } else if (left->type == L_NUMBER) {
      result->value.number = left->value.number == right->value.number;
    } else if (left->type == L_STRING) {
      if (strcmp(left->value.string, right->value.string) == 0) {
        result->value.number = 1;
      } else {
        result->value.number = 0;
      }
    }

    return result;
  } else if (binaryExpression->type == B_NE) {
    result->type = L_NUMBER;

    if (left->type != right->type) {
      result->value.number = 1;
    } else if (left->type == L_UNDEFINED) {
      result->value.number = 0;
    } else if (left->type == L_NUMBER) {
      result->value.number = left->value.number != right->value.number;
    } else if (left->type == L_STRING) {
      result->value.number = 1;

      if (strcmp(left->value.string, right->value.string) == 0) {
        result->value.number = 0;
      }
    }

    return result;
  } else if (left->type == L_NUMBER && right->type == L_NUMBER) {
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
      case B_LT:
        result->value.number = left->value.number < right->value.number;
        break;
      case B_LTE:
        result->value.number = left->value.number <= right->value.number;
        break;
      case B_GT:
        result->value.number = left->value.number > right->value.number;
        break;
      case B_GTE:
        result->value.number = left->value.number >= right->value.number;
        break;
    }

    // free(binaryExpression);
    // free(left);
    // free(right);
    return result;
  } else if (left->type == L_STRING && right->type == L_STRING) {
    result->type = L_STRING;

    if (binaryExpression->type == B_ADD) {
      char* strResult = calloc(1, sizeof(char));
      strcat(strResult, left->value.string);
      strcat(strResult, right->value.string);

      result->value.string = strResult;
      return result;
    }
  } else if (binaryExpression->type == B_MUL) {
    result->type = L_STRING;

    char* originalStr;
    int times;

    if (left->type == L_STRING && right->type == L_NUMBER) {
      originalStr = left->value.string;
      times = right->value.number;
    } else if (left->type == L_NUMBER && right->type == L_STRING) {
      originalStr = right->value.string;
      times = left->value.number;
    }

    char* strResult = "";
    for (int i = 0; i < times; i++) {
      strcat(strResult, originalStr);
    }

    result->value.string;
    return result;
  }

  printf("ERR: Binary expression type mismatch!\n");
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
        break;
      case U_NOT:
        if (value->value.number == 0) {
          result->value.number = 1;
        } else {
          result->value.number = 0;
        }
        break;
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
  LiteralExpression* value = evaluateExpression(assignment->expression);

  if (!assignVar(scope, assignment->identifier, value)) {
    printf("ERR: Unrecognized identifier '%s' on line %d.",
           assignment->identifier->lexeme, assignment->identifier->line);
    runtimeError();
  }

  return value;
}

LiteralExpression* evaluateVariable(VariableExpression* variable) {
  LiteralExpression* value = getVar(scope, variable->identifier);

  if (value == NULL) {
    printf("ERR: Unrecognized identifier '%s' on line %d.",
           variable->identifier->lexeme, variable->identifier->line);
    runtimeError();
  }

  return value;
}

int isTruthy(LiteralExpression* value) {
  if (value->type == L_UNDEFINED) {
    return 0;
  }

  if (value->type == L_NUMBER) {
    if (value->value.number == 0) {
      return 0;
    }
    return 1;
  }

  if (value->type == L_STRING) {
    if (value->value.string[0] == '\0') {
      return 0;
    }
    return 1;
  }

  return 0;
}