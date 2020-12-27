#include <stdio.h>

#include "debug\debugprints.h"
#include "evaluator\evaluator.h"
#include "parser\parser.h"
#include "scanner\scanner.h"

int main(int argc, char* argv[]) {
  // input loop??!?
  // FILE* stdinfile = stdin;
  // char c;
  // do {
  //   c = fgetc(stdinfile);
  //   printf("%c", c);
  // } while (c != EOF);

  if (argc >= 2) {
    char* filename = argv[1];
    if (openFile(filename) == 0) {
      printf("Could not open file '%s'.\n", filename);
      return 0;
    }
  } else {
    setFile(stdin);
  }

  List* tokenList = scan();

  closeFile();

  printf("Scanner Pretty Print:\n");

  Node* n = tokenList->head;
  while (n != NULL) {
    printToken(n->value);
    n = n->next;
  }

  printf("\n\n");

  Statement* top = parse(tokenList);

  if (top == NULL) {
    printf("\nParser failed.");
    return;
  }

  printf("\n\nParser Pretty Print:\n");
  Statement* stmt = top;
  while (stmt != NULL) {
    printStatement(stmt, 0);
    stmt = stmt->next;
  }

  printf("\n\nEvaluation:\n");
  evaluate(top);
}
