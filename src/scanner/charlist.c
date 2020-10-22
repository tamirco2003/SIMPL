#include "charlist.h"

#include <stdlib.h>

CharNode* createCharNode(char value) {
  CharNode* charNode = (CharNode*)malloc(sizeof(CharNode));
  charNode->value = value;
  charNode->next = NULL;
  return charNode;
}

char* toString(CharNode* charNode, int length) {
  CharNode* currentNode = charNode;
  char* string = calloc(length + 1, sizeof(char));

  for (int i = 0; currentNode != NULL; i++) {
    string[i] = currentNode->value;
    CharNode* nextNode = currentNode->next;
    free(currentNode);
    currentNode = nextNode;
  }

  string[length] = '\0';

  return string;
}