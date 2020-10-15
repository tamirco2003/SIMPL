#ifndef TOKENLIST_H
#define TOKENLIST_H

#include "tokens.h"

// Defines node in token list.
typedef struct _node {
  struct _node* next;
  Token* value;
} Node;

// Defines list.
typedef struct _list {
  Node* head;
  Node* tail;
} List;

// Creates list.
List* createList();

// Adds token to end of list.
void enqueue(List* list, Token* token);
// Creates token and enqueues it to list.
void addToken(List* list, TokenType type, int line, char* lexeme);
// Creates single char token and enqueues it to list.
void addSToken(List* list, TokenType type, int line);
// Removes token from start of list and returns it.
Token* dequeue(List* list);
// Returns token from start of list without removing it.
Token* peek(List* list);
// Adds token to start of list.
void push(List* list, Token* token);

#endif