//
// Created by janko on 11/5/18.
//

#ifndef IFJ_LEX_H
#define IFJ_LEX_H
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>

#define MAX_LENGTH 50
#define INCREMENT 100
#define LEX_ERR -1

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
    INT,
    FLOAT,
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
    IDENTIFIER,
    COMMENT,
    BLOCK_COMMENT,
    IDENTIF,
    MAXTOKEN

} Types;



int checkKeywords(char *tmp) ;

int addCharToArray(char c, char *str);

int getToken(char *value, int *line);


#endif //IFJ_LEX_H
