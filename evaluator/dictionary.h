#ifndef DICTIONARY_H
#define DICTIONARY_H

// Initial (and currently constant) size of dictionary.
#define DICTIONARY_SIZE 16

// Defines dictionary entry. Includes pointer to next entry with the same hash,
// a char pointer for the key, and a double for the value.
typedef struct _dictEntry {
  struct _dictEntry* next;
  char* key;
  double value;
} DictEntry;

// Simple string hashing function for dictionary keys. Takes in char pointer,
// returns int hash.
int hashKey(char* key);

// Creates a DictEntry array and returns a pointer to it's beginning.
DictEntry** createDict();
// Takes in dictionary and key, returns pointer to DictEntry, or NULL if no such
// key is found.
DictEntry* getFromDict(DictEntry** dict, char* key);
// Takes in dictionary, char pointer key, and double value, creates new
// DictEntry and inserts it into the dictionary accordingly.
void setDict(DictEntry** dict, char* key, double value);

#endif