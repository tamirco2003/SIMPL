#include "threadedscanner.h"

#include <stdlib.h>
#include <string.h>

#include "..\interfaces\threadlist.h"
#include "charlist.h"
#include "filehandler.h"

#define KEYWORD_COUNT 9
static const char* const keywords[KEYWORD_COUNT] = {
    "print", "let", "or", "and", "not", "if", "else", "while", "do"};
#define FIRST_KEYWORD T_PRINT

static ListStruct* tokenQueue = NULL;

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

char* stringLit(int line) {
  char first = getNextChar();
  int length = 0;

  CharNode* head = createCharNode(first);
  CharNode* tail = head;

  char c = peekNextChar();
  while (c != first) {
    // Handle unclosed strings.
    if (c == '\n' || c == EOF) {
      printf("ERR: Unclosed string literal on line '%d'.\n", line);
      scannerError();
    }
    // Handle escape sequences.
    if (c == '\\') {
      getNextChar();
      c = peekNextChar();
      switch (c) {
        case 'b':
          tail->next = createCharNode('\b');
          break;
        case 't':
          tail->next = createCharNode('\t');
          break;
        case 'n':
          tail->next = createCharNode('\n');
          break;
        case 'f':
          tail->next = createCharNode('\f');
          break;
        case 'r':
          tail->next = createCharNode('\r');
          break;
        default:
          tail->next = createCharNode(c);
          break;
      }
    } else {
      tail->next = createCharNode(c);
    }

    getNextChar();
    tail = tail->next;
    length++;
    c = peekNextChar();
  }

  getNextChar();  // Get rid of trailing quote.
  CharNode* stringStart = head->next;
  free(head);
  return toString(stringStart, length);
}

TokenTeeeepe keywordType(char* keyword) {
  for (int i = 0; i < KEYWORD_COUNT; i++) {
    if (strcmp(keywords[i], keyword) == 0) {
      return (TokenTeeeepe)(i + FIRST_KEYWORD);
    }
  }

  return T_IDENTIFIER;
}

void addToken(TokenTeeeepe type, int line, char* lexeme) {
  Token* token = (Token*)malloc(sizeof(Token));
  token->type = type;
  token->line = line;

  if (lexeme != NULL) {
    token->lexeme = (char*)malloc(sizeof(char) * (strlen(lexeme) + 1));
    strcpy(token->lexeme, lexeme);
  } else {
    token->lexeme = NULL;
  }

  ListItem* item = (ListItem*)malloc(sizeof(ListItem));
  item->token = token;

  // enqueueToken(token);
  enqueueItem(tokenQueue, item);
}

unsigned __stdcall scan(void* queue) {
  tokenQueue = queue;

  int line = 1;
  char c = peekNextChar();

  while (c != EOF) {
    switch (c) {
      case '+':
        getNextChar();
        addToken(T_PLUS, line, "+");
        break;
      case '-':
        getNextChar();
        addToken(T_MINUS, line, "-");
        break;
      case '*':
        getNextChar();
        addToken(T_STAR, line, "*");
        break;
      case '/':
        getNextChar();
        if (peekNextChar() == '/') {
          while (peekNextChar() != '\n') {
            getNextChar();
          }
        } else
          addToken(T_SLASH, line, "/");
        break;
      case '(':
        getNextChar();
        addToken(T_LPAR, line, "(");
        break;
      case ')':
        getNextChar();
        addToken(T_RPAR, line, ")");
        break;
      case ';':
        getNextChar();
        addToken(T_SEMICOLON, line, ";");
        break;
      case '=':
        getNextChar();

        if (peekNextChar() == '=') {
          getNextChar();
          addToken(T_COMP_EQUALS, line, "==");
        } else
          addToken(T_EQUALS, line, "=");

        break;
      case '%':
        getNextChar();
        addToken(T_PERCENT, line, "%");
        break;
      case '<':
        getNextChar();

        if (peekNextChar() == '=') {
          getNextChar();
          addToken(T_COMP_LTE, line, "<=");
        } else
          addToken(T_COMP_LT, line, "<");

        break;
      case '>':
        getNextChar();

        if (peekNextChar() == '=') {
          getNextChar();
          addToken(T_COMP_GTE, line, ">=");
        } else
          addToken(T_COMP_GT, line, ">");

        break;
      case '!':
        getNextChar();
        if (getNextChar() == '=') {
          addToken(T_COMP_NE, line, "!=");
        } else {
          printf("ERR: Unrecognized character '!' on line '%d'\n", c, line);
          scannerError();
        }

        break;
      case '{':
        getNextChar();
        addToken(T_LBRACE, line, "{");
        break;
      case '}':
        getNextChar();
        addToken(T_RBRACE, line, "}");
        break;
      case '\'':
      case '"':
        char* stringLiteral = stringLit(line);
        addToken(T_STRING, line, stringLiteral);
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
            printf("ERR: Multiple decimal points found on line '%d'\n", line);
            scannerError();
          }

          addToken(T_NUMBER, line, num);
        } else if (isAlpha(c)) {
          char* name = keyword();
          TokenTeeeepe type = keywordType(name);

          addToken(type, line, name);
        } else {
          printf("ERR: Unrecognized character '%c' on line '%d'\n", c, line);
          scannerError();
        }
        break;
    }

    c = peekNextChar();
  }

  addToken(T_EOF, line, "");

  printf("Finished scanning!\n");
  return 0;
}