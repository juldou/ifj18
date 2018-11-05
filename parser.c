#include "lex.h"
#include "error.h"

#define SYNTAX_OK 1

int token;

//temp
char value[100];
int line;
program();


int stat_list(){
    switch (token) {

        default:
            return ERR_SYNTAX;
    }
}

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
        default:
            return ERR_SYNTAX;
    }


}

int params() {
    // pravidlo <ID>(<ITEM><ITEM_LIST)
    switch (token) {

        case ROUNDR:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            return SYNTAX_OK;

        case INT:
        case FLOAT:
        case STRING:
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

int fun_params() {
    // pravidlo <ID>(<ITEM><ITEM_LIST)
    switch (token) {

        case ROUNDR:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            if(token != EOL) return ERR_SYNTAX;
            return SYNTAX_OK;

        case ID:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return LEX_ERR;

            if (token == COMMA) {
                if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
                if (token == ID) return params();
                return ERR_SYNTAX;
            }

            if( params() != SYNTAX_OK)
                return ERR_SYNTAX;

            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            return program();

        default:
            return ERR_SYNTAX;


    }
}

int fun() {
    switch (token) {
        case ID:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            if (token == ROUNDL)
                if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
                if(fun_params() != SYNTAX_OK) return ERR_SYNTAX;
        default:
            return ERR_SYNTAX;

    }
}



int program() {
    switch (token) {
        case KEYWORD_DEF:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            return fun() && program();
            //todo
        case ID:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            if (token == ROUNDL)
                return stat_list() && program();
        case EOF:
            return SYNTAX_OK;


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