// Responsible for Token data structure.
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

  T_COMP_EQUALS,
  T_COMP_LT,
  T_COMP_LTE,
  T_COMP_GT,
  T_COMP_GTE,
  T_COMP_NE,

  T_LBRACE,
  T_RBRACE,

  T_NUMBER,
  T_IDENTIFIER,
  T_STRING,

  T_PRINT,
  T_LET,
  T_OR,
  T_AND,
  T_NOT,
  T_IF,
  T_ELSE,

  T_EOF,
} TokenType;

// Defines token. Includes token type, line number, and char pointer to lexeme.
typedef struct _token {
  TokenType type;
  int line;
  char* lexeme;
} Token;

// Frees token and it's insides (lexeme).
void freeToken(Token* token);

#endif