// Responsible for scanning/tokenization functionality, naive implementation.

#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

#include "..\tokenlist.h"

// Exits the program when scanner encounters an error.
void scannerError();

// Takes in char pointer and char, appends char to char pointer.
void append(char* str, char c);

// Takes in char, returns true if char is a digit, false otherwise.
bool isDigit(char c);
// Takes in char, returns true if char is a letter or underscore, false
// otherwise.
bool isAlpha(char c);

// Takes in first char, keeps going until number is found or error occurs.
char* number(char c);
// Takes in first char, keeps going until keyword is found. Calls keywordType
// for reserved keywords.
char* keyword(char c);

// Takes in char pointer and returns type of keyword, one of reserved or
// identifier.
TokenType keywordType(char* keyword);

// Main scanning function. Calls addToken to populate list with tokens, calls
// isDigit, isAlpha, number, and keyword accordingly.
List* scan();

#endif