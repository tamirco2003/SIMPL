#ifndef TOKENS_H
#define TOKENS_H

// Defines type of token.
typedef enum _tokenType {
  T_PLUS,
  T_MINUS,
  T_STAR,
  T_SLASH,
  T_LPAR,
  T_RPAR,
  T_SEMICOLON,
  T_EQUALS,
  T_PERCENT,

  T_NUMBER,
  T_IDENTIFIER,

  T_PRINT,
  T_LET,

  T_EOF,
} TokenType;

// Defines token. Includes token type, line number, and char pointer to lexeme.
typedef struct _token {
  TokenType type;
  int line;
  char* lexeme;
} Token;

#endif