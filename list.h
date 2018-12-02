#ifndef IFJ_LIST_H
#define IFJ_LIST_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include "error.h"

#define INSTR_LENGTH 1020

typedef struct tElem {
    struct tElem *ptr;
    char instruction[INSTR_LENGTH];
//    char *instruction;
} *tElemPtr;

typedef struct {
    tElemPtr Act;
    tElemPtr First;
} tList;

void InitList(tList *);

void DisposeList(tList *);

char *AppendToList(tList *);

char *PostInsert(tList *);

int Active(tList *);

void printList(tList *L);

int find(tList *L, char *key);

#endif //IFJ_LIST_H
