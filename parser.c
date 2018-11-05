#include <stdio.h>
#include "lex.c"
#include "error.h"

int token;

//temp
char value[100];
int line;

int declr() {
    switch (token) {
        case ID:
            // pravidlo "ID" = <value>
            if ((token = getToken(value, &line)) == ERR_LEXICAL)
                return LEX_ERR;
            if (token != EQUAL)
                return ERR_SYNTAX;
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            return declr();
        case
    }


}

int params() {
    // pravidlo <ID>(<ITEM><ITEM_LIST)
    switch (token) {

        case ROUNDR:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            return SYNTAX_OK;
        case ID:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return LEX_ERR;

            if (token == COMMA) {
                if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
                if (token == ID) return params();
                return ERR_SYNTAX;
            }

            return params();
        default:
            return ERR_SYNTAX;


    }
}

int program() {
    int result;
    switch (token) {
        case ID:

        default:
            return ERR_SYNTAX;
    }
}

int parse() {
    int result;

    token = getToken(value, &line);
    if (token == ERR_LEXICAL) {
        result = ERR_LEXICAL;
    } else {
        result = program();
    }
    return result;
}