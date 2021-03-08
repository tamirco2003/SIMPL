#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <windows.h>

#include "..\ast.h"
#include "..\tokens.h"

typedef union _listItem {
  Token* token;
  Statement* statement;
  char* character;
} ListItem;

typedef struct _node {
  struct _node* next;
  ListItem* value;
} ListNode;

typedef enum _listType {
  LST_TOKEN,
  LST_STATEMENT,
  LST_CHAR,
} ListType;

typedef struct _list {
  ListType type;
  ListNode* head;
  ListNode* tail;
  HANDLE listMutex;
  HANDLE listSemaphore;

  int verbose;
} ListStruct;

ListStruct* createList(ListType type, int verbose);
void enqueueItem(ListStruct* list, ListItem* item);
// void enqueueToken(Token* token);
// void enqueueStatement(Statement* statement);
ListItem* dequeueItem(ListStruct* list);
ListItem* peekItem(ListStruct* list);
// void pushToken(Token* token);
void pushItem(ListStruct* list, ListItem* item);

#endif