#ifndef CHARLIST_H
#define CHARLIST_H

// Defines a character list node.
typedef struct _charNode {
  char value;
  struct _charNode* next;
} CharNode;

// Takes in a character, returns a new CharNode.
CharNode* createCharNode(char value);

// Takes in a charNode and the length of the list, returns a string representation of the charNode.
char* toString(CharNode* charNode, int length);


#endif