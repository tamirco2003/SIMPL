#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stdio.h>

// Opens a file.
int openFile(const char* filename);

// Gets the next char and advances the pointer.
char getNextChar();
// Gets the next char and does not advance the pointer.
char peekNextChar();

#endif