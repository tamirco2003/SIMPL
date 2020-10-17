#include <stdio.h>

#include "debug\debugprints.h"
#include "evaluator\evaluator.h"
#include "parser\parser.h"
#include "scanner\scanner.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("USAGE: simpl FILE\n");
    return 0;
  }

  char* filename = argv[1];
  if (openFile(filename) == 0) {
    printf("Could not open file '%s'.\n", filename);
    return 0;
  }

  List* tokenList = createList();

  if (scan(tokenList) != 0) {
    return;
  }

  printf("Scanner Pretty Print:\n");

  Node* n = tokenList->head;
  while (n != NULL) {
    printToken(n->value);
    n = n->next;
  }

  Statement* top = parse(tokenList);

  printf("\n\nParser Pretty Print:\n");
  Statement* stmt = top;
  while (stmt != NULL) {
    printStatement(stmt);
    stmt = stmt->next;
  }

  printf("\n\nEvaluation:\n");
  evaluate(top);
}
