//
// Created by janko on 11/5/18.
//

#ifndef IFJ_LEX_H
#define IFJ_LEX_H

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "str.h"

#define MAX_LENGTH 50
#define INCREMENT 100

typedef enum {
    KEYWORD_DEF,
    KEYWORD_DO,
    KEYWORD_ELSE,
    KEYWORD_END,
    KEYWORD_IF,
    KEYWORD_NOT,
    KEYWORD_NIL,
    KEYWORD_THEN,
    KEYWORD_WHILE,

    ID,
    NUM_INT,
    NUM_FLOAT,
    NUM_EXP,
    STRING,
    LEX_EOL,
    COMMA,
    ROUNDL,
    ROUNDR,
    ASSIGN,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LESS,
    MORE,
    LESS_EQUAL,
    MORE_EQUAL,
    EQUAL,
    NOT_EQUAL,

    START,
    LEX_EOF,
    NUM,
    COMMENT,
    BLOCK_COMMENT,
    IDENTIF,
    INPUTS,
    INPUTF,
    INPUTI,
    PRINT,
    ORD,
    CHR,
    SUBSTR,
    MAXTOKEN

} Types;


int checkKeywords(char *tmp);

int addCharToArray(char c, char **str);

int getToken(string *value, int *line);


#endif //IFJ_LEX_H
