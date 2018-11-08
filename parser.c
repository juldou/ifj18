#include "lex.h"
#include "error.h"

#define SYNTAX_OK 101

#define GET_TOKEN() do {if ((token = getToken(value, &line)) == ERR_LEXICAL)\
return LEX_ERR;} while(0)

int token;

//temp
char value[100];
int line;

char *values[] = {
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
        "INT",
        "FLOAT",
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
        "IDENTIF"

};

int program();


int expr() {
    switch (token) {
        case ID:
            GET_TOKEN();
            return SYNTAX_OK;
        default:
            return ERR_SYNTAX;
    }

}

int assign() {
    // pravidlo "ID" = <value>
    switch (token) {
        case ID:
            if (expr() != SYNTAX_OK) return ERR_SYNTAX;
            GET_TOKEN();
            if (token != LEX_EOL) return ERR_SYNTAX;
            GET_TOKEN();
            return SYNTAX_OK;
        case INT:
        case FLOAT:
        case STRING:
            GET_TOKEN();
            if (token != LEX_EOL) return ERR_SYNTAX;
            GET_TOKEN();
            return SYNTAX_OK;

        default:
            return ERR_SYNTAX;

    }
}

int stat_list() {
    switch (token) {
        case KEYWORD_IF:

            // pravidlo IF <EXPR> EOL
            if ((token = getToken(value, &line)) == ERR_LEXICAL)
                return LEX_ERR;

            if (expr() != SYNTAX_OK) return ERR_SYNTAX;

            if (token != KEYWORD_THEN) return ERR_SYNTAX;

            if ((token = getToken(value, &line)) == ERR_LEXICAL)
                return LEX_ERR;
            if (token != LEX_EOL) return ERR_SYNTAX;

            GET_TOKEN();

            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

            if (token != KEYWORD_ELSE) return ERR_SYNTAX;
            GET_TOKEN();
            if (token != LEX_EOL) return ERR_SYNTAX;
            GET_TOKEN();
            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

            if (token != KEYWORD_END) return ERR_SYNTAX;
            GET_TOKEN();
            if (token != LEX_EOL) return ERR_SYNTAX;
            GET_TOKEN();
            return SYNTAX_OK;

        case KEYWORD_DO:
        case KEYWORD_WHILE:

        case KEYWORD_END:
        case KEYWORD_ELSE:
            return SYNTAX_OK;
        case LEX_EOF:
            return SYNTAX_OK;

        case ID:
            GET_TOKEN();
            if (token == EQUAL) {
                GET_TOKEN();
                return assign();
            }
            if (token != LEX_EOL) return ERR_SYNTAX;
            GET_TOKEN();
            return SYNTAX_OK;

        default:
            return ERR_SYNTAX;
    }
}


int params() {
    // pravidlo <ITEM><ITEM_LIST
    switch (token) {

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
    // pravidlo <ITEM><ITEM_LIST
    switch (token) {


        case ROUNDR:
            GET_TOKEN();
            if(token != LEX_EOL) return ERR_SYNTAX;
            GET_TOKEN();
            return SYNTAX_OK;

        case ID:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return LEX_ERR;

            if (token == COMMA) {
                if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            }
            return  fun_params();


        default:
            return ERR_SYNTAX;


    }
}

int fun() {
    int brackets = 0;
    if (token != ID) return ERR_SYNTAX;
    if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
    if (token == ROUNDL) {
        brackets = 1;
        if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;

    }
    if (fun_params() != SYNTAX_OK) return ERR_SYNTAX;

//    if (brackets) {
//        if (token != ROUNDR)return ERR_SYNTAX;
//        if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
//    }
//    if (token != LEX_EOL) return ERR_SYNTAX;

//    if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;

    if (token == KEYWORD_END) {
        // prazdna funkcia
        if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
        if (token != LEX_EOL && token != LEX_EOF) return ERR_SYNTAX;
        return SYNTAX_OK;
    }
    if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

    if (token != KEYWORD_END) return ERR_SYNTAX;

    if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
    if (token != LEX_EOL && token != LEX_EOF) return ERR_SYNTAX;
    return SYNTAX_OK;


}


int program() {
    switch (token) {
        case KEYWORD_DEF:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            if (fun() != SYNTAX_OK) return ERR_SYNTAX;
            return program();
            //todo
        case ID:
            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;
            return program();
        case EOF:
            return SYNTAX_OK;

        case LEX_EOL:
            printf("EOL\n");
            return SYNTAX_OK;

            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            //todo temp
            return program();

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
    if (result == SYNTAX_OK)
        printf("*****SYNTAX OK\"*****\n");
    else if (result == ERR_SYNTAX)
        printf("*****SYNTAX ERROR*****\n");
    else if (result == ERR_LEXICAL)
        printf("*****LEX ERROR*****\n");


    return result;
}