#include "threadlist.h"

#include <stdio.h>

static const char* const TokenTypeString[] = {
    "PLUS",       "MINUS",    "STAR",    "SLASH",       "LPAR",    "RPAR",
    "SEMICOLON",  "EQUALS",   "PERCENT", "COMP_EQUALS", "COMP_LT", "COMP_LTE",
    "COMP_GT",    "COMP_GTE", "COMP_NE", "LBRACE",      "RBRACE",  "NUMBER",
    "IDENTIFIER", "STRING",   "PRINT",   "LET",         "OR",      "AND",
    "NOT",        "IF",       "ELSE",    "WHILE",       "DO",      "EOF"};

static const char* const StatementTypeString[] = {
    "BLOCK", "EXPRESSION", "DECLARATION", "PRINT",
    "IF",    "WHILE",      "DOWHILE",     "END",
};

ListStruct* createList(ListType type, int verbose) {
  ListStruct* list = (ListStruct*)malloc(sizeof(ListStruct));
  list->type = type;
  list->listSemaphore = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
  list->listMutex = CreateMutexW(NULL, FALSE, NULL);
  list->head = NULL;
  list->tail = NULL;
  list->verbose = verbose;
  return list;
}

void enqueueItem(ListStruct* list, ListItem* item) {
  WaitForSingleObject(list->listMutex, INFINITE);

  ListNode* node = (ListNode*)malloc(sizeof(ListNode));
  node->value = item;
  node->next = NULL;

  if (list->head == NULL) {
    list->head = node;
  } else if (list->tail != NULL) {
    list->tail->next = node;
  }

  list->tail = node;

  if (list->verbose) {
    printf("Enqueued ");
    switch (list->type) {
      case LST_TOKEN:
        printf("%s Token\n", TokenTypeString[item->token->type]);
        break;
      case LST_STATEMENT:
        printf("%s Statement\n", StatementTypeString[item->statement->type]);
        break;
      case LST_CHAR:
        printf("\"%s\"\n", item->character);
        break;
      default:
        printf("something?\n");
    }
  }

  ReleaseMutex(list->listMutex);
  ReleaseSemaphore(list->listSemaphore, 1, NULL);
}

ListItem* dequeueItem(ListStruct* list) {
  WaitForSingleObject(list->listSemaphore, INFINITE);
  WaitForSingleObject(list->listMutex, INFINITE);

  if (list->head == NULL) {
    printf("THREAD LIST NULL DEQUEUE ERROR\n");
    ReleaseMutex(list->listMutex);
    return NULL;
  }

  ListItem* item = list->head->value;
  ListNode* newHead = list->head->next;
  free(list->head);
  list->head = newHead;

  if (list->verbose) {
    printf("Dequeued ");
    switch (list->type) {
      case LST_TOKEN:
        printf("%s Token\n", TokenTypeString[item->token->type]);
        break;
      case LST_STATEMENT:
        printf("%s Statement\n", StatementTypeString[item->statement->type]);
        break;
      case LST_CHAR:
        printf("\"%s\"\n", item->character);
        break;
      default:
        printf("something?\n");
    }
  }
  
  ReleaseMutex(list->listMutex);
  return item;
}

ListItem* peekItem(ListStruct* list) {
  WaitForSingleObject(list->listSemaphore, INFINITE);
  WaitForSingleObject(list->listMutex, INFINITE);

  ListItem* item = list->head->value;

  if (list->verbose) {
    printf("Peeked ");
    switch (list->type) {
      case LST_TOKEN:
        printf("%s Token\n", TokenTypeString[item->token->type]);
        break;
      case LST_STATEMENT:
        printf("%s Statement\n", StatementTypeString[item->statement->type]);
        break;
      case LST_CHAR:
        printf("\"%s\"\n", item->character);
        break;
      default:
        printf("something?\n");
    }
  }

  ReleaseMutex(list->listMutex);
  ReleaseSemaphore(list->listSemaphore, 1, NULL);

  return item;
}

void pushItem(ListStruct* list, ListItem* item) {
  WaitForSingleObject(list->listMutex, INFINITE);

  ListNode* node = (ListNode*)malloc(sizeof(ListNode));
  node->value = item;
  node->next = list->head;
  list->head = node;

  if (list->tail == NULL) {
    list->tail = node;
  }

  if (list->verbose) {
    printf("Pushed ");
    switch (list->type) {
      case LST_TOKEN:
        printf("%s Token\n", TokenTypeString[item->token->type]);
        break;
      case LST_STATEMENT:
        printf("%s Statement\n", StatementTypeString[item->statement->type]);
        break;
      case LST_CHAR:
        printf("\"%s\"\n", item->character);
        break;
      default:
        printf("something?\n");
    }
  }

  ReleaseMutex(list->listMutex);
  ReleaseSemaphore(list->listSemaphore, 1, NULL);
}
