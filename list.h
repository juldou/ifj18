/**
* School project to subject IFJ (Formal Languages and Compilers)
* Compiler implementation of imperative language IFJ18
*
* Module for list data structure
*
* Author: Julius Marko  Jan Zauska
* Login:  xmarko17      xzausk00
*/

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
