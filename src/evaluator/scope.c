#include "scope.h"

#include <stdlib.h>

Scope* createScope(Scope* enclosing) {
  Scope* newScope = (Scope*)malloc(sizeof(Scope));
  newScope->enclosing = enclosing;
  newScope->entries = createDict();
  return newScope;
}

Scope* destroyScope(Scope* scope) {
  destroyDict(scope->entries);
  Scope* enclosing = scope->enclosing;
  free(scope);
  return enclosing;
}

bool declareVar(Scope* scope, Token* identifier, LiteralExpression* value) {
  DictEntry** dict = scope->entries;

  // Make sure it var doesn't exist in current scope.
  if (getFromDict(dict, identifier->lexeme) != NULL) {
    return false;
  }

  setDict(dict, identifier->lexeme, value);
  return true;
}

DictEntry* searchForEntry(Scope* scope, Token* identifier) {
  Scope* currentScope = scope;

  while (currentScope != NULL) {
    DictEntry* entry = getFromDict(currentScope->entries, identifier->lexeme);

    if (entry != NULL) {
      return entry;
    }

    currentScope = currentScope->enclosing;
  }

  return NULL;
}

bool assignVar(Scope* scope, Token* identifier, LiteralExpression* value) {
  DictEntry* entry = searchForEntry(scope, identifier);

  if (entry == NULL) return false;

  entry->value = value;
  return true;
}

LiteralExpression* getVar(Scope* scope, Token* identifier) {
  DictEntry* entry = searchForEntry(scope, identifier);

  if (entry == NULL) return NULL;

  return entry->value;
}