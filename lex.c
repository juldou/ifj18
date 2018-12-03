#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "lex.h"
#include "error.h"


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

int prev_token = -1;

int getToken(string *value, int *line) {
    if (prev_token != -1) {
        int temp = prev_token;
        prev_token = -1;
        return temp;
    }
    return getTokenFromInput(value, line);
}

int getTokenFromInput(string *value, int *line) {
    static int lineCount = 0;
    int s, state = START;
    int res = 0;
    int resTmp[4] = {0, 0, 0, '\0'};
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
                            state = STRING;
                            break;
                        case '#':
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
                                        state = BLOCK_COMMENT;
                                        break;
                                    }
                                    s = fgetc(stdin);
                                }
                                if (strcmp(value->str, "begin") != 0) {
                                    return ERR_LEXICAL;
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

                if (s == '#') {
                    strAddChar(value, '\\');
                    strAddChar(value, '0');
                    strAddChar(value, '3');
                    strAddChar(value, '5');
                } else if (s == '\\') {
                    s = fgetc(stdin);
                    if (s == '"') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '3');
                        strAddChar(value, '4');
                    } else if (s == 'n') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '1');
                        strAddChar(value, '0');
                    } else if (s == 't') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '0');
                        strAddChar(value, '9');
                    } else if (s == 's') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '3');
                        strAddChar(value, '2');
                    } else if (s == '\\') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '9');
                        strAddChar(value, '2');

                    } else if (s == 'x') {
                        s = fgetc(stdin);
                        if (isupper(s)) {
                            res = ((s - 'A') + 10);

                        } else if (islower(s)) {
                            res = ((s - 'a') + 10);

                        } else if (isdigit(s)) {
                            res = s - '0';

                        } else if (!isalpha(s)) return ERR_LEXICAL;

                        s = fgetc(stdin);

                        if (isupper(s)) {
                            res = res * 16 + ((s - 'A') + 10);

                        } else if (islower(s)) {
                            res = res * 16 + ((s - 'a') + 10);

                        } else if (isdigit(s)) {
                            res = res * 16 + (s - '0');
                        } else ungetc(s, stdin);

                        strAddChar(value, '\\');
                        resTmp[0] = res % 10 + '0';
                        res /= 10;
                        resTmp[1] = res % 10 + '0';
                        res /= 10;
                        resTmp[2] = res % 10 + '0';
                        strAddChar(value, resTmp[2]);
                        strAddChar(value, resTmp[1]);
                        strAddChar(value, resTmp[0]);

                    } else {
                        ungetc(s, stdin);
                    }
                } else if (s == ' ') {
                    strAddChar(value, '\\');
                    strAddChar(value, '0');
                    strAddChar(value, '3');
                    strAddChar(value, '2');
                } else if (s == '"') {
                    state = START;
                    return STRING;
                } else strAddChar(value, s);
                while (1) {

                    s = fgetc(stdin);
                    if (s == '#') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '3');
                        strAddChar(value, '5');
                    } else if (s == '\\') {
                        s = fgetc(stdin);
                        if (s == '"') {
                            strAddChar(value, '\\');
                            strAddChar(value, '0');
                            strAddChar(value, '3');
                            strAddChar(value, '4');
                        } else if (s == 'n') {
                            strAddChar(value, '\\');
                            strAddChar(value, '0');
                            strAddChar(value, '1');
                            strAddChar(value, '0');
                        } else if (s == 't') {
                            strAddChar(value, '\\');
                            strAddChar(value, '0');
                            strAddChar(value, '0');
                            strAddChar(value, '9');
                        } else if (s == 's') {
                            strAddChar(value, '\\');
                            strAddChar(value, '0');
                            strAddChar(value, '3');
                            strAddChar(value, '2');
                        } else if (s == '\\') {
                            strAddChar(value, '\\');
                            strAddChar(value, '0');
                            strAddChar(value, '9');
                            strAddChar(value, '2');
                        } else if (s == 'x') {
                            s = fgetc(stdin);
                            if (isupper(s)) {
                                res = ((s - 'A') + 10);

                            } else if (islower(s)) {
                                res = ((s - 'a') + 10);

                            } else if (isdigit(s)) {
                                res = s - '0';

                            } else if (!isalpha(s)) return ERR_LEXICAL;

                            s = fgetc(stdin);

                            if (isupper(s)) {
                                res = res * 16 + ((s - 'A') + 10);

                            } else if (islower(s)) {
                                res = res * 16 + ((s - 'a') + 10);

                            } else if (isdigit(s)) {
                                res = res * 16 + (s - '0');
                            } else ungetc(s, stdin);

                            strAddChar(value, '\\');
                            resTmp[0] = res % 10 + '0';
                            res /= 10;
                            resTmp[1] = res % 10 + '0';
                            res /= 10;
                            resTmp[2] = res % 10 + '0';
                            strAddChar(value, resTmp[2]);
                            strAddChar(value, resTmp[1]);
                            strAddChar(value, resTmp[0]);

                        } else {
                            ungetc(s, stdin);
                        }
                    } else if (s == ' ') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '3');
                        strAddChar(value, '2');
                    } else if (s == '"') {
                        return STRING;
                    } else {
                        strAddChar(value, s);
                    }
                    //printf("value: %s\n",value->str);
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
                        s = fgetc(stdin);
                        if (isalpha(s)) {
                            while (isalpha(s)) {
                                strAddChar(value, s);
                                s = fgetc(stdin);
                            }
                            return ERR_LEXICAL;
                        } else {
                            ungetc(s, stdin);
                        }
                    } else {
                        ungetc(s, stdin);
                    }
                    int a = checkKeywords(value->str);

                    if (a == -1) {
                        if (value->str[value->length - 1] == '?' || value->str[value->length - 1] == '!')
                            return IDF;
                        return ID;

                    } else {
                        return a;
                    }
                }
                break;
            case BLOCK_COMMENT:

                while (1) {
                    if (s == '\n') {
                        s = fgetc(stdin);
                        if (s == '=') {
                            s = fgetc(stdin);
                            if (s == 'e') {
                                s = fgetc(stdin);
                                if (s == 'n') {
                                    s = fgetc(stdin);
                                    if (s == 'd') {
                                        while ((s = fgetc(stdin) != '\n')) {
                                            if (s == EOF)
                                                return ERR_LEXICAL;
                                        }
                                        return LEX_EOL;
                                    }
                                }
                            }
                        }
                    }
                    s = fgetc(stdin);

                }
            case NUM:

                while (isdigit(s)) {

                    strAddChar(value, s);
                    s = fgetc(stdin);
                }
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
                if (!isdigit(s)) {
                    return ERR_LEXICAL;
                }
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
                if(!isspace(s))
                    return ERR_LEXICAL;
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
//         a = getTokenFromInput(value, &line);
//         printf("Value: %15s line: %5d type: %d\n", value->str, line, a);
////         if (a == LEX_EOF) {
////             break;
////         }
//
//     } while (a != LEX_EOF);
//     return 0;
// }
