// Simple "naive" scanner.

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

#include "..\tokenlist.h"

// Appends char to string.
void append(char* str, char c);

// Checks if char is digit.
bool isDigit(char c);
// Checks if char is letter or underscore.
bool isAlpha(char c);

// Finds number in source.
char* number(char c);
// Finds keyword in source.
char* keyword(char c);

// Returns type of keyword.
TokenType keywordType(char* keyword);

// Main scanning function.
int scan(List* list);

#endif