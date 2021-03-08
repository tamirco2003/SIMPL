// Responsible for lexing/scanning/tokenifying, naive switch case algorithm.
#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>
#include "..\tokens.h"

// #include "..\tokenlist.h"

// Exits the program when scanner encounters an error.
void scannerError();

// Takes in char, returns true if char is a digit, false otherwise.
bool isDigit(char c);
// Takes in char, returns true if char is a letter or underscore, false
// otherwise.
bool isAlpha(char c);

// Returns a char pointer to a string containing the number currently being
// scanned.
char* number();
// Returns a char pointer to a string containing the keyword currently being
// scanned.
char* keyword();
// Returns a char pointer to a string containing the string literal currently
// being scanned.
char* stringLit(int line);

// Takes in char pointer and returns type of keyword, reserved or
// identifier.
TokenTeeeepe keywordType(char* keyword);

void addToken(TokenTeeeepe type, int line, char* lexeme);

// Main scanning function. Calls addToken to populate list with tokens, calls
// isDigit, isAlpha, number, keyword, and keywordType accordingly.
unsigned __stdcall scan(void* pArguments);

#endif