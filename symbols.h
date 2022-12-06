#ifndef SYMBOLS_H
#define SYMBOLS_H

#include "lexer.h"
#include "parser.h"

// define your own types and function prototypes for the symbol table(s) module below
typedef enum {sta,field,argument,var,function,method,constructor} SymbolKind;

typedef enum {classScope,subScope}Scope;

typedef struct {
    char name[128];
    char type[128];
    int staOffset;
    int fieOffset;
    int argOffset;
    int varOffset;
    Scope scope;
    SymbolKind kind;

}Symbol;

void AddSymbol(char name[],char type[],SymbolKind kind,int currentTable,int count,Scope scope);
char *GetType(int currentTable,int count);
SymbolKind GetKind(int currentTable,int count);
char* GetKind1(int currentTable,int count);
int FindSymbol(char name[],int currentTable,int time);
int Find(char name[],int currentTable,int totalTable);
int FindTable(char name[],int currentTable,int totalTable);
int GetOffset(int currentTable,int count);
void PrintTable(int m,int n);
void offset_reset();
void ClearTable();


#endif
