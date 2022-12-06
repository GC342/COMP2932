//
// Created by 15822 on 2021/4/26.
//
#include "VMWriter.h"


void WritePush(FILE *VMOut,char segment[128],int index){
    char temp[128],num[10];
    strcpy(temp,"push ");
    strcat(temp,segment);
    sprintf(num," %d\n",index);
    strcat(temp,num);
    fputs(temp,VMOut);
}

void WritePop(FILE *VMOut,char segment[128],int index){
    char temp[128],num[10];
    strcpy(temp,"pop ");
    strcat(temp,segment);
    sprintf(num," %d\n",index);
    strcat(temp,num);
    fputs(temp,VMOut);

}

void WriteArithmetic(FILE *VMOut,char command[5]){
    char temp[10];
    strcpy(temp,command);
    strcat(temp,"\n");
    fputs(temp,VMOut);
}

void WriteLabel(FILE *VMOut,char label[30]){
    char temp[36];
    strcpy(temp,"label ");
    strcat(temp,label);
    strcat(temp,"\n");
    fputs(temp,VMOut);

}

void WriteGoto(FILE *VMOut,char label[35]){
    char temp[36];
    strcpy(temp,"goto ");
    strcat(temp,label);
    strcat(temp,"\n");
    fputs(temp,VMOut);

}

void WriteIf(FILE *VMOut,char label[35]){
    char temp[36];
    strcpy(temp,"if-goto ");
    strcat(temp,label);
    strcat(temp,"\n");
    fputs(temp,VMOut);

}

void WriteCall(FILE *VMOut,char name[128], int nArgs){
    char temp[36],num[10];
    strcpy(temp,"call ");
    strcat(temp,name);
    sprintf(num," %d\n",nArgs);
    strcat(temp,num);
    fputs(temp,VMOut);

}

void WriteFunction(FILE *VMOut,char name[128], int nLocals){
    char temp[36],num[10];
    strcpy(temp,"function ");
    strcat(temp,name);
    sprintf(num," %d\n",nLocals);
    strcat(temp,num);
    fputs(temp,VMOut);

}

void WriteReturn(FILE *VMOut){
    fputs("return\n",VMOut);

}

