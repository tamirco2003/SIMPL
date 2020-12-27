#include "filehandler.h"

static FILE* sourceFile;

void setFile(FILE* file) {
  sourceFile = file;
}

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

int closeFile() {
  FILE* currFile = sourceFile;
  sourceFile = NULL;
  return fclose(currFile);
}