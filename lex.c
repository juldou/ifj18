#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "lex.h"
#include "error.h"


char *string;
int i = 0;

int checkKeywords(char *tmp) {

    //char *keyWords[] = {"def", "do", "else", "end", "if", "not", "nil", "then", "while"};
    if (strcmp("def", tmp) == 0) return KEYWORD_DEF;
    else if (strcmp("do", tmp) == 0) return KEYWORD_DO;
    else if (strcmp("else", tmp) == 0) return KEYWORD_ELSE;
    else if (strcmp("end", tmp) == 0) return KEYWORD_END;
    else if (strcmp("if", tmp) == 0) return KEYWORD_IF;
    else if (strcmp("not", tmp) == 0) return KEYWORD_NOT;
    else if (strcmp("nil", tmp) == 0) return KEYWORD_NIL;
    else if (strcmp("then", tmp) == 0) return KEYWORD_THEN;
    else if (strcmp("while", tmp) == 0) return KEYWORD_WHILE;
    else if (strcmp("inputs", tmp) == 0) return INPUTS;
    else if (strcmp("inputf", tmp) == 0) return INPUTF;
    else if (strcmp("inputi", tmp) == 0) return INPUTI;
    else if (strcmp("print", tmp) == 0) return PRINT;
    else if (strcmp("ord", tmp) == 0) return ORD;
    else if (strcmp("chr", tmp) == 0) return CHR;
    else if (strcmp("substr", tmp) == 0) return SUBSTR;


    else return -1;

}

int addCharToArray(char c, char *str) {

    if (i >= 100) {
        string = realloc(string, INCREMENT);
        if (string == NULL) {
            printf("realloc err\n");
            return ERR_LEXICAL;
        }
    }
    str[i] = c;
    i++;
    str[i] = '\0';
    return 0;
}

int getToken(char *value, int *line) {
    string = malloc(sizeof(char) * MAX_LENGTH);
    if (string == NULL) {
        printf("malloc err\n");
        return ERR_LEXICAL;
    }

    int s, state = START, f;
    string[0] = '\0';
    while (1) {

        //todo
        s = fgetc(stdin);

        switch (state) {
            case START:
                if (s == '\n') {
                    line++;
                    return LEX_EOL;
                } else if (s == EOF) {
                    return LEX_EOF;
                }
                if (isspace(s));
                else if (islower(s) || s == '_') {
                    //addCharToArray(s, string);
                    state = ID;
                } else if (isdigit(s)) {
                    state = NUM;
                } else {
                    switch (s) {
                        //operatory
                        case '+':
                            return PLUS;
                        case '-':
                            return MINUS;
                        case '*':
                            return MUL;
                        case '/':
                            return DIV;
                        case '<':
                            s = fgetc(stdin);
                            if (s == '=') {
                                return LESS_EQUAL;
                            } else {
                                ungetc(s, stdin);
                                return LESS;
                            }
                        case '>':
                            s = fgetc(stdin);
                            if (s == '=') {
                                return MORE_EQUAL;
                            } else {
                                ungetc(s, stdin);
                                return MORE;
                            }
                        case '(':
                            return ROUNDL;
                        case ')':
                            return ROUNDR;
                        case ',':
                            return COMMA;
                        case '=':
                            s = fgetc(stdin);
                            if (s == '=') {
                                return EQUAL;
                            } else if (s == 'b' || s == 'e') {
                                while (!isspace(s)) {

                                    s = fgetc(stdin);
                                    addCharToArray(s, string);
                                }
                                if (strcmp(string, "begin") == 0) {
                                    state = BLOCK_COMMENT;
                                } else if (strcmp(string, "end") == 0) {
                                    state = START;
                                } else {
                                    return ERR_LEXICAL;
                                }
                            } else {
                                ungetc(s, stdin);
                                return ASSIGN;
                            }
                    }
                    default:
                        printf("LEX_ERR\n");
                }

            case IDENTIF:
                if (isalnum(s) || s == '_' || s == '?' || s == '!') {
                    while (isalnum(s) || s == '_' ) {

                        addCharToArray(s, string);
                        s = fgetc(stdin);
                    }

                    if (s == '?' || s == '!') {
                        addCharToArray(s, string);
                    } else {
                        ungetc(s, stdin);
                    }
                    int a = checkKeywords(string);
                    if (a == -1) {
                        strcpy(value, string);
                        string[0] = '\0';
                        i=0;
                        return ID;

                    } else {
                        strcpy(value, string);
                        string[0] = '\0';
                        i=0;
                        return a;
                    }

                    case NUM:
                        f = 0;
                    while (isdigit(s)) {
                        if (isdigit(s)) {
                            addCharToArray(s, value);
                        } else if (s == '.' || s == 'e' || s == 'E') {
                            f++;
                            addCharToArray(s, value);
                            return FLOAT;
                        }
                    }
                    strcpy(value, string);
                    if (f == 0) return INT;
                    else return FLOAT;;
                }
        }
    }
}
