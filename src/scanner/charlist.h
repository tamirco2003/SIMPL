#ifndef CHARLIST_H
#define CHARLIST_H

typedef struct _charNode {
  char value;
  struct _charNode* next;
} CharNode;

CharNode* createCharNode(char value);
char* toString(CharNode* charNode, int length);


#endif