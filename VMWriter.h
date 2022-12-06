//
// Created by 15822 on 2021/4/26.
//

#ifndef CODEGENERATIONPROJECT_VMWRITER_H
#define CODEGENERATIONPROJECT_VMWRITER_H

#include <stdio.h>
#include <string.h>

void WritePush(FILE *VMOut,char segment[128],int index);
void WritePop(FILE *VMOut,char segment[128],int index);
void WriteArithmetic(FILE *VMOut,char command[5]);
void WriteLabel(FILE *VMOut,char label[30]);
void WriteGoto(FILE *VMOut,char label[35]);
void WriteIf(FILE *VMOut,char label[35]);
void WriteCall(FILE *VMOut,char name[128], int nArgs);
void WriteFunction(FILE *VMOut,char name[128], int nLocals);
void WriteReturn(FILE *VMOut);

#endif //CODEGENERATIONPROJECT_VMWRITER_H
