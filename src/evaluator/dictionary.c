#include "dictionary.h"

#include <stdlib.h>
#include <string.h>

// Initial (and currently constant) size of dictionary.
#define DICTIONARY_SIZE 16

int hashKey(char* key) {
  int hash = 0;
  for (int i = 0; i < key[i] != '\0'; i++) {
    hash += key[i];
  }
  return hash % DICTIONARY_SIZE;
}

DictEntry** createDict() {
  DictEntry** dict = (DictEntry**)calloc(DICTIONARY_SIZE, sizeof(DictEntry*));
  return dict;
}

void destroyDict(DictEntry** dict) {
  for (int i = 0; i < DICTIONARY_SIZE; i++) {
    freeDictEntry(dict[i]);
  }
  free(dict);
}

void freeDictEntry(DictEntry* dictEntry) {
  if (dictEntry == NULL) return;

  freeDictEntry(dictEntry->next);
  free(dictEntry);
}

DictEntry* getFromDict(DictEntry** dict, char* key) {
  int hash = hashKey(key);
  DictEntry* entry = dict[hash];

  while (entry != NULL && strcmp(entry->key, key) != 0) {
    entry = entry->next;
  }

  return entry;
}

void setDict(DictEntry** dict, char* key, LiteralExpression* value) {
  int hash = hashKey(key);

  DictEntry* existingEntry = getFromDict(dict, key);
  if (existingEntry != NULL) {
    existingEntry->value = value;
    return;
  }

  DictEntry* entry = (DictEntry*)malloc(sizeof(DictEntry));
  entry->key = key;
  entry->value = value;
  entry->next = dict[hash];
  dict[hash] = entry;
}