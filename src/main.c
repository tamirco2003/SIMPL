#include <stdio.h>
#include <string.h>

#include "debug\debugprints.h"
#include "evaluator\evaluator.h"
#include "parser\parser.h"
#include "scanner\scanner.h"

#define OPTION_PREFIX '-'
#define VERBOSE_OPTION "-v"

int main(int argc, char* argv[]) {
  bool verbose = false;
  char* filename = NULL;

  // Parse command line arguments.
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == OPTION_PREFIX) {
      if (strcmp(argv[i], VERBOSE_OPTION) == 0) {
        verbose = true;
      } else {
        printf("Could not recognize option '%s'.\n", argv[i]);
        return 0;
      }
    } else if (filename == NULL) {
      filename = argv[i];
    }
  }

  // If file found in command, try to open it.
  if (filename != NULL) {
    if (openFile(filename) == 0) {
      printf("Could not open file '%s'.\n", filename);
      return 0;
    }
  } else {
    setFile(stdin);
  }

  List* tokenList = scan();
  closeFile();

  if (verbose) {
    printf("Scanner Pretty Print:\n");
    Node* n = tokenList->head;
    while (n != NULL) {
      printToken(n->value);
      n = n->next;
    }
    printf("\n\n");
  }

  Statement* top = parse(tokenList);

  if (top == NULL) {
    printf("\nParser failed.");
    return;
  }

  if (verbose) {
    printf("\n\nParser Pretty Print:\n");
    Statement* stmt = top;
    while (stmt != NULL) {
      printStatement(stmt, 0);
      stmt = stmt->next;
    }

    printf("\n\nEvaluation:\n");
  }

  evaluate(top);
}
