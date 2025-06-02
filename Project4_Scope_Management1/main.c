/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "reader.h"
#include "parser.h"

#define MAX_PATH 260
#define IO_ERROR -1
 /******************************************************************/

int main(int argc, char *argv[]) {

    char filepath[MAX_PATH];

    printf("Enter the input file path: ");
    if (fgets(filepath, sizeof(filepath), stdin) == NULL) {
        printf("Error reading input.\n");
        return -1;
    }

    // Remove newline character if present
    filepath[strcspn(filepath, "\n")] = '\0';

  if (compile(filepath) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}
