#ifndef IFJ_LIST_H
#define IFJ_LIST_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<string.h>
#include "error.h"

#define TRUE 1
#define FALSE 0

typedef struct tElem {
    struct tElem *ptr;
    char instruction[1020];
//    char *instruction;
} *tElemPtr;

typedef struct {
    tElemPtr Act;
    tElemPtr First;
} tList;

void InitList (tList *);
void DisposeList (tList *);
int Insert(tList *);
int InsertFirst (tList *, char *);
char *AppendToList(tList *);
void First (tList *);
int CopyFirst (tList *, char *);
void DeleteFirst (tList *);
void PostDelete (tList *);
int PostInsert (tList *, char *);
void Succ (tList *);
int Copy (tList *, char *);
void Actualize (tList *, char *);
int  Active (tList *);
int CreateInstr();
void printList (tList *L);

#endif //IFJ_LIST_H
