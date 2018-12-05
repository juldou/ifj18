/**
 * School project to subject IFJ (Formal Languages and Compilers)
 * Compiler implementation of imperative language IFJ18
 *
 * Module for operations with string, inspired by the one used in "jednoduchy_interpret"
 */
#include <string.h>
#include <stdlib.h>
#include "str.h"

#define STR_LEN_INC 8

int strInit(string *s)
{
    if ((s->str = (char *) malloc(STR_LEN_INC)) == NULL)
        return STR_ERROR;
    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = STR_LEN_INC;
    return STR_SUCCESS;
}

void strFree(string *s)
{
    free(s->str);
}

void strClear(string *s)
{
    s->str[0] = '\0';
    s->length = 0;
}

int strAddChar(string *s1, char c)
{
    if (s1->length + 1 >= s1->allocSize) {
        if ((s1->str = (char *) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
            return STR_ERROR;
        s1->allocSize = s1->length + STR_LEN_INC;
    }
    s1->str[s1->length] = c;
    s1->length++;
    s1->str[s1->length] = '\0';
    return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
    int newLength = s2->length;
    if (newLength >= s1->allocSize) {
        // pamet nestaci, je potreba provest realokaci
        if ((s1->str = (char *) realloc(s1->str, newLength + 1)) == NULL)
            return STR_ERROR;
        s1->allocSize = newLength + 1;
    }
    strcpy(s1->str, s2->str);
    s1->length = newLength;
    return STR_SUCCESS;
}
