/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Compiler Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.


Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/

#include "compiler.h"
#include "symbols.h"
#include "parser.h"
#include "lexer.h"
#include "io.h"
#include <stdio.h>
#include <string.h>

char fileList[100][128];


int InitCompiler ()
{
    ParserInfo p;
    InitParser("Array.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[0],"Array.jack");
    InitParser("Keyboard.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[1],"Keyboard.jack");
    InitParser("Math.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[2],"Math.jack");
    InitParser("Memory.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[3],"Memory.jack");
    InitParser("Screen.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[4],"Screen.jack");
    InitParser("String.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[5],"String.jack");
    InitParser("Sys.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[6],"Sys.jack");
    InitParser("Output.jack");
    p = Parse();
    if (p.er != none)
        return 0;
    StopParser();
    strcpy(fileList[7],"Output.jack");



    return 1;
}

ParserInfo compile (char* dir_name)
{
	ParserInfo p;
	ParserInfo p1;
    p.er = none;
    p1.er = none;

	// write your code below

//	DIR *pDir;
//	struct dirent* ptr;

	struct _finddata_t fileInfo;
	char _toSearch[128];
	long fHandle;
	int filecount = 8;

	strcpy(_toSearch,"./");
    strcat(_toSearch,dir_name);
    strcat(_toSearch,"/*.jack");

    fHandle = _findfirst(_toSearch, &fileInfo);

//    if (!(pDir = opendir(_toSearch)))
//        return p;
//
//    while((ptr = readdir(pDir))!=0){
//        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0){
//            char FileName[128];
//            strcpy(FileName,"\0\0");
//            strcat(FileName,"./");
//            strcat(FileName,dir_name);
//            strcat(FileName,"/");
//            strcat(FileName,ptr->d_name);
//            InitParser(FileName);
//            p = Parse();
//            if (p.er != none)
//                return p;
//            StopParser();
//            PrintTable(100,30);
//            InitParser(FileName);
//            p = Parse();
//            if (p.er != none)
//                return p;
//            StopParser();
//        }
//    }

//    closedir(pDir);

	if (fHandle == -1L){
	    return p;
	}
	else{
	    do{
	        char FileName[128];
	        strcpy(FileName,"\0\0");
            strcat(FileName,".\\");
	        strcat(FileName,dir_name);
	        strcat(FileName,"\\");
            strcat(FileName,fileInfo.name);
            strcpy(fileList[filecount],FileName);
            filecount++;
	        InitParser(FileName);
	        p = Parse();
            StopParser();
//	        if (p.er != none)
//	            p1.er = p.er;
	    }while(_findnext(fHandle,&fileInfo) == 0);
	}
	for (int i = 0;i<filecount;i++){
        InitParser(fileList[i]);
        p = Parse();
        StopParser();
        if (p.er != none)
//            if (p1.er != none)
//                return p1;
//            else
                return p;
//        }

	}
	return p;
}

int StopCompiler ()
{
    for(int i = 0; i<100; i++){
        strcpy(fileList[i],"");
    }
    ClearTable();
	return 1;
}


#ifndef TEST_COMPILER
int main ()
{
	InitCompiler ();
	ParserInfo p = compile ("Pong");
	PrintTable(100,30);
	printf("%u %s %d %s",p.er,p.tk.lx,p.tk.ln,p.tk.fl);
	StopCompiler ();
	return 1;
}
#endif
