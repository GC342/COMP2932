
/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
The Symbol Tables Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name:
Student ID:
Email:
Date Work Commenced:
*************************************************************************/

#include "symbols.h"
#include "string.h"
#include <stdio.h>

#define MAXSYMBOL 1000
#define MAXTABLE 1500
Symbol symbolTable [MAXTABLE][MAXSYMBOL];
int static staOffset = 0;
int static fieOffset = 0;
int static argOffset = 0;
int static varOffset = 0;


void AddSymbol(char name[],char type[],SymbolKind kind,int currentTable,int count,Scope scope){
    if (strcmp(name,"\0")!=0)
    strcpy(symbolTable[currentTable][count].name,name);
    if (strcmp(type,"\0")!=0)
    strcpy(symbolTable[currentTable][count].type,type);
    if (kind != -1)
    symbolTable[currentTable][count].kind = kind;
    symbolTable[currentTable][count].scope = scope;
    if (kind == sta){
        symbolTable[currentTable][count].staOffset = staOffset;
        staOffset++;
    }
    if (kind == field){
        symbolTable[currentTable][count].fieOffset = fieOffset;
        fieOffset++;
    }
    if (kind == argument){
        symbolTable[currentTable][count].argOffset = argOffset;
        argOffset++;
    }
    if (kind == var){
        symbolTable[currentTable][count].varOffset = varOffset;
        varOffset++;
    }
}

char *GetType(int currentTable,int count){
    return symbolTable[currentTable][count].name;
}

SymbolKind GetKind(int currentTable,int count){
    if (symbolTable[currentTable][count].kind == sta)
        return sta;
    else if (symbolTable[currentTable][count].kind == field)
        return field;
    else if (symbolTable[currentTable][count].kind == argument)
        return argument;
    else if (symbolTable[currentTable][count].kind == var)
        return var;
}

char* GetKind1(int currentTable,int count){
    if (symbolTable[currentTable][count].kind == sta)
        return "static";
    else if (symbolTable[currentTable][count].kind == field)
        return "this";
    else if (symbolTable[currentTable][count].kind == argument)
        return "argument";
    else if (symbolTable[currentTable][count].kind == var)
        return "local";
    else
        return "";

}

int GetOffset(int currentTable,int count){
    if (symbolTable[currentTable][count].kind == sta)
        return symbolTable[currentTable][count].staOffset;
    else if (symbolTable[currentTable][count].kind == field)
        return symbolTable[currentTable][count].fieOffset;
    else if (symbolTable[currentTable][count].kind == argument)
        return symbolTable[currentTable][count].argOffset;
    else if (symbolTable[currentTable][count].kind == var)
        return symbolTable[currentTable][count].varOffset;
    else
        return -1;
}

Scope GetScope(int currentTable,int count){
    return symbolTable[currentTable][count].scope;

}



int FindSymbol(char name[],int currentTable,int time){
    if (time == 0){
        for (int i = 0; i < 1000; i++) {
               if (strcmp(symbolTable[currentTable][i].name, name) == 0)
                    return 1;
            }
    }
    if (time == 1) {
        while (currentTable >= 0) {
            for (int i = 0; i < 1000; i++) {
                if (symbolTable[currentTable][i].scope == classScope) {
                    if (strcmp(symbolTable[currentTable][i].name, name) == 0)
                        return 1;
                }
            }
            currentTable--;
        }
    }


    return 0;

}

int Find(char name[],int currentTable,int totalTable){
    for (int i = 0; i < 1000; i++) {
        if (strcmp(symbolTable[currentTable][i].name, name) == 0)
            return i;
    }
    for (int i = 0;i<totalTable;i++){
        for (int j = 0;j<1000;j++){
            if (symbolTable[i][j].scope == classScope){
                if (strcmp(symbolTable[i][j].name, name) == 0)
                    return j;
            }
        }
    }
    return -1;
}

int FindTable(char name[],int currentTable,int totalTable){
    for (int i = 0; i < 1000; i++) {
        if (strcmp(symbolTable[currentTable][i].name, name) == 0)
            return currentTable;
    }
    for (int i = 0;i<totalTable;i++){
        for (int j = 0;j<1000;j++){
            if (symbolTable[i][j].scope == classScope){
                if (strcmp(symbolTable[i][j].name, name) == 0)
                    return i;
            }
        }
    }
    return -1;
}


void PrintTable(int m,int n){
    for(int i = 0;i<m;i++){
        for (int  j = 0;j<n;j++){

            printf("%s ",symbolTable[i][j].name);
        }
    }

}

void offset_reset(){
    varOffset = 0;
    fieOffset = 0;
    argOffset = 0;
    staOffset = 0;

}

void ClearTable(){
    for(int i = 0;i<1000;i++){
        for(int j = 0;j<1000;j++){
            symbolTable[i][j].argOffset=0;
            symbolTable[i][j].fieOffset=0;
            symbolTable[i][j].staOffset=0;
            symbolTable[i][j].varOffset=0;
            strcpy(symbolTable[i][j].type,"");
            strcpy(symbolTable[i][j].name,"");
        }
    }
}