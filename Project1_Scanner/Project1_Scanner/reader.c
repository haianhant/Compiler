/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"

FILE *inputStream;
int lineNo, colNo;
int currentChar;

int readChar(void) {
  currentChar = getc(inputStream);
  colNo ++;
  if (currentChar == '\n') {
    lineNo ++;
    colNo = 0;
  }
  return currentChar;
}

openInputStream() {
    inputStream = fopen("D:\\Desktop\\Scanner\\obj\\Debug\\Downloads\\Project1_Scanner\\Project1_Scanner\\err1.kpl", "rt");
    if (inputStream == NULL)
    {printf("Khong thay \n");return IO_ERROR;}
    else printf("Da mo xong \n");

  lineNo = 1;
  colNo = 0;
  readChar();
  return IO_SUCCESS;
}

void closeInputStream() {
  fclose(inputStream);
}

