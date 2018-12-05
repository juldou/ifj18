/**
* School project to subject IFJ (Formal Languages and Compilers)
* Compiler implementation of imperative language IFJ18
*
* Module for lexical analysis
*
* Author: Martin Rockar  Jan Zauska  Jan Vavro
* Login:  xrocka00       xzausk00    xvavro05
*/

#ifndef IFJ_LEX_H
#define IFJ_LEX_H

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include <stdbool.h>
#include "str.h"

typedef enum {
    KEYWORD_DEF = 200,
    KEYWORD_DO,
    KEYWORD_ELSE,
    KEYWORD_END,
    KEYWORD_IF,
    KEYWORD_NOT,
    KEYWORD_NIL,
    KEYWORD_THEN,
    KEYWORD_WHILE,

    ID,
    IDF,
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
    LENGTH,
    MAXTOKEN

} Types;

int getToken(string *value);

int checkKeywords(char *tmp);

int getTokenFromInput(string *value);

bool isOperator(int symbol);


#endif //IFJ_LEX_H
