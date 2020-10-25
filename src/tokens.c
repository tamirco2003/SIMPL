#include "tokens.h"

#include <stdlib.h>

void freeToken(Token* token) {
  free(token->lexeme);
  free(token);
}