/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while(currentChar != EOF && charCodes[currentChar] == CHAR_SPACE) {
    readChar();
  }
}

void skipComment() {
  int cn = colNo-2, count = 0;
  while(currentChar != EOF) {
    if(charCodes[currentChar] == CHAR_TIMES) {
      count = 1;
    } else {
      if(charCodes[currentChar] == CHAR_RPAR && count) {
        readChar();
        return;
      }
      count = 0;
    }
    readChar();
  }
  error(ERR_ENDOFCOMMENT, lineNo, cn);
}


Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_IDENT, lineNo, colNo);
  int len = 1;
  token->string[0] = (char)currentChar;
  token->string[1] = '\0';
  readChar();
  while(currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_LETTER)) {
    if(len+1 > MAX_IDENT_LEN) error(ERR_IDENTTOOLONG, lineNo, colNo-len);
    token->string[len] = (char)currentChar;
    token->string[len+1] = '\0';
    len++;
    readChar();
  }
  TokenType type = checkKeyword(token->string);
  if(type != TK_NONE) token->tokenType = type;
  return token;
}

Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int len = 1;
  token->string[0] = (char)currentChar;
  token->string[1] = '\0';
  readChar();
  while(currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
    if(len+1 > 10) error(ERR_NUMBERTOOLONG, lineNo, colNo-len);
    token->string[len] = (char)currentChar;
    token->string[len+1] = '\0';
    len++;
    readChar();
  }
  if(charCodes[currentChar] == CHAR_PERIOD) {
    token->tokenType = TK_FLOAT;;
    token->string[len] = (char)currentChar;
    token->string[len+1] = '\0';
    len++;
    readChar();
  }
  while(currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT) {
    if(len+1 > 10) error(ERR_NUMBERTOOLONG, lineNo, colNo-len);
    token->string[len] = (char)currentChar;
    token->string[len+1] = '\0';
    len++;
    readChar();
  }

  if(token->tokenType == TK_NUMBER) token->value = atoi(token->string);
  else token->value = atof(token->string);
  return token;
}

Token* readConstChar(void) {
  int cn = colNo-1;
  Token *token = makeToken(TK_CHAR, lineNo, cn);
  if(currentChar != EOF && charCodes[currentChar] != CHAR_SINGLEQUOTE) {
    readChar();
    if(currentChar != EOF && charCodes[currentChar] == CHAR_SINGLEQUOTE) {
      token->string[0] = (char)currentChar;
      token->string[1] = '\0';
      return token;
    }
  }
  error(ERR_INVALIDCHARCONSTANT, lineNo, cn);
  return NULL;
}

Token* createOneCharacterToken(TokenType type) {
  int cn = colNo, ln = lineNo;
  readChar();
  return makeToken(type, ln, cn);
}


Token* getToken(void) {
  int cn = colNo, ln = lineNo;

  if(currentChar == EOF) {
    Token *token = makeToken(TK_EOF, lineNo, colNo);
    return token;
  }

  switch(charCodes[currentChar]) {
    case CHAR_SPACE: skipBlank(); return getToken();
    case CHAR_LETTER: return readIdentKeyword();
    case CHAR_DIGIT: return readNumber();
    case CHAR_EQ: return createOneCharacterToken(SB_EQ);
    case CHAR_SEMICOLON: return createOneCharacterToken(SB_SEMICOLON);
    case CHAR_COMMA: return createOneCharacterToken(SB_COMMA);
    case CHAR_PERIOD: return createOneCharacterToken(SB_PERIOD);
    case CHAR_RPAR: return createOneCharacterToken(SB_RPAR);
    case CHAR_MOD: return createOneCharacterToken(SB_MOD);
    case CHAR_GT:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_GE, ln, cn);
      }
      return makeToken(SB_GT, ln, cn);
    case CHAR_LT:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_LE, ln, cn);
      }
      return makeToken(SB_LT, ln, cn);
    case CHAR_EXCLAIMATION:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_NEQ, ln, cn);
      }
      error(ERR_INVALIDSYMBOL, ln, cn);
    case CHAR_COLON:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_ASSIGN, ln, cn);
      }
      return makeToken(SB_COLON, ln, cn);
    case CHAR_PLUS:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_ASSIGN_PLUS, ln, cn);
      }
      return makeToken(SB_PLUS, ln, cn);
    case CHAR_MINUS:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_ASSIGN_SUBTRACT, ln, cn);
      }
      return makeToken(SB_MINUS, ln, cn);
    case CHAR_TIMES:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_ASSIGN_TIMES, ln, cn);
      }
      return makeToken(SB_TIMES, ln, cn);
    case CHAR_SLASH:
      readChar();
      if(charCodes[currentChar] == CHAR_EQ) {
        readChar();
        return makeToken(SB_ASSIGN_DIVIDE, ln, cn);
      }
      return makeToken(SB_SLASH, ln, cn);
    case CHAR_SINGLEQUOTE:
      readChar();
      return readConstChar();
    case CHAR_LBRACKET:
      readChar();
      return makeToken(SB_LBRACKET, ln, cn);
    case CHAR_RBRACKET:
      readChar();
      return makeToken(SB_RBRACKET, ln, cn);
    case CHAR_LPAR:
      readChar();
      if(charCodes[currentChar] == CHAR_TIMES) {
        readChar();
        skipComment();
        return getToken();
      } else return makeToken(SB_LPAR, ln, cn);
    default:
      error(ERR_INVALIDSYMBOL, ln, cn);
      return NULL;
  }

}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_FLOAT: printf("TK_FLOAT(%s)\n", token->string); break;
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
  case KW_FLOAT: printf("KW_FLOAT\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_ASSIGN_PLUS: printf("SB_ASSIGN_PLUS\n"); break;
  case SB_ASSIGN_SUBTRACT: printf("SB_ASSIGN_SUBTRACT\n"); break;
  case SB_ASSIGN_TIMES: printf("SB_ASSIGN_TIMES\n"); break;
  case SB_ASSIGN_DIVIDE: printf("SB_ASSIGN_DIVIDE\n"); break;
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
  case SB_MOD: printf("SB_MOD\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LBRACKET: printf("SB_LBRACKET\n"); break;
  case SB_RBRACKET: printf("SB_RBRACKET\n"); break;
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

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}



