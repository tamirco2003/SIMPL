#include <process.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "debug\debugprints.h"
#include "evaluator\threadedevaluator.h"
#include "interfaces\threadlist.h"
#include "parser\threadedparser.h"
#include "scanner\threadedscanner.h"

#define OPTION_PREFIX '-'
#define VERBOSE_OPTION "-v"
#define THREAD_COUNT 3

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

  // Create thread safe token queue.
  ListStruct* tokenQueue = createList(LST_TOKEN, verbose);
  // Create thread safe statement queue.
  ListStruct* statementQueue = createList(LST_STATEMENT, verbose);

  // Queue array to pass to parser.
  ListStruct* queues[] = {tokenQueue, statementQueue};

  HANDLE threads[THREAD_COUNT];
  // Scanner thread.
  threads[0] = (HANDLE)_beginthreadex(NULL, 0, &scan, tokenQueue, 0, NULL);
  // Parser thread.
  threads[1] = (HANDLE)_beginthreadex(NULL, 0, &parse, queues, 0, NULL);
  // Evaluator thread.
  threads[2] = (HANDLE)_beginthreadex(NULL, 0, &evaluate, statementQueue, 0, NULL);

  WaitForMultipleObjects(THREAD_COUNT, threads, TRUE, INFINITE);

  printf("Finished!\n");
}
