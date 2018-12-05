/**
 * Lexical analyser
 * @author  Martin Rockar
 * @version 1.0
 */
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>
#include "lex.h"
#include "error.h"

/**
 * Function compares string with reserved keywords,
 * if there is a match,function returns value of a reserved word.
 * @param   *tmp    string to compare
 * @return          value of a keyword or -1 if it is an identifier
 */
int checkKeywords(char *tmp) {

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

/**
 * Function stores previous token, if there is no token stored, function getTokenFromInput is called.
 * @param   value   value of a token
 * @return          previous token or calls function getTokenFromInput
 */
int getToken(string *value) {
    if (prev_token != -1) {
        int temp = prev_token;
        prev_token = -1;
        return temp;
    }
    return getTokenFromInput(value);
}

/**
 * Main function of a scanner, this function read characters from stdin and transform
 * them into lexems(tokens)
 * @param   value   value of a token
 * @return          type of a token represented by integer number.
 */
int getTokenFromInput(string *value) {
    int s, state = START;
    int res = 0;
    int resTmp[4] = {0, 0, 0, '\0'};
    static int isFirstToken = 1;
    strClear(value);
    while (1) {

        s = fgetc(stdin);
        if (s == EOF) {
            if (state == START) {
                return LEX_EOF;
            }
        }
        switch (state) {
            //main switch for states
            case START:
                if (isFirstToken) {
                    if (isFirstToken) {
                        isFirstToken = 0;
                    }
                    if (s == '=') {
                        while (!isspace(s)) {
                            strAddChar(value, s);

                            if (value->length > 6) {
                                return ERR_LEXICAL;
                            }

                            s = fgetc(stdin);
                        }
                        if (strcmp(value->str, "=begin") == 0) {
                            state = BLOCK_COMMENT;
                            break;
                        } else {
                            return ERR_LEXICAL;
                        }
                    }
                }

                if (s == '\n') {
                    s = fgetc(stdin);
                    if (s == '=') {
                        while (!isspace(s)) {
                            strAddChar(value, s);

                            if (value->length > 6) {
                                return ERR_LEXICAL;
                            }

                            s = fgetc(stdin);
                        }
                        if (strcmp(value->str, "=begin") == 0) {
                            state = BLOCK_COMMENT;
                            break;
                        } else {
                            return ERR_LEXICAL;
                        }
                    } else {
                        ungetc(s, stdin);

                        return LEX_EOL;
                    }
                } else if (isspace(s)); //skip white spaces
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

                    if (s == '0') {
                        //check if first number of an multi digit number is not 0
                        s = fgetc(stdin);
                        if (isdigit(s)) {
                            return ERR_LEXICAL;
                        } else {
                            ungetc(s, stdin);
                        }
                    }
                    state = NUM;
                    break;
                } else {
                    switch (s) {
                        //switch for operators
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
                            //line comment
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
                            } else {
                                ungetc(s, stdin);

                                return ASSIGN;
                            }

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
                    //escape sequence
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
                        //hexadecimal escape sequence
                        s = fgetc(stdin);
                        if (s >= 65 && s <= 70) {
                            res = ((s - 'A') + 10);

                        } else if (s >= 97 && s <= 102) {
                            res = ((s - 'a') + 10);

                        } else if (isdigit(s)) {
                            res = s - '0';

                        } else return ERR_LEXICAL;

                        s = fgetc(stdin);

                        if (s >= 65 && s <= 70) {
                            res = res * 16 + ((s - 'A') + 10);

                        } else if (s >= 97 && s <= 102) {
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
                } else strAddChar(value, s);
                while (1) {

                    s = fgetc(stdin);
                    if (s == '#') {
                        strAddChar(value, '\\');
                        strAddChar(value, '0');
                        strAddChar(value, '3');
                        strAddChar(value, '5');
                    } else if (s == '\\') {
                        //escape sequence
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
                            //hexadecimal escape sequence
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
                    } else if (s == EOF) {
                        return ERR_LEXICAL;
                    } else if (s == '"') {
                        return STRING;
                    } else {
                        strAddChar(value, s);
                    }
                    //printf("value: %s\n",value->str);
                }

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
                    int a = checkKeywords(value->str);  //checks if string is a reserved word or an identifier

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
                //checking for end of a block comment (=end) in input
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
                                        s = fgetc(stdin);
                                        if (s == ' ' || s == '\t') {
                                            while ((s = fgetc(stdin)) != '\n') {
                                                if (s == EOF)
                                                    return ERR_LEXICAL;
                                            }
                                        }
                                        return (s == '\n') ? LEX_EOL : ERR_LEXICAL;

                                    }
                                }
                            }
                        }
                    }
                    s = fgetc(stdin);
                    if (s == EOF) {
                        return ERR_LEXICAL;
                    }
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
                } else if (isOperator(s) || isspace(s)) {
                    ungetc(s, stdin);
                    return NUM_INT;
                } else {
                    return ERR_LEXICAL;
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
                if (!isspace(s) && !isOperator(s))
                    return ERR_LEXICAL;
                ungetc(s, stdin);
                return NUM_EXP;
        }
    }
}
/**
 * Function for checking if character is an operator.
 * @param   symbol  character to check
 * @return  true if character is an operator, else it return false
 */
bool isOperator(int symbol) {
    return ((symbol == '*' || symbol == '+' || symbol == '-' || symbol == '/' || symbol == ')') ||
            (symbol == '<' || symbol == '>' || symbol == '=' || symbol == ',')) ? true : false;
}

