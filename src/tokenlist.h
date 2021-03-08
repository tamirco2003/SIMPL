// Responsible for Token list/queue data structure.
#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "tokens.h"

// Defines node in token list. Includes next node and token value.
typedef struct _node {
  struct _node* next;
  Token* value;
} Node;

// Defines list. Includes head and tail of list.
typedef struct _list {
  Node* head;
  Node* tail;
} List;

// Creates and returns a new list.
List* createList();

// Takes in a list and a token, adds token to end of list.
void enqueue(List* list, Token* token);
// Takes in a list, and everything needed to make a token. Creates new token and
// calls enqueue.
void addToken(List* list, TokenTeeeepe type, int line, char* lexeme);
// Removes token from start of list and returns it.
Token* dequeue(List* list);
// Returns token from start of list without removing it.
Token* peek(List* list);
// Adds token to start of list.
void push(List* list, Token* token);

#endif