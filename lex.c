#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "lex.h"
#include "error.h"

char *valuess[] = {
        "KEYWORD_DEF",
        "KEYWORD_DO",
        "KEYWORD_ELSE",
        "KEYWORD_END",
        "KEYWORD_IF",
        "KEYWORD_NOT",
        "KEYWORD_NIL",
        "KEYWORD_THEN",
        "KEYWORD_WHILE",
        "ID",
        "NUM_INT",
        "NUM_FLOAT",
        "NUM_EXP",
        "STRING",
        "LEX_EOL",
        "COMMA",
        "ROUNDL",
        "ROUNDR",
        "ASSIGN",
        "PLUS",
        "MINUS",
        "MUL",
        "DIV",
        "LESS",
        "MORE",
        "LESS_EQUAL",
        "MORE_EQUAL",
        "EQUAL",
        "NOT_EQUAL",

        "START",
        "LEX_EOF",
        "NUM",
        "COMMENT",
        "BLOCK_COMMENT",
        "IDENTIF",
        "INPUTS",
        "INPUTF",
        "INPUTI",
        "PRINT",
        "ORD",
        "CHR",
        "SUBSTR",
        "LENGTH",
        "MAXTOKEN"

};

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
    else if (strcmp("length", tmp) == 0) return LENGTH;

    else return -1;

}

int getToken(string *value, int *line) {
    static int lineCount = 0;
    int s, state = START;
    strClear(value);
    while (1) {

        s = fgetc(stdin);
        if (s == EOF) {
            (*line) = lineCount;
            return LEX_EOF;
        }
        switch (state) {

            case START:

                if (s == '\n') {
                    lineCount++;
                    (*line) = lineCount;
                    return LEX_EOL;
                } else if (isspace(s));
                else if (islower(s) || s == '_') {
                    strAddChar(value, s);
                    s = fgetc(stdin);
                    if (isalnum(s) || s == '_' || s == '?' || s == '!') {
                        ungetc(s, stdin);
                        state = IDENTIF;
                    } else {
                        ungetc(s, stdin);
                        return ID;
                    }
                    break;


                } else if (isdigit(s)) {
                    strAddChar(value, s);
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
                                if (s == EOF)
                                    return LEX_EOF;
                                s = fgetc(stdin);
                            }
                            return LEX_EOL;
                        case '=':
                            s = fgetc(stdin);
                            if (s == '=') {
                                return EQUAL;
                            } else if (s == 'b' || s == 'e') {
                                while (!isspace(s)) {
                                    strAddChar(value, s);
                                    if (strcmp(value->str, "begin") == 0) {
                                        //printf("block comment\n");
                                        state = BLOCK_COMMENT;
                                        break;
                                    }
                                    s = fgetc(stdin);
                                }
                            } else {
                                ungetc(s, stdin);

                                return ASSIGN;
                            }
                            break;

                        case '!':
                            s = fgetc(stdin);
                            if (s == '=')
                                return NOT_EQUAL;
                            ungetc(s, stdin);
                            return ERR_LEXICAL;
                        default:
                            return ERR_LEXICAL;
                    }

                }
                break;
            case STRING:
                strAddChar(value, s);
                while (1) {
                    s = fgetc(stdin);
                    if (s == '"') {
                        break;
                    }
                    strAddChar(value, s);
                }
                return STRING;

            case IDENTIF:
                if (isalnum(s) || s == '_' || s == '?' || s == '!') {
                    while (isalnum(s) || s == '_') {

                        strAddChar(value, s);
                        s = fgetc(stdin);
                    }

                    if (s == '?' || s == '!') {
                        strAddChar(value, s);
                    } else {
                        ungetc(s, stdin);
                    }
                    int a = checkKeywords(value->str);
                    //printf("a = %d\n",a);
                    if (a == -1) {
                        return ID;

                    } else {
                        return a;
                    }
                }
                break;
            case BLOCK_COMMENT:

                while (1) {
                    s = fgetc(stdin);
                    if (s == '\n') {
                        s = fgetc(stdin);
                        if (s == '=') {
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
                    strAddChar(value, s);
                    s = fgetc(stdin);
                    //printf("nacitam nove s:%c\n", s);


                }
                //printf("vyskocil som z whilu\n");
                if (s == '.') {
                    strAddChar(value, s);
                    state = NUM_FLOAT;
                    break;
                } else if (s == 'e' || s == 'E') {
                    strAddChar(value, s);
                    state = NUM_EXP;
                    break;
                } else {
                    ungetc(s, stdin);
                    return NUM_INT;
                }


            case NUM_FLOAT:

                while (isdigit(s)) {
                    strAddChar(value, s);
                    s = fgetc(stdin);
                }
                if (s == 'e' || s == 'E') {
                    strAddChar(value, s);
                    state = NUM_EXP;
                    break;
                } else {
                    ungetc(s, stdin);
                    return NUM_FLOAT;
                }
            case NUM_EXP:

                if (s == '+' || s == '-') {
                    strAddChar(value, s);
                    s = fgetc(stdin);
                }
                if (!isdigit(s)) {
                    return ERR_LEXICAL;
                }
                while (isdigit(s)) {
                    strAddChar(value, s);
                    s = fgetc(stdin);
                }
                ungetc(s, stdin);
                return NUM_EXP;
        }
    }
}


// int main() {
//     int a;
//     string *value;
//     value = malloc(sizeof(string));
//     if (value == NULL) return ERR_INTERNAL;
//     if (strInit(value) == STR_ERROR) return ERR_INTERNAL;
//     int line = 0;
//
//     do {
//         a = getToken(value, &line);
//         printf("Value: %15s line: %5d type: %s\n", value->str, line, valuess[a]);
////         if (a == LEX_EOF) {
////             break;
////         }
//
//     } while (a != LEX_EOF);
//     return 0;
// }
