#include "tokenlist.h"

#include <stdlib.h>
#include <string.h>

List* createList() {
  List* list = (List*)malloc(sizeof(List));
  list->head = NULL;
  list->tail = NULL;
  return list;
}

void enqueue(List* list, Token* token) {
  Node* node = (Node*)malloc(sizeof(Node));
  node->value = token;
  node->next = NULL;

  if (list->tail == NULL) {
    list->head = node;
  } else {
    list->tail->next = node;
  }

  list->tail = node;
}

void addToken(List* list, TokenType type, int line, char* lexeme) {
  Token* token = (Token*)malloc(sizeof(Token));
  token->type = type;
  token->line = line;

  if (lexeme != NULL) {
    token->lexeme = (char*)malloc(sizeof(char) * (strlen(lexeme) + 1));
    strcpy(token->lexeme, lexeme);
  } else {
    token->lexeme = NULL;
  }

  enqueue(list, token);
}

Token* dequeue(List* list) {
  if (list->head == NULL) {
    return NULL;
  }

  Token* token = list->head->value;
  Node* newHead = list->head->next;
  free(list->head);
  list->head = newHead;

  return token;
}

Token* peek(List* list) { return list->head->value; }

void push(List* list, Token* token) {
  Node* node = (Node*)malloc(sizeof(Node));
  node->value = token;
  node->next = list->head;
  list->head = node;

  if (list->tail == NULL) {
    list->tail = node;
  }
}