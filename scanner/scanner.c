#include "scanner.h"

#include <stdlib.h>
#include <string.h>

#include "filehandler.h"

#define KEYWORD_COUNT 2
#define BUFFER_SIZE 500

void append(char* str, char c) {
  int len = strlen(str);
  str[len++] = c;
  str[len] = '\0';
}

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isAlpha(char c) {
  return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char* number(char c) {
  char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
  memset(buffer, '\0', sizeof(char) * BUFFER_SIZE);
  append(buffer, c);

  bool decimal = false;
  char next = peekNextChar();

  while (next == '.' || isDigit(next)) {
    if (next == '.') {
      if (!decimal) {
        decimal = true;
        append(buffer, next);
      } else {
        return NULL;
      }
    } else {
      append(buffer, next);
    }

    getNextChar();
    next = peekNextChar();
  }

  char* result = malloc(sizeof(char) * strlen(buffer));
  strcpy(result, buffer);

  free(buffer);

  return result;
}

char* keyword(char c) {
  char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
  memset(buffer, '\0', sizeof(char) * BUFFER_SIZE);
  append(buffer, c);

  char next = peekNextChar();

  while (isAlpha(next) || isDigit(next)) {
    append(buffer, next);
    getNextChar();
    next = peekNextChar();
  }

  char* result = malloc(sizeof(char) * strlen(buffer));
  strcpy(result, buffer);

  free(buffer);

  return result;
}

TokenType keywordType(char* keyword) {
  char* keywords[KEYWORD_COUNT] = {"print", "let"};

  for (int i = 0; i < KEYWORD_COUNT; i++) {
    if (strcmp(keywords[i], keyword) == 0) {
      return (TokenType)(i + T_PRINT);
    }
  }

  return T_IDENTIFIER;
}

int scan(List* list) {
  int line = 1;

  char c = getNextChar();
  while (c != EOF) {
    switch (c) {
      case '+':
        addSToken(list, T_PLUS, line);
        break;
      case '-':
        addSToken(list, T_MINUS, line);
        break;
      case '*':
        addSToken(list, T_STAR, line);
        break;
      case '/':
        addSToken(list, T_SLASH, line);
        break;
      case '(':
        addSToken(list, T_LPAR, line);
        break;
      case ')':
        addSToken(list, T_RPAR, line);
        break;
      case ';':
        addSToken(list, T_SEMICOLON, line);
        break;
      case '=':
        addSToken(list, T_EQUALS, line);
        break;
      case '%':
        addSToken(list, T_PERCENT, line);
        break;
      case '\n':
        line++;
      case ' ':
      case '\t':
      case '\r':
        break;
      default:
        if (isDigit(c)) {
          char* num = number(c);
          if (num == NULL) {
            printf("ERR: Multiple decimal points found on line '%d'\n", line);
            return 1;
          }
          addToken(list, T_NUMBER, line, num);
        } else if (isAlpha(c)) {
          char* name = keyword(c);
          TokenType type = keywordType(name);
          addToken(list, type, line, name);
        } else {
          printf("ERR: Unrecognized character '%c' on line '%d'\n", c, line);
          return 1;
        }
        break;
    }

    c = getNextChar();
  }

  addSToken(list, T_EOF, line);

  return 0;
}