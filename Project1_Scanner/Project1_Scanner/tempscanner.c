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
char str[MAX_IDENT_LEN + 1];  // dự phòng ký tự null
char c;

Token* getToken(void) {
  Token *token;
  switch (state) {
  case 0:
    if (currentChar == EOF)
      state = 1;
    else
      switch (charCodes[currentChar]) {
        case CHAR_SPACE:
          state = 2; break;
        case CHAR_LETTER:
          ln = lineNo;
          cn = colNo;
          state = 3;
          break;
        case CHAR_DIGIT:
          ln = lineNo;
          cn = colNo;
          state = 7;
          break;
        case CHAR_PLUS:
          state = 9;
          break;
        case CHAR_MINUS:
          state = 10;
          break;
        case CHAR_TIMES:
          state = 11;
          break;
        case CHAR_SLASH:
          state = 12;
          break;
        case CHAR_LT:
          state = 13;
          break;
        case CHAR_GT:
          state = 16;
          break;
        case CHAR_EQ:
          state = 19;
          break;
        case CHAR_EXCLAMATION:
          state = 20;
          break;
        case CHAR_COMMA:
          state = 23;
          break;
        case CHAR_PERIOD:
          state = 24;
          break;
        case CHAR_SEMICOLON:
          state = 27;
          break;
        case CHAR_COLON:
          state = 28;
          break;
        case CHAR_SINGLEQUOTE:
          state = 31;
          break;
        case CHAR_LPAR:
          state = 35;
          break;
        case CHAR_RPAR:
          state = 42;
          break;
        default:
          state = 43;
      }
    return getToken();

  case 1:
    return makeToken(TK_EOF, lineNo, colNo);

  case 2:
    // Bỏ qua khoảng trắng: đọc cho đến khi không còn ký tự khoảng trắng
    while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
      readChar();
    }
    state = 0;
    return getToken();

  case 3: {
    // Nhận định danh: chữ cái đầu tiên đã được nhận ở state 0
    int i = 0;
    while (currentChar != EOF &&
           (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)) {
      if (i < MAX_IDENT_LEN)
        str[i++] = (char)currentChar;
      else {
        error(ERR_IDENTTOOLONG, lineNo, colNo);
        // Nếu vượt quá độ dài, bỏ qua các ký tự còn lại
      }
      readChar();
    }
    str[i] = '\0';
    // Kiểm tra xem chuỗi có là từ khóa không
    TokenType type = checkKeyword(str);
    if (type == TK_NONE)
      type = TK_IDENT;
    token = makeToken(type, ln, cn);
    strcpy(token->string, str);
    state = 0;
    return token;
  }

  case 7: {
    // Nhận số (số nguyên không dấu)
    int i = 0;
    char numStr[20];   // dự phòng đủ số chữ số
    while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
      if (i < 10)
        numStr[i++] = (char)currentChar;
      else
        error(ERR_NUMBERTOOLONG, lineNo, colNo);
      readChar();
    }
    numStr[i] = '\0';
    token = makeToken(TK_NUMBER, ln, cn);
    strcpy(token->string, numStr);
    token->value = atoi(numStr);
    state = 0;
    return token;
  }

  case 9:
    // Xử lý dấu cộng: token SB_PLUS
    readChar();
    state = 0;
    return makeToken(SB_PLUS, lineNo, colNo - 1);

  case 10:
    // Xử lý dấu trừ: token SB_MINUS
    readChar();
    state = 0;
    return makeToken(SB_MINUS, lineNo, colNo - 1);

  case 11:
    // Xử lý dấu nhân: token SB_TIMES
    readChar();
    state = 0;
    return makeToken(SB_TIMES, lineNo, colNo - 1);

  case 12:
    // Xử lý dấu chia: token SB_SLASH
    readChar();
    state = 0;
    return makeToken(SB_SLASH, lineNo, colNo - 1);

  case 13:
    // Dấu '<' có thể là SB_LT hoặc SB_LE ("<=")
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
      state = 14;
    else
      state = 15;
    return getToken();

  case 14:
    // Nhận được "<=": SB_LE
    readChar();
    state = 0;
    return makeToken(SB_LE, lineNo, colNo - 1);

  case 15:
    // Chỉ là "<": SB_LT
    state = 0;
    return makeToken(SB_LT, lineNo, colNo - 1);

  case 16:
    // Xử lý dấu '>': có thể là SB_GT hoặc SB_GE (">=")
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
      state = 18;
    else
      state = 17;
    return getToken();

  case 17:
    // Chỉ là ">": SB_GT
    state = 0;
    return makeToken(SB_GT, lineNo, colNo - 1);

  case 18:
    // ">=": SB_GE
    readChar();
    state = 0;
    return makeToken(SB_GE, lineNo, colNo - 1);

  case 19:
    // Dấu '=': token SB_EQ
    readChar();
    state = 0;
    return makeToken(SB_EQ, lineNo, colNo - 1);

  case 20:
    // Dấu '!': có thể tạo thành token SB_NEQ nếu có '=' sau đó
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
      state = 21;
    else
      state = 22;
    return getToken();

  case 21:
    // token SB_NEQ
    readChar();
    state = 0;
    return makeToken(SB_NEQ, lineNo, colNo - 1);

  case 22:
    // Lỗi: ký tự '!' không hợp lệ
    token = makeToken(TK_NONE, lineNo, colNo - 1);
    error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    state = 0;
    return token;

  case 23:
    // Dấu phẩy
    readChar();
    state = 0;
    return makeToken(SB_COMMA, lineNo, colNo - 1);

  case 24:
    // Dấu chấm
    readChar();
    state = 0;
    return makeToken(SB_PERIOD, lineNo, colNo - 1);

  case 27:
    // Dấu chấm phẩy
    readChar();
    state = 0;
    return makeToken(SB_SEMICOLON, lineNo, colNo - 1);

  case 28: {
    // Dấu ':' có thể là token SB_COLON hoặc SB_ASSIGN (":=")
    readChar();
    if (currentChar != EOF && charCodes[currentChar] == CHAR_EQ) {
      readChar();
      state = 0;
      return makeToken(SB_ASSIGN, lineNo, colNo - 1);
    } else {
      state = 0;
      return makeToken(SB_COLON, lineNo, colNo - 1);
    }
  }

  case 29:
    // (Nếu cần xử lý đặc biệt cho ký hiệu (. hoặc tương tự) – nếu không có, có thể báo lỗi)
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    state = 0;
    return getToken();

  case 30:
    // Tương tự như case 29
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    state = 0;
    return getToken();

  case 31:
    // Nhận ký tự hằng (sau dấu nháy đơn mở)
    readChar();
    if (currentChar == EOF)
      state = 34;
    else if (isprint(currentChar))
      state = 32;
    else
      state = 34;
    return getToken();

  case 32:
    c = currentChar;
    readChar();
    if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
      state = 33;
    else
      state = 34;
    return getToken();

  case 33:
    token = makeToken(TK_CHAR, lineNo, colNo - 1);
    token->string[0] = c;
    token->string[1] = '\0';
    readChar();
    state = 0;
    return token;

  case 34:
    // Lỗi ký tự hằng không hợp lệ
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo - 2);
    state = 0;
    return getToken();

  case 35: {
    // Khi gặp '(': Kiểm tra xem có phải comment hay là token SB_LPAR
    ln = lineNo;
    cn = colNo;
    readChar();
    if (currentChar == EOF)
      state = 41;
    else
      switch (charCodes[currentChar]) {
        case CHAR_PERIOD:
          state = 36; break;  // Ví dụ: "(."
        case CHAR_TIMES:
          state = 38; break;  // "(*": bắt đầu comment
        default:
          state = 41;         // chỉ là '('
      }
    return getToken();
  }

  case 36:
    // Nhận token cho "(.": SB_LSEL (theo định nghĩa trong token.h)
    readChar();
    state = 0;
    return makeToken(SB_LSEL, ln, cn);

  case 37:
    // Nếu có trường hợp đặc biệt cho ".)" (SB_RSEL)
    readChar();
    state = 0;
    return makeToken(SB_RSEL, lineNo, colNo - 1);

  case 38: {
    // Xử lý comment bắt đầu bởi "(*"
    // Đọc cho đến khi gặp chuỗi "*)"
    while (1) {
      if (currentChar == EOF) { state = 40; break; }
      if (charCodes[currentChar] == CHAR_TIMES) {
        state = 39;
        break;
      }
      readChar();
    }
    return getToken();
  }

  case 39:
    // Sau dấu '*' trong comment, kiểm tra xem có đóng comment không
    readChar();
    if (currentChar == EOF) {
      state = 40;
      return getToken();
    }
    if (charCodes[currentChar] == CHAR_RPAR) {
      readChar();
      state = 0;
      return getToken();
    } else {
      state = 38;
      return getToken();
    }

  case 40:
    // Lỗi: không kết thúc comment
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
    state = 0;
    return getToken();

  case 41:
    // Trường hợp không phải comment: '(' token
    state = 0;
    return makeToken(SB_LPAR, ln, cn);

  case 42:
    readChar();
    state = 0;
    return makeToken(SB_RPAR, lineNo, colNo);

  case 43:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    state = 0;
    return token;

  default:
    // Trường hợp không xác định: chuyển về state 0
    readChar();
    state = 0;
    return getToken();
  }
}

/******************************************************************/

void printToken(Token *token) {
  printf("%d-%d:", token->lineNo, token->colNo);
  switch (token->tokenType) {
    case TK_NONE: printf("TK_NONE\n"); break;
    case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
    case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
    case TK_CHAR: printf("TK_CHAR('%s')\n", token->string); break;
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

int main() {
  if (scan("example2.kpl") == IO_ERROR) {
    printf("Can't read input file!\n");
  }
  return 0;
}
