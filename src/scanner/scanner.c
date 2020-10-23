#include "scanner.h"

#include <stdlib.h>
#include <string.h>

#include "charlist.h"
#include "filehandler.h"

#define KEYWORD_COUNT 2
static char* keywords[KEYWORD_COUNT] = {"print", "let"};
#define FIRST_KEYWORD T_PRINT

void scannerError() { exit(1); }

bool isDigit(char c) { return c >= '0' && c <= '9'; }

bool isAlpha(char c) {
  return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char* number() {
  char first = getNextChar();
  int length = 1;

  CharNode* head = createCharNode(first);
  CharNode* tail = head;

  bool decimal = false;

  char c = peekNextChar();
  while (isDigit(c) || c == '.') {
    if (c == '.') {
      if (decimal) {
        return NULL;
      }

      decimal = true;
    }

    tail->next = createCharNode(getNextChar());
    tail = tail->next;
    length++;
    c = peekNextChar();
  }

  return toString(head, length);
}

char* keyword() {
  char first = getNextChar();
  int length = 1;

  CharNode* head = createCharNode(first);
  CharNode* tail = head;

  char c = peekNextChar();
  while (isAlpha(c) || isDigit(c)) {
    tail->next = createCharNode(getNextChar());
    tail = tail->next;
    length++;
    c = peekNextChar();
  }

  return toString(head, length);
}

TokenType keywordType(char* keyword) {
  for (int i = 0; i < KEYWORD_COUNT; i++) {
    if (strcmp(keywords[i], keyword) == 0) {
      return (TokenType)(i + FIRST_KEYWORD);
    }
  }

  return T_IDENTIFIER;
}

void comment() { 
  getNextChar();
  while (getNextChar() != '\n') {}
}

List* scan() {
  List* list = createList();

  int line = 1;
  char c = peekNextChar();

  while (c != EOF) {
    switch (c) {
      case '+':
        getNextChar();
        addToken(list, T_PLUS, line, "+");
        break;
      case '-':
        getNextChar();
        addToken(list, T_MINUS, line, "-");
        break;
      case '*':
        getNextChar();
        addToken(list, T_STAR, line, "*");
        break;
      case '/':
        getNextChar();
        if (peekNextChar() == '/')
          comment();
        else
          addToken(list, T_SLASH, line, "/");
        break;
      case '(':
        getNextChar();
        addToken(list, T_LPAR, line, "(");
        break;
      case ')':
        getNextChar();
        addToken(list, T_RPAR, line, ")");
        break;
      case ';':
        getNextChar();
        addToken(list, T_SEMICOLON, line, ";");
        break;
      case '=':
        getNextChar();
        addToken(list, T_EQUALS, line, "=");
        break;
      case '%':
        getNextChar();
        addToken(list, T_PERCENT, line, "%");
        break;
      case '\n':
        line++;
      case ' ':
      case '\t':
      case '\r':
        getNextChar();
        break;
      default:
        if (isDigit(c)) {
          char* num = number();

          if (num == NULL) {
            printf("\nERR: Multiple decimal points found on line '%d'\n", line);
            scannerError();
          }

          addToken(list, T_NUMBER, line, num);
        } else if (isAlpha(c)) {
          char* name = keyword();
          TokenType type = keywordType(name);

          addToken(list, type, line, name);
        } else {
          printf("\nERR: Unrecognized character '%c' on line '%d'\n", c, line);
          scannerError();
        }
        break;
    }

    c = peekNextChar();
  }

  addToken(list, T_EOF, line, "");

  return list;
}