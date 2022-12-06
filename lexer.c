/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"




// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE
FILE* fp;
char* fileName;
int linNum = 1;

char temp[128];
char* symbols[] = { "(", ")", "[", "]", "{", "}", ",", ";", "=", ".", "+", "-", "*", "/", "&", "|", "~", "<", ">" };
char* reswords[] = { "class", "constructor", "method", "function", "int", "boolean", "char", "void", "var", "static", "field", "let", "do", "if", "else", "while", "return", "true", "false", "null", "this" };

int isLetter(char c) {

	if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z')
		return 1;
	else
		return 0;

}

int isNum(char c) {

	if (c >= '0' && c <= '9')
		return 1;
	else
		return 0;

}

int isSymbol(char c) {

	for (int i = 0; i < 19; i++) {

		if (c == *symbols[i])
			return 1;
			
	}

	return 0;

}


void clearTemp(char* temp){

    for(int i=0;i<128;i++){


            temp[i] = '\0';


    }

}


// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name)
{

	fp = fopen(file_name, "r");
	fileName = file_name;

	if (fp == NULL){
		printf("fail to open the file! \n");
		return 0;
	}

  return 1;
}


// Get the next token from the source file
Token GetNextToken ()
{
	Token t;
	t.tp = ERR;
	t.ec = -1;
	clearTemp(t.lx);
	strcpy(t.fl,fileName);
    char c;
    start:
    c = fgetc(fp);
    comment:
    while (c == ' ' || c == '\t' || c == '\r'){
        c = fgetc(fp);
    }

      if (c == '/'){                                   //Comment

          c = fgetc(fp);

        if (c == '/'){

            do{
                c = fgetc(fp);
            }while(c != '\n' && c != EOF);

            if (c == EOF){

                t.tp = ERR;
                t.ln = linNum;
                strcpy(t.lx,"Error: unexpected eof in comment");
                t.ec = EofInCom;
                return t;

        }
            if (c == '\n') {
                linNum++;
                c = fgetc(fp);
                goto comment;
            }
        }
        else if( c == '*'){

            do{
                if (c == '\n')
                    linNum++;
                c = fgetc(fp);
            }while(c != '*' && c != EOF);

            if (c == EOF){

                t.tp = ERR;
                t.ln = linNum;
                strcpy(t.lx,"Error: unexpected eof in comment");
                t.ec = EofInCom;
                return t;

            }

            Star:
            if (c == '*'){

                c = fgetc(fp);

                if (c == EOF){

                    t.tp = ERR;
                    t.ln = linNum;
                    strcpy(t.lx,"Error: unexpected eof in comment");
                    t.ec = EofInCom;
                    return t;

                }

                if (c == '/'){

                    goto start;
                }
                else {
                    do{
                        if (c == '\n')
                            linNum++;
                        c = fgetc(fp);
                    }while(c != '*' && c != EOF);
                    if (c == '*')
                        goto Star;
                    if (c == EOF){

                        t.tp = ERR;
                        t.ln = linNum;
                        strcpy(t.lx,"Error: unexpected eof in comment");
                        t.ec = EofInCom;
                        return t;

                    }
                }

            }
        }
        else{
            ungetc(c,fp);
            t.tp = SYMBOL;
            t.ln = linNum;
            t.lx[0] = '/';
            return t;
        }
      }

    if (c == '\n')    {
        linNum++;//Switch line
        c = fgetc(fp);
        goto comment;
    }


      if (isLetter(c) || c == '_'){                                 //Resword or Identifier

          int i = 0;
          while(isLetter(c) || c == '_' || isNum(c)){

              temp[i] = c;
              c = fgetc(fp);
              i++;

          }

          ungetc(c,fp);

        for(i=0; i<21; i++){
            if (strcmp(temp, reswords[i]) == 0){
                t.tp = RESWORD;
                t.ln = linNum;
                strcpy(t.lx, reswords[i]);
                clearTemp(temp);
                return t;
            }
        }

          t.tp = ID;
          t.ln = linNum;
          strcpy(t.lx, temp);
          clearTemp(temp);
          return t;
      }

	  if (c == EOF) {                               //End of File

		  t.tp = EOFile;
		  t.ln = linNum;
		  strcpy(t.lx, "End of File");
		  return t;
	  }

	  if (isNum(c)){                                //Digit
          int i = 0;
          do{
              temp[i] = c;
              c = fgetc(fp);
              i++;
          }while(isNum(c));

          ungetc(c,fp);

          t.tp = INT;
          t.ln = linNum;
          strcpy(t.lx, temp);
          clearTemp(temp);
          return t;
	  }

	  if (c == '"'){                                //String literal
          c = fgetc(fp);
	      int i = 0;
          do{
              if (i >= 128)
                  break;
              temp[i] = c;
              c = fgetc(fp);
              i++;
          }while(c != '"' && c!= EOF);
          if (c == EOF){
              t.tp = ERR;
              t.ln = linNum;
              t.ec = EofInStr;
              strcpy(t.lx,"Error: unexpected eof in string constant");
              clearTemp(temp);
              return t;
          }
          for(int i=0; i<strlen(temp); i++){
              if (temp[i] == '\n'){
                  t.tp = ERR;
                  t.ln = linNum;
                  strcpy(t.lx,"Error: new line in string constant");
                  t.ec = NewLnInStr;
                  clearTemp(temp);
                  return t;
              }
          }
          if (i < 128){
              t.tp = STRING;
              t.ln = linNum;
              strcpy(t.lx, temp);
              clearTemp(temp);
              return t;
          }
          else{
              t.tp = ERR;
              t.ln = linNum;
              strcpy(t.lx,"Error: unexpected eof in string constant");
              t.ec = EofInStr;
              clearTemp(temp);
              return t;
          }

	  }

	  if (isSymbol(c)){                                 //Symbol

          t.tp = SYMBOL;
          t.ln = linNum;
          t.lx[0] = c;
          clearTemp(temp);
          return t;

	  }
	  else if (c =='\n')
	      linNum++;
	  else{
          t.tp = ERR;
          t.ln = linNum;
          t.ec = IllSym;
          strcpy(t.lx,"Error: illegal symbol in source file");
          clearTemp(temp);
          return t;
	  }

  return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken () {
    Token t;
    t = GetNextToken();
    if (t.tp == STRING) {
        ungetc('"', fp);
        for (int i = strlen(t.lx) - 1; i >= 0; i--) {
            if (t.lx[i] == '\0')
                break;
            ungetc(t.lx[i], fp);
        }
        ungetc('"', fp);
    } else if (t.tp == EOFile) {
        ungetc(EOF, fp);
    }else if (t.tp == SYMBOL){
        ungetc(t.lx[0],fp);
    }
    else {
        for (int i = strlen(t.lx) - 1; i >= 0; i--) {
            if (t.lx[i] == '\0')
                break;
            ungetc(t.lx[i], fp);
        }
    }
        return t;
    }


// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
	linNum = 1;
	clearTemp(temp);

	fclose(fp);
	
	return 0;
}

// do not remove the next line
#ifndef TEST
//int main ()
//{
//	// implement your main function here
//  // NOTE: the autograder will not use your main function
//    InitLexer("Main.jack");
//        Token t;
//        do {
//            t = GetNextToken();
//            printf("<%s,%d,%s,%u>\n", t.fl, t.ln, t.lx, t.tp);
//        }while(!feof(fp));
//    t = PeekNextToken();
//    printf("<%s,%d,%s,%u>\n", t.fl, t.ln, t.lx, t.tp);
//    t = PeekNextToken();
//    printf("<%s,%d,%s,%u>\n", t.fl, t.ln, t.lx, t.tp);
//    for(int i = 0; i<1000;i++){
//        t = GetNextToken();
//        printf("<%s,%d,%s,%u>\n", t.fl, t.ln, t.lx, t.tp);
//        t = PeekNextToken();
//        printf("<%s,%d,%s,%u>\n", t.fl, t.ln, t.lx, t.tp);
//    }
//
//
//
//    StopLexer ();
//
//	return 0;
//}
// do not remove the next line
#endif
