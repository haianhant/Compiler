/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h> // For exit()
#include <string.h>

#include "reader.h"   // Should define IO_SUCCESS, IO_ERROR
#include "parser.h"   // For compile()
#include "codegen.h"  // For initCodeBuffer, serialize, printCodeBuffer, cleanCodeBuffer

int dumpCode = 1; // <<<< ASM dump enabled by default

void printUsage(void) {
  printf("Usage: kplc [input_kpl_file output_code_file]\n");
  printf("  If file paths are not provided, you will be prompted.\n");
}

/******************************************************************/

int main(int argc, char *argv[]) {
  char inputFile[256];  // Buffer for input file path
  char outputFile[256]; // Buffer for output file path (binary code)

  // --- Argument Handling ---
  if (argc == 3) {
    // Case 1: Input and Output files provided as command-line arguments
    strncpy(inputFile, argv[1], sizeof(inputFile) - 1);
    inputFile[sizeof(inputFile) - 1] = '\0'; // Ensure null-termination

    strncpy(outputFile, argv[2], sizeof(outputFile) - 1);
    outputFile[sizeof(outputFile) - 1] = '\0'; // Ensure null-termination
    printf("Using input file (from command line): %s\n", inputFile);
    printf("Using output file (from command line): %s\n", outputFile);
  } else if (argc > 1 && argc != 3) {
    // Case 2: Incorrect number of command-line arguments (not 0, not 2 for files)
    printf("kplc: Incorrect number of arguments.\n");
    printUsage();
    return -1;
  } else {
    // Case 3: No command-line arguments for files (argc == 1), prompt the user
    printf("Enter the path for the KPL input source file: ");
    if (scanf("%255s", inputFile) != 1) {
      fprintf(stderr, "Error reading input file path.\n");
      return -1;
    }
    while (getchar() != '\n'); // Clear the input buffer

    printf("Enter the path for the compiled output file (e.g., program.out): ");
    if (scanf("%255s", outputFile) != 1) {
      fprintf(stderr, "Error reading output file path.\n");
      return -1;
    }
    while (getchar() != '\n'); // Clear the input buffer
  }

  // --- Compilation Process ---
  initCodeBuffer(); // Initialize the code buffer once

  printf("Compiling '%s'...\n", inputFile);
  if (compile(inputFile) == IO_ERROR) {
    printf("Compilation failed for '%s'. Check if the file exists and is readable.\n", inputFile);
    cleanCodeBuffer();
    return -1;
  }
  printf("Compilation successful.\n");

  printf("Serializing to '%s'...\n", outputFile);
  if (serialize(outputFile) == IO_ERROR) {
    printf("Failed to write binary output to '%s'.\n", outputFile);
    cleanCodeBuffer();
    return -1;
  }
  printf("Serialization successful.\n");

  // The dumpCode is set to 1 by default at the top of this file.
  // So, the ASM dump will always be printed if compilation & serialization succeed.
  if (dumpCode) {
    printf("\n--- Generated ASM Code (Console Dump) ---\n");
    printCodeBuffer();
    printf("--- End of ASM Code Dump ---\n");
  }

  cleanCodeBuffer(); // Clean up the code buffer at the end

  printf("\nProcess completed. Binary code saved to '%s'.\n", outputFile);
  return 0; // Indicate overall success
}
