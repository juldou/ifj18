#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "lex.h"
#include "error.h"

#define MAX_LENGTH 50
#define INCREMENT 100
typedef enum {
    KEYWORD_DEF,    //0
    KEYWORD_DO,     //1
    KEYWORD_ELSE,   //2
    KEYWORD_END,    //3
    KEYWORD_IF,     //4
    KEYWORD_NOT,    //5
    KEYWORD_NIL,    //6
    KEYWORD_THEN,   //7
    KEYWORD_WHILE,  //8

    ID,             //9
    NUM_INT,        //10
    NUM_FLOAT,      //11
    NUM_EXP,        //12
    STRING,         //13
    LEX_EOL,        //14
    COMMA,          //15
    ROUNDL,         //16
    ROUNDR,         //17
    ASSIGN,         //18
    PLUS,           //19
    MINUS,          //20
    MUL,            //21
    DIV,            //22
    LESS,           //23
    MORE,           //24
    LESS_EQUAL,     //25
    MORE_EQUAL,     //26
    EQUAL,          //27
    NOT_EQUAL,      //28

    START,          //29
    LEX_EOF,        //30
    NUM,            //31
    IDENTIFIER,     //32
    COMMENT,        //33
    BLOCK_COMMENT,  //34
    IDENTIF         //35

} Types;

FILE *file;

unsigned int i = 0;
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

    if (i >= INCREMENT) {
        string = realloc(string, sizeof(str) + INCREMENT*sizeof(char));
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

    int s, state = START, tmp;
    string[0] = '\0';
    while (1) {

        s = fgetc(stdin);
        if (s == EOF) {
            return LEX_EOF;
        }
        switch (state) {

            case START:

                if (s == '\n') {
                    line++;
                    return LEX_EOL;
                }
                else if (isspace(s));
                else if (islower(s) || s == '_') {
                    addCharToArray(s, string);
                    s = fgetc(stdin);
                    if (isalnum(s) || s == '_' || s == '?' || s == '!') {
                        ungetc(s, stdin);
                        state = IDENTIF;
                    } else {
                        ungetc(s, stdin);
                        strcpy(value, string);
                        string[0] = '\0';
                        i = 0;
                        state = START;
                        return IDENTIFIER;
                    }
                    break;


                } else if (isdigit(s)) {
                    addCharToArray(s, string);
                    state = NUM;
                    break;
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
                        case '"':
                            printf("string\n");
                            state = STRING;
                            break;
                        case '#':
                            printf("line comment\n");
                            s = fgetc(stdin);
                            while (s != '\n') {
                                s = fgetc(stdin);
                            }
                            break;
                        case '=':
                            s = fgetc(stdin);
                            if (s == '=') {
                                return EQUAL;
                            } else if (s == 'b' || s == 'e') {
                                while (!isspace(s)) {
                                    addCharToArray(s, string);
                                    if (strcmp(string, "begin") == 0) {
                                        printf("block comment\n");
                                        state = BLOCK_COMMENT;
                                        string[0] = '\0';
                                        i = 0;
                                        break;
                                    }
                                    s = fgetc(stdin);
                                }
                            } else {
                                ungetc(s, stdin);

                                return ASSIGN;
                            }
                            break;

                        default:
                            printf("LEX_ERR\n");
                            break;
                    }

                }
                break;
            case STRING:
                addCharToArray(s, string);
                while (1) {
                    s = fgetc(stdin);
                    if (s == '"') {
                        break;
                    }
                    addCharToArray(s, string);
                }
                strcpy(value, string);
                string[0] = '\0';
                i = 0;
                state = START;
                return STRING;

            case IDENTIF:
                if (isalnum(s) || s == '_' || s == '?' || s == '!') {
                    while (isalnum(s) || s == '_') {

                        addCharToArray(s, string);
                        s = fgetc(stdin);
                    }

                    if (s == '?' || s == '!') {
                        addCharToArray(s, string);
                    } else {
                        ungetc(s, stdin);
                    }
                    int a = checkKeywords(string);
                    printf("a = %d\n",a);
                    if (a == -1) {
                        strcpy(value, string);
                        string[0] = '\0';
                        i = 0;
                        state = START;
                        return IDENTIFIER;

                    } else {
                        strcpy(value, string);
                        string[0] = '\0';
                        i = 0;
                        state = START;
                        return a;
                    }
                }
                break;
            case BLOCK_COMMENT:

                while (1) {
                    s = fgetc(stdin);
                    if(s == '\n') {
                        s = fgetc(stdin);
                        if (s == '='){
                            s = fgetc(stdin);
                            if (s == 'e') {
                                s = fgetc(stdin);
                                if (s == 'n') {
                                    s = fgetc(stdin);
                                    if (s == 'd') {
                                        state = START;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            case NUM:

                while (isdigit(s)) {
                    //printf("pridavam do pola znak %c\n", s);
                    addCharToArray(s, string);
                    s = fgetc(stdin);
                    //printf("nacitam nove s:%c\n", s);


                }
                //printf("vyskocil som z whilu\n");
                if (s == '.') {
                    addCharToArray(s, string);
                    state = NUM_FLOAT;
                    break;
                } else if (s == 'e' || s == 'E') {
                    addCharToArray(s, string);
                    state = NUM_EXP;
                    break;
                } else {
                    strcpy(value, string);
                    string[0] = '\0';
                    i = 0;
                    ungetc(s, stdin);
                    state = START;
                    return NUM_INT;
                }


            case NUM_FLOAT:

                while (isdigit(s)) {
                    addCharToArray(s, string);
                    s = fgetc(stdin);
                }
                if (s == 'e' || s == 'E') {
                    addCharToArray(s, string);
                    state = NUM_EXP;
                    break;
                } else {
                    strcpy(value, string);
                    string[0] = '\0';
                    i = 0;
                    ungetc(s, stdin);
                    state = START;
                    return NUM_FLOAT;
                }
            case NUM_EXP:

                if (s == '+' || s == '-') {
                    addCharToArray(s, string);
                    s = fgetc(stdin);
                }
                if (!isdigit(s)) {
                    return LEX_ERR;
                }
                while (isdigit(s)) {
                    addCharToArray(s, string);
                    s = fgetc(stdin);
                }
                strcpy(value, string);
                string[0] = '\0';
                i = 0;
                ungetc(s, stdin);
                state = START;
                return NUM_EXP;

        }
    }
}


// int main() {
//     int a;
//     char value[100];
//     int line;
//     string = malloc(sizeof(char) * MAX_LENGTH);
//     if (string == NULL) {
//         printf("malloc err\n");
//         return LEX_ERR;
//     }
//     file = stdin;
//     if (file == NULL) {
//         printf("file cant open\n");
//         return LEX_ERR;
//     }
//
//     do {
//         a = getToken(value, &line);
//         printf("Value: %s line: %d type: %d\n", value, line, a);
//         value[0] = '\0';
//         if (a == LEX_ERR) {
//             break;
//         }
//
//     } while (a != LEX_EOF);
//     return 0;
// }
