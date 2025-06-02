/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];
int state;
int ln, cn;
char str[MAX_IDENT_LEN + 1];
char numStr[20];
char c;
/***************************************************************/
Token* getToken(void)
{
  Token *token;
  switch(state)
  {
  case 0:
  	if (currentChar == EOF) state =1;
  	else
  	switch (charCodes[currentChar])
  	{
  		case CHAR_SPACE:
  			state =2;break;
  		case CHAR_LETTER:
            ln=lineNo;
            cn=colNo;
            state =3;
            break;
  		case CHAR_DIGIT:
            ln = lineNo;
            cn = colNo;
            state =7;
            break;
  		case CHAR_PLUS:
            ln = lineNo;
            cn = colNo;
            state = 9;
            break;
  		case CHAR_MINUS:
            ln = lineNo;
            cn = colNo;
            state =10;
            break;
  		case CHAR_TIMES:
            ln = lineNo;
            cn = colNo;
            state =11;
            break;
  		case CHAR_SLASH:
            ln = lineNo;
            cn = colNo;
            state =12;
            break;
  		case CHAR_LT:
            ln = lineNo;
            cn = colNo;
            state =13;
            break;
  		case CHAR_GT:
            ln = lineNo;
            cn = colNo;
            state= 16;
            break;
  		case CHAR_EQ:
            ln = lineNo;
            cn = colNo;
            state =19;
            break;
  		case CHAR_EXCLAMATION:
            ln = lineNo;
            cn = colNo;
            state = 20;
            break;
  		case CHAR_COMMA:
            ln = lineNo;
            cn = colNo;
            state =23;
            break;
  		case CHAR_PERIOD:
            ln = lineNo;
            cn = colNo;
            state =24;
			break;
  		case CHAR_SEMICOLON:
            ln = lineNo;
            cn = colNo;
            state=27;
			break;
  		case CHAR_COLON:
            ln = lineNo;
            cn = colNo;
            state =28;
			break;
   		case CHAR_SINGLEQUOTE:
            ln = lineNo;
            cn = colNo;
            state =31;
            break;
        case CHAR_LPAR:
		    ln = lineNo;
            cn = colNo;
            state = 35;
            break;
        case CHAR_RPAR:
            ln = lineNo;
            cn = colNo;
            state= 42;
			break;
        default:
            state=43;
    }
	return getToken();
  case 1:
  	return makeToken(TK_EOF, lineNo, colNo);
  case 2:
    //TODO (Skip blanks)
    while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
      readChar();
    }
    state = 0;
    return getToken();
  case 3: {
    // ToDo
    int i = 0;
    while (currentChar != EOF &&
           (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)) {
        if (i < MAX_IDENT_LEN)
            str[i++] = (char)currentChar;
        else {
            error(ERR_IDENTTOOLONG, ln, cn);
        }
        readChar();
    }

    str[i] = '\0';
    state = 4;
    return getToken();
  }

  case 4: {
    TokenType tempType = checkKeyword(str);
    if (tempType == TK_NONE)
        state = 5;
    else {
        state = 6;
    }
    return getToken();
  }

  case 5: {
    // Identifier
    token = makeToken(TK_IDENT, ln, cn);
    strcpy(token->string, str);
    state = 0;
    return token;
}

  case 6: {
    // Keyword
    TokenType kwType = checkKeyword(str);
    token = makeToken(kwType, ln, cn);
    strcpy(token->string, str);
    state = 0;
    return token;
}

  case 7: {
	//TODO  Numbers
    int i = 0;
    while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
      if (i < 10)
        numStr[i++] = (char)currentChar;
      else
        error(ERR_NUMBERTOOLONG, lineNo, lineNo);
      readChar();
    }
    numStr[i] = '\0';
    state = 8;
    return getToken();
  }
  case 8:
    token = makeToken(TK_NUMBER, ln, cn);
    strcpy(token->string, numStr);
    token->value = atoi(numStr);
    state = 0;
    return token;
  case 9:
	  readChar();
	  state = 0; //add
    return makeToken(SB_PLUS, ln, cn);
  case 10:
	//TODO
	  readChar();
	  state = 0;
    return makeToken(SB_MINUS, ln, cn);
  case 11:
	//TODO
	  readChar();
    state = 0;
    return makeToken(SB_TIMES, ln, cn);
  case 12:
	//TODO
	  readChar();
    state = 0;
    return makeToken(SB_SLASH, ln, cn);
  case 13:
    readChar();
   	if (charCodes[currentChar] == CHAR_EQ) state = 14; else state =15;
	  return getToken();
  case 14:
	  readChar();
	  state = 0; //add
	  return makeToken(SB_LE, ln, cn);
  case 15:
    readChar();
    state = 0; //add
	  return makeToken(SB_LT, ln, cn);
  case 16:
	//TODO
	readChar();
    if (charCodes[currentChar] == CHAR_EQ)
      state = 17;
    else
      state = 18;
    return getToken();
  case 17:
	//TODO
	  readChar();
	  state = 0;
    return makeToken(SB_GE, lineNo, colNo - 1);
  case 18:
	//TODO
	  readChar();
    state = 0;
    return makeToken(SB_GT, lineNo, colNo - 1);
  case 19:
	//TODO
	  readChar();
    state = 0;
    return makeToken(SB_EQ, ln, cn);
  case 20:
	//TODO
	  readChar();
    if (charCodes[currentChar] == CHAR_EQ)
      state = 21;
    else
      state = 22;
    return getToken();
  case 21:
    readChar();
    state = 0;
    return makeToken(SB_NEQ, lineNo, colNo-1);
  case 22:
    token = makeToken(TK_NONE, lineNo, colNo-1);
    error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    state = 0;
    return token;
  case 23:
	//TODO
	  readChar();
    state = 0;
    return makeToken(SB_COMMA, ln, cn);
  case 24:
    //TODO
    readChar();
    if (charCodes[currentChar] == CHAR_RPAR) {
      state = 25;
      return getToken();
    } else {
      state = 26;
      return getToken();
    }
  case 25:
    //TODO
    readChar();
    state = 0;
    return makeToken(SB_RSEL, ln, cn);
  case 26:
    //TODO
    readChar();
    state = 0;
    return makeToken(SB_PERIOD, ln, cn);
  case 27:
    //TODO
    readChar();
    state = 0;
    return makeToken(SB_SEMICOLON, ln, cn);
  case 28:
    //TODO
    {
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      state = 29;
      return getToken();
    } else {
      state = 30;
      return getToken();
    }
  }
  case 29:
	//TODO
    readChar();
    state = 0;
    return makeToken(SB_ASSIGN, ln, cn);
  case 30:
    //TODO
    readChar();
    state = 0;
    return makeToken(SB_COLON, ln, cn);
  case 31:
    readChar();
  	if (currentChar == EOF)
  		state=34;
  		else
			if(isprint(currentChar))
		  		state =32;
		  	else state =34;
  		return getToken();
  case 32:
	  c= currentChar;
  	readChar();
  	if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
		  state=33;
		else
			state =34;
  		return getToken();
  case 33:
  	token = makeToken(TK_CHAR, lineNo, colNo-2);
  	token->string[0] =c;
  	token->string[1] ='\0';
  	readChar();
  	state = 0;
  	return token;
  case 34:
  	error(ERR_INVALIDCHARCONSTANT, lineNo, colNo-2);
  	state = 0;
  	return getToken();
  case 35: // tokens begin with lpar, skip comments
    readChar();
    if (currentChar == EOF)
		state=41;
    else
    	switch (charCodes[currentChar])
		{
    		case CHAR_PERIOD:
                state =36;
                break;
    		case CHAR_TIMES:
    			state =37;
                break;
            default:state =41;
        }
    return getToken();

  case 36:
  	//TODO
  	readChar();
    state = 0;
    return makeToken(SB_LSEL, ln, cn);
  case 37:
  	//TODO
  	//in the cmt
  	while (1) {
      if (currentChar == EOF) { state = 40; break; }
      if (charCodes[currentChar] == CHAR_TIMES) {
        state = 38;
        break;
      }
      readChar();
    }
    return getToken();
  case 38:
  	//TODO
  	readChar();
    if (currentChar == EOF) {
      state = 40;
      return getToken();
    }
    else if (charCodes[currentChar] == CHAR_RPAR) {
      state = 39;
      return getToken();
    }
    else if (charCodes[currentChar] == CHAR_TIMES) {
      state = 38;
    } else {
      state = 37;
      return getToken();
    }
  case 39:
  	//TODO
  	readChar();
    state = 0;
    return getToken();
  case 40:
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
    state = 0;
    return getToken();
  case 41:
    state =0;
    return makeToken(SB_LPAR, ln, cn);
  case 42:
    readChar();
    state = 0;
    return makeToken(SB_RPAR, ln, cn);
  case 43:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    state = 0;
    return token;
  }
}

/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

/*int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}*/

int main()
{

if (scan("D:\\Desktop\\Scanner\\obj\\Debug\\Downloads\\Project1_Scanner\\Project1_Scanner\\example3.kpl") == IO_ERROR) {
   printf("Can\'t read input file!\n");
      }
return 0;

}


