// Responsible for file handling.
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stdio.h>

// Takes in file name and sets a static file pointer. Calls fopen. Returns 0 if
// failed, 1 if successful.
int openFile(const char* filename);

// Returns the next char and advances the pointer. Calls fgetc.
char getNextChar();
// Returns the next char and does not advance the pointer. Calls getNextChar and
// ungetc.
char peekNextChar();

// Closes a file. Calls fclose and returns its value. Returns 0 on success and
// EOF on failure.
int closeFile();

#endif