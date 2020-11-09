// Responsible for scope structure.
#ifndef SCOPE_H
#define SCOPE_H

#include <stdbool.h>

#include "dictionary.h"

typedef struct _scope {
  DictEntry** entries;
  struct _scope* enclosing;
} Scope;

// Creates a new scope and it's internal dictionary. Takes in the enclosing
// scope, calls createDict;
Scope* createScope(Scope* enclosing);

// Takes in a scope pointer, token pointer "identifier", and LiteralExpression
// pointer "value". If identifier doesn't already exist in scope, adds value to
// the dictionary with key identifier. Calls getFromDict and setDict.
bool declareVar(Scope* scope, Token* identifier, LiteralExpression* value);

// Takes in a scope pointer and token pointer "identifier", searches scope and
// enclosing scopes for key identifier. If found, returns a pointer to the
// dictionary entry, otherwise returns null. Calls getFromDict.
DictEntry* searchForEntry(Scope* scope, Token* identifier);

// Takes in a scope pointer, token pointer "identifier", and LiteralExpression
// pointer "value". Calls searchForEntry to find variable in dictionary and sets
// it's value. Returns true if successful, false if variable is not found.
bool assignVar(Scope* scope, Token* identifier, LiteralExpression* value);

// Takes in a scope pointer and token pointer "identifier". Calls searchForEntry
// to find variable in dictionary and returns it's value. Returns null if not
// found.
LiteralExpression* getVar(Scope* scope, Token* identifier);

#endif