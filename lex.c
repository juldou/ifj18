#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "lex.h"
#include "error.h"

#define MAX_LENGTH 50
#define INCREMENT 100


FILE *file;

unsigned int i = 0;
char *string;


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
                        return ID;
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
                            //printf("string\n");
                            state = STRING;
                            break;
                        case '#':
                            //printf("line comment\n");
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
                                        //printf("block comment\n");
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
                            return ERR_LEXICAL;
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
                        return ID;

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
                    return ERR_LEXICAL;
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
//         return ERR_LEXICAL;
//     }
//     file = stdin;
//     if (file == NULL) {
//         printf("file cant open\n");
//         return ERR_LEXICAL;
//     }
//
//     do {
//         a = getToken(value, &line);
//         printf("Value: %s line: %d type: %d\n", value, line, a);
//         value[0] = '\0';
//         if (a == ERR_LEXICAL) {
//             break;
//         }
//
//     } while (a != LEX_EOF);
//     return 0;
// }
