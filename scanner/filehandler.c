#include "filehandler.h"

FILE* sourceFile;

int openFile(const char* filename) {
  sourceFile = fopen(filename, "r");

  if (sourceFile == NULL) {
    return 0;
  }

  return 1;
}

char getNextChar() {
  return fgetc(sourceFile);
}

char peekNextChar() {
  char c = getNextChar();
  ungetc(c, sourceFile);
  return c;
}