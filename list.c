/**
* School project to subject IFJ (Formal Languages and Compilers)
* Compiler implementation of imperative language IFJ18
*
* Module for list data structure
*
* Author: Julius Marko  Jan Zauska
* Login:  xmarko17      xzausk00
*/

#include <string.h>
#include "list.h"

void InitList(tList *L) {
    L->Act = NULL;
    L->First = NULL;
}

void DisposeList(tList *L) {
    tElemPtr actual = L->First;
    tElemPtr next = NULL;
    while (actual != NULL) {  // iterate over list and delete elements (clean up memory)
        next = actual->ptr;
        free(actual);
        actual = next;
    }
    L->Act = NULL;  // there can be InitList() instead, but I rather do it this way
    L->First = NULL;
}

char *AppendToList(tList *L) {
    tElemPtr tElemToAddPtr = (tElemPtr) malloc(sizeof(struct tElem));
    if (tElemToAddPtr == NULL) return NULL;
    tElemToAddPtr->ptr = NULL;

    if (L->First == NULL) {
        L->First = tElemToAddPtr;
    } else {
        tElemPtr tmp = L->First;
        while (tmp->ptr != NULL) {
            tmp = tmp->ptr;
        }
        tmp->ptr = tElemToAddPtr;
    }
    return tElemToAddPtr->instruction;
}

char *PostInsert(tList *L) {
    if (Active(L)) {
        tElemPtr toInsert = (tElemPtr) malloc(sizeof(struct tElem));
        if (toInsert == NULL) return NULL;
        toInsert->ptr = L->Act->ptr;
        L->Act->ptr = toInsert;  // insert it after active elem.
        return toInsert->instruction;
    }
    return NULL;
}

int Active(tList *L) {
    return (L->Act == NULL) ? 0 : 1;
}

void printList(tList *L) {
    tElemPtr temp = L->First;
    while (temp != NULL) {
        fprintf(stdout, "%s", temp->instruction);
        temp = temp->ptr;
    }
}

int find(tList *L, char *key) {
    if (L == NULL)
        return 0;
    tElemPtr temp = L->First;

    while (temp) {
        if (strcmp(temp->instruction, key) == 0) {
            L->Act = temp;
            return 1;
        }
        temp = temp->ptr;
    }
    return 0;
}
