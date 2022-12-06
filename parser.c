#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "symbols.h"
#include "VMWriter.h"

FILE *fp,*VMOut;
int count = 0;
int static currentTable = 0;
int static time = 0;
int static result[1000] = {0};
int resultCount = 0;
int classTable;
char className[128];
int cla_var_count = 1;
static char scannedFile[100][128];
static char classList[100][128];
int static fileCount = 0;
int static classcount = 0;
static int stopcount = 0;
int static totalTable = 0;
int fieldCount = 0;
int whileNum = 0;
int ifNum = 0;
ParserInfo static p1;
// you can declare prototypes of parser functions below

ParserInfo type();
ParserInfo classVarDeclar();
ParserInfo paramList();
ParserInfo class();
ParserInfo memberDeclar();
ParserInfo subroutineDeclar();
ParserInfo subroutineBody(ParserInfo temp,ParserInfo name);
ParserInfo statement();
ParserInfo varDeclarStatement();
ParserInfo letStatement();
ParserInfo ifStatement();
ParserInfo whileStatement();
ParserInfo doStatement(); //more work need to be done
ParserInfo subroutineCall();
ParserInfo expressionList(int *argCount);
ParserInfo returnStatement();
ParserInfo expression();
ParserInfo relationalExpression();
ParserInfo ArithmeticExpression();
ParserInfo term();
ParserInfo factor();
ParserInfo operand();


int LexEr(Token token){
    if (token.ec == EofInCom || token.ec == EofInStr || token.ec == IllSym || token.ec == NewLnInStr)
        return 1;
    else return 0;
}

ParserInfo class(){
    ParserInfo pi;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    pi.er = none;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "class") == 0)
        ;
    else{
        pi.er = classExpected;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (next_token.tp == ID) {
        strcpy(className,pi.tk.lx);
        if (time == 0) {
            strcpy(scannedFile[fileCount], strcat(className,".jack"));
            fileCount++;
            strcpy(className,pi.tk.lx);
            strcpy(classList[classcount],className);
            classcount++;
            classTable = currentTable;
            AddSymbol(className,"\0",-1,currentTable,count,classScope);
            count++;
        }
    }
    else{
        pi.er = idExpected;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "{") == 0) {
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)) {
            pi.er = lexerErr;
            return pi;
        }
        while (strcmp(next_token.lx, "static") == 0 || strcmp(next_token.lx, "field") == 0 ||
               strcmp(next_token.lx, "constructor") == 0 || strcmp(next_token.lx, "function") == 0 ||
               strcmp(next_token.lx, "method") == 0) {
            pi = memberDeclar();
            if (pi.er != none)
                return pi;
            next_token = PeekNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)) {
                pi.er = lexerErr;
                return pi;
            }
        }
    }
    else {
        pi.er = openBraceExpected;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "}") == 0)
        ;
    else{
        pi.er =closeBraceExpected;
        return pi;
    }
    return pi;
}

ParserInfo type(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "int") == 0){
        pi.er = none;
        return pi;
    }
    else if (strcmp(next_token.lx, "char") == 0){
        pi.er = none;
        return pi;
    }
    else if (strcmp(next_token.lx, "boolean") == 0){
        pi.er = none;
        return pi;
    }
    else if (next_token.tp == ID){
        if (time == 1) {
            for(int i =0;;i++){
                if (strcmp(pi.tk.lx,classList[i]) == 0)
                    break;
                if (i >= 100){
                    pi.er = undecIdentifier;
                    return pi;
                }
            }
        }
    }
    else {
        pi.er = illegalType;
        return pi;
    }
    return pi;

}

ParserInfo classVarDeclar(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"static") == 0) {
        if (time == 0)
        AddSymbol("\0", "\0", sta, classTable, count, classScope);
    }
    else if (strcmp(next_token.lx,"field") == 0) {
        if (time == 0)
        AddSymbol("\0", "\0", field, classTable, count, classScope);
    }
    else{
        pi.er = classVarErr;
        return pi;
    }
    pi = type();
    if (time == 0)
    AddSymbol("\0",pi.tk.lx,-1,classTable,count,classScope);
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (next_token.tp == ID) {
        fieldCount++;
        if (time == 0) {
            if (!FindSymbol(pi.tk.lx, classTable,time)) {
                AddSymbol(pi.tk.lx, "\0", -1, classTable, count, classScope);
                count++;
                cla_var_count++;
            } else {
                pi.er = redecIdentifier;
                if (p1.er == none)
                    p1 = pi;
            }
        }
        if (time == 1){
            if (p1.er != none)
                return p1;
        }
    }
    else{
        pi.er = idExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    while(strcmp(next_token.lx,",") == 0){
        GetNextToken();
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (next_token.tp == ID) {
            fieldCount++;
            if (time == 0) {
                if (!FindSymbol(pi.tk.lx, currentTable,time)) {
                    AddSymbol(pi.tk.lx, GetType(currentTable, count - 1), GetKind(currentTable, count - 1), classTable, count, classScope);
                    count++;
                    cla_var_count++;
                } else {
                    pi.er = redecIdentifier;
                    if (p1.er == none)
                        p1 = pi;
                }
            }
            if (time == 1){
                if (p1.er != none)
                    return p1;
            }
        }

        else{
            pi.er = idExpected;
        }
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,";") == 0){
        pi.er = none;
        return pi;
    }
    else{
        pi.er = semicolonExpected;
        return pi;
    }
}

ParserInfo paramList(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "int") == 0||strcmp(next_token.lx, "boolean") == 0||strcmp(next_token.lx, "char") == 0||next_token.tp == ID){
        pi = type();
        if (time == 0)
        AddSymbol("\0",pi.tk.lx,-1,currentTable,count,subScope);
        if (pi.er != none)
            return pi;
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (next_token.tp == ID) {
            if (time == 0) {
                AddSymbol(pi.tk.lx, "\0", argument, currentTable, count, subScope);
                count++;
            }
        }
        else{
            pi.er = idExpected;
            return pi;
        }
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        while(strcmp(next_token.lx,",") == 0){
            GetNextToken();
            pi = type();
            if (pi.er != none)
                return pi;
            next_token = GetNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)){
                pi.er = lexerErr;
                return pi;
            }
            if (next_token.tp == ID) {
                if (time == 0) {
                    AddSymbol(pi.tk.lx, GetType(currentTable, count - 1), argument, currentTable, count, subScope);
                    count++;
                }
            }
            else{
                pi.er = idExpected;
            }
            next_token = PeekNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)){
                pi.er = lexerErr;
                return pi;
            }
        }
    }
    return pi;

}

ParserInfo varDeclarStatement(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"var") == 0)
        if (time == 0)
            AddSymbol("\0","\0",var,currentTable,count,subScope);
    else{
        pi.er = syntaxError;
    }
    pi = type();
    if (time == 0)
    AddSymbol("\0",pi.tk.lx,-1,currentTable,count,subScope);
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (next_token.tp == ID) {
        if (time == 0) {
            if (!FindSymbol(pi.tk.lx, currentTable,time)) {
                AddSymbol(pi.tk.lx, "\0", -1, currentTable, count, subScope);
                count++;
            } else {
                pi.er = redecIdentifier;
                if (p1.er == none)
                    p1 = pi;
            }
        }
        if (time == 1){
            if (p1.er != none)
                return p1;
        }
    }
    else{
        pi.er = idExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    while(strcmp(next_token.lx,",") == 0){
        GetNextToken();
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (next_token.tp == ID) {
            if (time == 0) {
                if (!FindSymbol(pi.tk.lx, currentTable,time)) {
                    AddSymbol(pi.tk.lx, GetType(currentTable, count - 1), var, currentTable, count, subScope);
                    count++;
                } else {
                    pi.er = redecIdentifier;
                    if (p1.er == none)
                        p1 = pi;
                }
            }
            if (time == 1){
                if (p1.er != none)
                    return p1;
            }
        }
        else{
            pi.er = idExpected;
        }
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,";") == 0){
        pi.er = none;
        return pi;
    }
    else{
        pi.er = semicolonExpected;
        return pi;
    }

}

ParserInfo statement(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "var") == 0) {
        pi = varDeclarStatement();
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx, "let") == 0) {
        pi = letStatement();
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx, "if") == 0){
        pi = ifStatement();
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx, "while") == 0){
        pi = whileStatement();
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx, "do") == 0) {
        pi = doStatement();
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx, "return") == 0){
        pi = returnStatement();
        if (pi.er != none)
            return pi;
    }
    else {
        pi.er = syntaxError;
        return pi;
    }
    return pi;
}

ParserInfo subroutineBody(ParserInfo temp,ParserInfo name){
    ParserInfo pi;
    int checkFlag = 1;
    int checkFlag1 = 0;
    int varCount = 0;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"{") == 0)
        ;
    else{
        pi.er = openBraceExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    while(strcmp(next_token.lx, "var") == 0||strcmp(next_token.lx, "let") == 0||strcmp(next_token.lx, "if") == 0||strcmp(next_token.lx, "while") == 0||strcmp(next_token.lx, "do") == 0||strcmp(next_token.lx, "return") == 0){
        if (strcmp(next_token.lx, "var") == 0){
            varCount++;
        }
        else {
            checkFlag = 0;
        }
        if (checkFlag == 0 && checkFlag1 == 0){
            if (time == 1){
                WriteFunction(VMOut,name.tk.lx,varCount);
                if (strcmp(temp.tk.lx, "constructor") == 0){
                    WritePush(VMOut,"constant",fieldCount);
                    WriteCall(VMOut,"Memory.alloc",1);
                    WritePop(VMOut,"pointer",0);
                }
                if (strcmp(temp.tk.lx, "method") == 0){
                    WritePush(VMOut,"argument",0);
                    WritePop(VMOut,"pointer",0);
                }
            }
            checkFlag1 = 1;
        }
        pi = statement();
        if (pi.er != none)
            return pi;
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"}") == 0)
        ;
    else{
        pi.er = closeBraceExpected;
        return pi;
    }
    return pi;
}

ParserInfo subroutineDeclar(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    ParserInfo temp,name;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    temp = pi;
    if (strcmp(next_token.lx,"constructor") == 0) {
        if (time == 0) {
            currentTable++;
            count = 0;
            AddSymbol("\0", "\0", constructor, classTable, cla_var_count, classScope);
            offset_reset();
        }
        else {
            currentTable++;
        }
    }
    else if (strcmp(next_token.lx,"function") == 0){
        if (time == 0) {
            currentTable++;
            count = 0;
            AddSymbol("\0", "\0", function, classTable, cla_var_count, classScope);
            offset_reset();
        }
        else {
            currentTable++;
        }
    }
    else if (strcmp(next_token.lx,"method") == 0){
        if (time == 0) {
            currentTable++;
            count = 0;
            AddSymbol("\0", "\0", method, classTable, cla_var_count, classScope);
            offset_reset();
            AddSymbol("this", className, argument, currentTable, count, subScope);
            count++;
        }
        else {
            currentTable++;
        }
    }
    else {
        pi.er = subroutineDeclarErr;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "int") == 0||strcmp(next_token.lx, "boolean") == 0||strcmp(next_token.lx, "char") == 0||next_token.tp == ID){
        pi = type();
        if (time == 0)
        AddSymbol("\0",pi.tk.lx,-1,classTable,cla_var_count,classScope);
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx,"void") == 0)
        GetNextToken();
    else{
        pi.er = syntaxError;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    strcpy(name.tk.lx,className);
    strcat(name.tk.lx,".");
    strcat(name.tk.lx,pi.tk.lx);
    if (next_token.tp == ID){
        if (time == 0) {
            AddSymbol(pi.tk.lx, "\0", -1, classTable, cla_var_count, classScope);
            cla_var_count++;
        }
    }
    else{
        pi.er = idExpected;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"(") == 0)
        ;
    else{
        pi.er = openParenExpected;
        return pi;
    }
    pi = paramList();
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,")") == 0)
        ;
    else{
        pi.er = closeParenExpected;
        return pi;
    }
    pi = subroutineBody(temp,name);
    if (pi.er != none)
        return pi;
    return pi;
}

ParserInfo memberDeclar(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"static") == 0||strcmp(next_token.lx,"field") == 0) {
        pi = classVarDeclar();
        if (pi.er != none)
            return pi;
    }
    else if (strcmp(next_token.lx,"constructor") == 0||strcmp(next_token.lx,"function") == 0||strcmp(next_token.lx,"method") == 0) {
        pi = subroutineDeclar();
        if (pi.er != none)
            return pi;
    }
    else{
        pi.er = memberDeclarErr;
        return pi;
    }
    return pi;
}

ParserInfo letStatement() {
    ParserInfo pi, temp;
    int checkflag = 0;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "let") == 0);
    else {
        pi.er = syntaxError;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    temp.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (next_token.tp == ID) {
        if (time == 0) {
            result[resultCount] = FindSymbol(pi.tk.lx, currentTable, time);
            resultCount++;
        }
        if (time == 1) {
            if (result[resultCount] == 0) {
                if (!FindSymbol(pi.tk.lx, totalTable, time)) {
                    pi.er = undecIdentifier;
                    if (p1.er != none)
                        return p1;
                    else
                        return pi;
                }
            }
            resultCount++;
        }
    } else {
        pi.er = idExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "[") == 0) {
        if (time == 1){
            WritePush(VMOut, GetKind1(FindTable(temp.tk.lx, currentTable,totalTable), Find(temp.tk.lx, currentTable,totalTable)),
                      GetOffset(FindTable(temp.tk.lx, currentTable,totalTable), Find(temp.tk.lx, currentTable,totalTable)));
        }
        checkflag = 1;
        GetNextToken();
        pi = expression();
        if (pi.er != none)
            return pi;
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)) {
            pi.er = lexerErr;
            return pi;
        }
        if (strcmp(next_token.lx, "]") == 0);
        else {
            pi.er = closeBracketExpected;
            return pi;
        }
    }
    if (time == 1) {
        if (checkflag == 1)
            WriteArithmetic(VMOut, "add");

    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "=") == 0);
    else {
        pi.er = equalExpected;
        return pi;
    }
    pi = expression();
    if (pi.er != none)
        return pi;
    if (time == 1) {
        if (checkflag == 1) {
            WritePop(VMOut, "temp", 0);
            WritePop(VMOut, "pointer", 1);
            WritePush(VMOut, "temp", 0);
            WritePop(VMOut,"that",0);
        }
        else{
            WritePop(VMOut, GetKind1(FindTable(temp.tk.lx,currentTable,totalTable), Find(temp.tk.lx,currentTable,totalTable)),
                     GetOffset(FindTable(temp.tk.lx,currentTable,totalTable),Find(temp.tk.lx,currentTable,totalTable)));
        }
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,";") == 0)
        ;
    else{
        pi.er = semicolonExpected;
        return pi;
    }
    return pi;
}

ParserInfo ifStatement(){
    ParserInfo pi;
    int ifNum1;
    char ifLable[20];
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"if") == 0)
        ;
    else {
        pi.er = syntaxError;
        return pi;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"(") == 0)
        ;
    else {
        pi.er = openParenExpected;
        return pi;
    }
    pi = expression();
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,")") == 0)
        ;
    else {
        pi.er = closeParenExpected;
        return pi;
    }
    if (time == 1){
        sprintf(ifLable,"IF_TURE%d",ifNum);
        WriteIf(VMOut,ifLable);
        sprintf(ifLable,"IF_FALSE%d",ifNum);
        WriteGoto(VMOut,ifLable);
        ifNum1 = ifNum++;
        strcpy(ifLable,"");
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"{") == 0)
        ;
    else {
        pi.er = openBraceExpected;
        return pi;
    }
    if (time == 1){
        sprintf(ifLable,"IF_TURE%d",ifNum1);
        WriteLabel(VMOut,ifLable);
        strcpy(ifLable,"");
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    while(strcmp(next_token.lx, "var") == 0||strcmp(next_token.lx, "let") == 0||strcmp(next_token.lx, "if") == 0||strcmp(next_token.lx, "while") == 0||strcmp(next_token.lx, "do") == 0||strcmp(next_token.lx, "return") == 0){
        pi = statement();
        if (pi.er != none)
            return pi;
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
    }
    if (time == 1){
        sprintf(ifLable,"IF_END%d",ifNum1);
        WriteGoto(VMOut,ifLable);
        sprintf(ifLable,"IF_FALSE%d",ifNum1);
        WriteLabel(VMOut,ifLable);
        strcpy(ifLable,"");
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"}") == 0)
        ;
    else{
        pi.er = closeBraceExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"else") == 0){
        GetNextToken();
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (strcmp(next_token.lx,"{") == 0)
            ;
        else {
            pi.er = openBraceExpected;
            return pi;
        }
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        while(strcmp(next_token.lx, "var") == 0||strcmp(next_token.lx, "let") == 0||strcmp(next_token.lx, "if") == 0||strcmp(next_token.lx, "while") == 0||strcmp(next_token.lx, "do") == 0||strcmp(next_token.lx, "return") == 0){
            pi = statement();
            if (pi.er != none)
                return pi;
            next_token = PeekNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)){
                pi.er = lexerErr;
                return pi;
            }
        }
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (strcmp(next_token.lx,"}") == 0)
            ;
        else{
            pi.er = closeBraceExpected;
            return pi;
        }
    }
    if (time == 1){
        sprintf(ifLable,"IF_END%d",ifNum1);
        WriteLabel(VMOut,ifLable);
    }
    return pi;
}

ParserInfo whileStatement() {
    ParserInfo pi;
    char whileLable[20];
    int whileNum1;
    sprintf(whileLable,"WHILE_EXP%d",whileNum);
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "while") == 0);
    else {
        pi.er = syntaxError;
        return pi;
    }
    if (time == 1){
        WriteLabel(VMOut,whileLable);
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "(") == 0);
    else {
        pi.er = openParenExpected;
        return pi;
    }
    pi = expression();
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, ")") == 0);
    else {
        pi.er = closeParenExpected;
        return pi;
    }
    if (time == 1){
        WriteArithmetic(VMOut,"not");
        sprintf(whileLable,"WHILE_END%d",whileNum);
        WriteIf(VMOut,whileLable);
        whileNum1 = whileNum + 1;
        whileNum++;
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "{") == 0);
    else {
        pi.er = openBraceExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    while (strcmp(next_token.lx, "var") == 0 || strcmp(next_token.lx, "let") == 0 || strcmp(next_token.lx, "if") == 0 ||
           strcmp(next_token.lx, "while") == 0 || strcmp(next_token.lx, "do") == 0 ||
           strcmp(next_token.lx, "return") == 0) {
        pi = statement();
        if (pi.er != none)
            return pi;
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)) {
            pi.er = lexerErr;
            return pi;
        }
    }
    if (time == 1){
        sprintf(whileLable,"WHILE_EXP%d",whileNum1);
        WriteGoto(VMOut,whileLable);
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "}") == 0);
    else {
        pi.er = closeBraceExpected;
        return pi;
    }
    if (time == 1){
        sprintf(whileLable,"WHILE_END%d",whileNum1);
        WriteLabel(VMOut,whileLable);
    }
    return pi;
}

ParserInfo doStatement(){
    ParserInfo pi;
    pi.er = none;
    Token next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "do") == 0);
    else {
        pi.er = syntaxError;
        return pi;
    }
    pi = subroutineCall();
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,";") == 0)
        ;
    else{
        pi.er = semicolonExpected;
        return pi;
    }
    return pi;
}

ParserInfo subroutineCall() {
    ParserInfo pi;
    ParserInfo temp,temp1;
    strcpy(temp1.tk.lx,className);
    int *argCount_add;
    int argCount = 0;
    int checkFlag = 0;
    argCount_add = &argCount;
    pi.er = none;
    Token next_token;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    temp = pi;
    if (next_token.tp == ID) {
        if (time == 0){
            result[resultCount] = FindSymbol(pi.tk.lx, currentTable,time);
            resultCount++;
        }
        if (time == 1) {
            if (result[resultCount] == 0) {
                if (!FindSymbol(pi.tk.lx, totalTable, time)) {
                    pi.er = undecIdentifier;
                    if (p1.er != none)
                        return p1;
                    else
                        return pi;
                }
            }
            resultCount++;
        }
    }
    else {
        pi.er = idExpected;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, ".") == 0) {

        checkFlag = 1;
        if (time == 1){
            if (strcmp(temp.tk.lx,"Array") != 0 && strcmp(temp.tk.lx,"Screen") != 0 && strcmp(temp.tk.lx,"Output") != 0 && strcmp(temp.tk.lx,"Keyboard") != 0 &&
                    strcmp(temp.tk.lx,"Math") != 0 && strcmp(temp.tk.lx,"Memory") != 0 && strcmp(temp.tk.lx,"String") != 0 && strcmp(temp.tk.lx,"Sys") != 0)
            WritePush(VMOut, GetKind1(FindTable(temp.tk.lx,currentTable,totalTable), Find(temp.tk.lx,currentTable,totalTable)),
                      GetOffset(FindTable(temp.tk.lx,currentTable,totalTable),Find(temp.tk.lx,currentTable,totalTable)));
        }
        strcat(temp.tk.lx,pi.tk.lx);
        GetNextToken();
        next_token = GetNextToken();
        pi.tk = next_token;
        if (time == 1) {
            if (GetKind(FindTable(pi.tk.lx, currentTable, totalTable), Find(pi.tk.lx, currentTable, totalTable)) ==
                method) {
                argCount = 1;
            }
        }
        if (LexEr(next_token)) {
            pi.er = lexerErr;
            return pi;
        }
        if (next_token.tp == ID){
            strcat(temp.tk.lx,pi.tk.lx);
            if (time == 0){
                result[resultCount] = FindSymbol(pi.tk.lx, currentTable,time);
                resultCount++;
            }
            if (time == 1) {
                if (result[resultCount] == 0) {
                    if (!FindSymbol(pi.tk.lx, totalTable, time)) {
                        pi.er = undecIdentifier;
                        if (p1.er != none)
                            return p1;
                        else
                            return pi;
                    }
                }
                resultCount++;
            }
        }
        else {
            pi.er = idExpected;
        }
    }
    else {
        if (time == 1){
            if (GetKind(FindTable(temp.tk.lx,currentTable,totalTable), Find(temp.tk.lx,currentTable,totalTable)) == method)
                argCount = 1;
            strcat(temp1.tk.lx,".");
            strcat(temp1.tk.lx,temp.tk.lx);
            WritePush(VMOut,"pointer",0);
        }
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,"(") == 0)
        ;
    else{
        pi.er = openParenExpected;
        return pi;
    }
    pi = expressionList(argCount_add);
    if (pi.er != none)
        return pi;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,")") == 0)
        ;
    else{
        pi.er = closeParenExpected;
        return pi;
    }
    if (time == 1){
        if (checkFlag) {
            WriteCall(VMOut, temp.tk.lx, argCount);
        }
        else {
            WriteCall(VMOut, temp1.tk.lx, argCount);
        }
        WritePop(VMOut, "temp", 0);
    }
    return pi;
}

ParserInfo expressionList(int *argCount){
    ParserInfo pi;
    pi.er = none;
    Token next_token;
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "*") == 0 || strcmp(next_token.lx, "/") == 0 || strcmp(next_token.lx, "-") == 0 ||
           strcmp(next_token.lx, "~") == 0 ||  strcmp(next_token.lx, "(") == 0 ||strcmp(next_token.lx, "true") == 0
           || strcmp(next_token.lx, "false") == 0 || strcmp(next_token.lx, "null") == 0 || strcmp(next_token.lx, "this") == 0
           || next_token.tp == ID || next_token.tp == INT || next_token.tp == STRING ){
        *argCount = *argCount + 1;
        pi = expression();
        if (pi.er != none)
            return pi;
        next_token = PeekNextToken();
        while(strcmp(next_token.lx, ",") == 0){
            *argCount = *argCount + 1;
            GetNextToken();
            pi = expression();
            if (pi.er != none)
                return pi;
            next_token = PeekNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)) {
                pi.er = lexerErr;
                return pi;
            }
        }
    }
    return pi;
}

ParserInfo returnStatement(){
    ParserInfo pi;
    pi.er = none;
    Token next_token;
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)) {
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx, "return") == 0);
    else {
        pi.er = syntaxError;
        return pi;
    }
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (strcmp(next_token.lx, "*") == 0 || strcmp(next_token.lx, "/") == 0 || strcmp(next_token.lx, "-") == 0 ||
        strcmp(next_token.lx, "~") == 0 ||  strcmp(next_token.lx, "(") == 0 ||strcmp(next_token.lx, "true") == 0
        || strcmp(next_token.lx, "false") == 0 || strcmp(next_token.lx, "null") == 0 || strcmp(next_token.lx, "this") == 0
        || next_token.tp == ID || next_token.tp == INT || next_token.tp == STRING ) {
        pi = expression();
        if (pi.er != none)
            return pi;
    }
    else{
        if (time == 1){
            WritePush(VMOut,"constant",0);
        }
    }
    if (time == 1){
        WriteReturn(VMOut);
    }
    next_token = GetNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    if (strcmp(next_token.lx,";") == 0)
        ;
    else{
        pi.er = semicolonExpected;
        return pi;
    }
    return pi;
}

ParserInfo expression(){
    ParserInfo pi;
    char operator[2];
    pi.er = none;
    Token next_token;
    pi = relationalExpression();
    if (pi.er != none)
        return pi;
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    strcpy(operator,pi.tk.lx);
    if (strcmp(next_token.lx, "&") == 0 ||strcmp(next_token.lx, "|") == 0){
        GetNextToken();
        pi = relationalExpression();
        if (pi.er != none)
            return pi;
    }
    if (time == 1){
        if (strcmp(operator, "&") == 0 ){
            WriteArithmetic(VMOut,"and");
        }
        if (strcmp(operator, "|") == 0 ){
            WriteArithmetic(VMOut,"or");
        }
    }
    return pi;
}

ParserInfo relationalExpression(){
    ParserInfo pi;
    char operator[2];
    pi.er = none;
    Token next_token;
    pi = ArithmeticExpression();
    if (pi.er != none)
        return pi;
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    strcpy(operator,pi.tk.lx);
    if (strcmp(next_token.lx, "=") == 0 ||strcmp(next_token.lx, ">") == 0||strcmp(next_token.lx, "<") == 0){
        GetNextToken();
        pi = ArithmeticExpression();
        if (pi.er != none)
            return pi;
    }
    if (time == 1){
        if (strcmp(operator, "=") == 0 ){
            WriteArithmetic(VMOut,"eq");
        }
        if (strcmp(operator, ">") == 0 ){
            WriteArithmetic(VMOut,"gt");
        }
        if (strcmp(operator, "<") == 0 ){
            WriteArithmetic(VMOut,"lt");
        }
    }
    return pi;
}

ParserInfo ArithmeticExpression(){
    ParserInfo pi;
    char operator[2];
    pi.er = none;
    Token next_token;
    pi = term();
    if (pi.er != none)
        return pi;
    next_token = PeekNextToken();
    pi.tk = next_token;
    strcpy(operator,pi.tk.lx);
    if (strcmp(next_token.lx, "+") == 0 ||strcmp(next_token.lx, "-") == 0){
        GetNextToken();
        pi = term();
        if (pi.er != none)
            return pi;
    }
    if (time == 1){
        if (strcmp(operator, "+") == 0 ){
            WriteArithmetic(VMOut,"add");
        }
        if (strcmp(operator, "-") == 0 ){
            WriteArithmetic(VMOut,"sub");
        }
    }
    return pi;
}

ParserInfo term(){
    ParserInfo pi;
    char operator[2];
    pi.er = none;
    Token next_token;
    pi = factor();
    if (pi.er != none)
        return pi;
    next_token = PeekNextToken();
    pi.tk = next_token;
    strcpy(operator,pi.tk.lx);
    if (strcmp(next_token.lx, "*") == 0 ||strcmp(next_token.lx, "/") == 0){
        GetNextToken();
        pi = factor();
        if (pi.er != none)
            return pi;
    }
    if (time == 1){
        if (strcmp(operator, "*") == 0 ){
            WriteCall(VMOut,"Math.multiply",2);
        }
        if (strcmp(operator, "/") == 0 ){
            WriteCall(VMOut,"Math.divide",2);
        }
    }
    return pi;
}

ParserInfo factor(){
    ParserInfo pi;
    char operator[2];
    pi.er = none;
    Token next_token;
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    strcpy(operator,pi.tk.lx);
    if (strcmp(next_token.lx, "-") == 0)
        GetNextToken();
    else if (strcmp(next_token.lx, "~") == 0)
        GetNextToken();
    pi = operand();
    if (pi.er != none)
        return pi;
    if (time == 1){
        if (strcmp(operator, "~") == 0 ){
            WriteArithmetic(VMOut,"not");
        }
        if (strcmp(operator, "-") == 0 ){
            WriteArithmetic(VMOut,"neg");
        }
    }
    return pi;
}

ParserInfo operand(){
    ParserInfo pi;
    ParserInfo temp;
    int argCount = 0;
    int *argCount_add = &argCount;
    char number[128];
    pi.er = none;
    Token next_token;
    next_token = PeekNextToken();
    pi.tk = next_token;
    if (LexEr(next_token)){
        pi.er = lexerErr;
        return pi;
    }
    temp = pi;
    if (next_token.tp == INT){
        GetNextToken();
        if (time == 1){
            WritePush(VMOut,"constant",atol(pi.tk.lx));
        }
    }
    else if (next_token.tp == ID){
        if (time == 0){
            result[resultCount] = FindSymbol(pi.tk.lx, currentTable,time);
            resultCount++;
        }
        if (time == 1) {
            if (result[resultCount] == 0) {
                if (!FindSymbol(pi.tk.lx, totalTable, time)) {
                    pi.er = undecIdentifier;
                    if (p1.er != none)
                        return p1;
                    else
                        return pi;
                }
            }
            WritePush(VMOut, GetKind1(FindTable(temp.tk.lx,currentTable,totalTable), Find(temp.tk.lx,currentTable,totalTable)),
                      GetOffset(FindTable(temp.tk.lx,currentTable,totalTable),Find(temp.tk.lx,currentTable,totalTable)));
            resultCount++;
        }
        GetNextToken();
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (strcmp(next_token.lx, ".") == 0){\
            strcat(temp.tk.lx,".");
            GetNextToken();
            next_token = GetNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)){
                pi.er = lexerErr;
                return pi;
            }
            if (next_token.tp == ID) {
                strcat(temp.tk.lx,pi.tk.lx);
                if (time == 0){
                    result[resultCount] = FindSymbol(pi.tk.lx, currentTable,time);
                    resultCount++;
                }
                if (time == 1) {
                    if (result[resultCount] == 0) {
                        if (!FindSymbol(pi.tk.lx, totalTable, time)) {
                            pi.er = undecIdentifier;
                            if (p1.er != none)
                                return p1;
                            else
                                return pi;
                        }
                    }
                    resultCount++;
                }
            }
            else{
                pi.er = idExpected;
                return pi;
            }
        }
        next_token = PeekNextToken();
        pi.tk = next_token;
        if (strcmp(next_token.lx, "[") == 0) {
            GetNextToken();
            pi = expression();
            if (time == 1){
                WriteArithmetic(VMOut,"add");
                WritePop(VMOut,"pointer",1);
                WritePush(VMOut,"that",0);
            }
            if (pi.er != none)
                return pi;
            next_token = GetNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)){
                pi.er = lexerErr;
                return pi;
            }
            if (strcmp(next_token.lx, "]") == 0)
                ;
            else {
                pi.er = closeBracketExpected;
                return pi;
            }
        }
        else if (strcmp(next_token.lx, "(") == 0){
            GetNextToken();
            pi = expressionList(argCount_add);
            if (pi.er != none)
                return pi;
            next_token = GetNextToken();
            pi.tk = next_token;
            if (LexEr(next_token)){
                pi.er = lexerErr;
                return pi;
            }
            if (strcmp(next_token.lx, ")") == 0)
                ;
            else {
                pi.er = closeParenExpected;
                return pi;
            }
            if (time == 1){
                WriteCall(VMOut,temp.tk.lx,argCount);
            }
        }
    }
    else if (strcmp(next_token.lx, "(") == 0){
        GetNextToken();
        pi = expression();
        if (pi.er != none)
            return pi;
        next_token = GetNextToken();
        pi.tk = next_token;
        if (LexEr(next_token)){
            pi.er = lexerErr;
            return pi;
        }
        if (strcmp(next_token.lx, ")") == 0)
            ;
        else {
            pi.er = closeParenExpected;
            return pi;
        }
    }
    else if (next_token.tp == STRING){
        GetNextToken();
        if (time == 1){
            WritePush(VMOut,"constant", strlen(pi.tk.lx));
            WriteCall(VMOut,"String.new",1);
            for(int i=0;i<strlen(pi.tk.lx);i++){
                WritePush(VMOut,"constant",(int)pi.tk.lx[i]);
                WriteCall(VMOut,"String.appendChar",2);
            }
        }
    }
    else if (strcmp(next_token.lx, "true") == 0){
        GetNextToken();
        WritePush(VMOut,"constant",0);
        WriteArithmetic(VMOut,"not");
    }
    else if (strcmp(next_token.lx, "false") == 0){
        GetNextToken();
        WritePush(VMOut,"constant",0);
    }
    else if (strcmp(next_token.lx, "null") == 0){
        GetNextToken();
        WritePush(VMOut,"constant",0);
    }
    else if (strcmp(next_token.lx, "this") == 0){
        GetNextToken();
        WritePush(VMOut,"pointer",0);
    }
    else {
        pi.er = syntaxError;
        return pi;
    }
    return pi;
}

int InitParser (char* file_name)
{
    int k;
    char filename[128];
    char outputFlie[128];
    InitLexer(file_name);
    fp = fopen(file_name,"r");
    for(k= strlen(file_name);;k--){
        if (file_name[k] == '\\')
        break;
        if (k<0)
            break;
    }
    if (k>0) {
        strcpy(filename, file_name + k + 1);
    }
    else{
        strcpy(filename, file_name);
    }
    if (time == 0) {
        for (int i = 0; i < 100; i++) {
            if (strcmp(scannedFile[i], filename) == 0) {
                totalTable = currentTable;
                currentTable = 0;
                resultCount = 0;
                time = 1;
                break;
            }

        }
    }
    if (time == 1) {
        for (k = 0;; k++) {
            if (filename[k] == '.')
                break;
        }
        filename[k + 1] = 'V';
        filename[k + 2] = 'M';
        filename[k + 3] = '\0';
        filename[k + 4] = '\0';
        VMOut = fopen(filename, "w");
    }

    if (fp == NULL) {
        printf("fail to open the file! \n");
        return 0;
    }
    else
	    return 1;
}

ParserInfo Parse ()
{
	ParserInfo pi;
	pi = class();// implement the function

	return pi;
}

int StopParser ()
{
    StopLexer();
    currentTable++;
    if (time == 1){
        count = 0;
        cla_var_count = 1;
        stopcount++;
        fclose(VMOut);
        if (stopcount == fileCount) {
            time = 0;
            stopcount = 0;
            fileCount = 0;
            totalTable = 0;
            currentTable = 0;
            p1.er = none;
            classcount = 0;
            for(int i = 0;i<100;i++) {
                strcpy(scannedFile[i],"");
                strcpy(classList[i],"");
            }
            for(int i=0; i<1000;i++){
                result[i]=0;
                resultCount=0;
            }
        }
    }
    ifNum = 0;
    whileNum = 0;
    offset_reset();
    fieldCount = 0;
    fp = NULL;
	return 1;
}

#ifndef TEST_PARSER
//int main ()
//{
//    ParserInfo pi;
//    InitLexer("classExpected.jack");
//    InitParser("classExpected.jack");
//    pi = Parse();
//    StopLexer();
//    StopParser();
//    printf("%d %s %d",pi.er,pi.tk.lx,pi.tk.ln);
//	return 1;
//}
#endif
