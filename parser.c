#include "lex.h"
#include "error.h"

#define SYNTAX_OK 101

#define GET_TOKEN() do {if ((token = getToken(value, &line)) == ERR_LEXICAL)\
return ERR_LEXICAL;} while(0)

#define ACCEPT(type) do{ if(token != type) return ERR_SYNTAX;\
    GET_TOKEN();} while(0)
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
        case INT:
        case FLOAT:
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
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;
        case INT:
        case FLOAT:
        case STRING:
            GET_TOKEN();
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        default:
            return ERR_SYNTAX;

    }
}


int stat_list() {
    switch (token) {
        case KEYWORD_IF:
            // pravidlo IF <EXPR> EOL <STAT_LIST> ELSE EOL <STAT_LIST> END
            GET_TOKEN();
            if (expr() != SYNTAX_OK) return ERR_SYNTAX;

            ACCEPT(KEYWORD_THEN);
            ACCEPT(LEX_EOL);

            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

            ACCEPT(KEYWORD_ELSE);
            ACCEPT(LEX_EOL);

            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

            ACCEPT(KEYWORD_END);
            ACCEPT(LEX_EOL);

            return stat_list();

        case KEYWORD_WHILE:
            // pravidlo WHILE <EXPR> DO EOL <STAT_LIST> END
            GET_TOKEN();
            if (expr() != SYNTAX_OK) return ERR_SYNTAX;

            ACCEPT(KEYWORD_DO);
            ACCEPT(LEX_EOL);


            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

            ACCEPT(KEYWORD_END);
            ACCEPT(LEX_EOL);

            return stat_list();

        case KEYWORD_DEF:
        case KEYWORD_END:
        case KEYWORD_ELSE:
            return SYNTAX_OK;

            //todo temp
        case KEYWORD_NIL:
        case LEX_EOF:
            return SYNTAX_OK;

        case ID:
            GET_TOKEN();
            if (token == ASSIGN) {
                GET_TOKEN();
                if (assign() != SYNTAX_OK) return ERR_SYNTAX;
                return stat_list();
            }
            ACCEPT(LEX_EOL);
            return stat_list();

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
            GET_TOKEN();

            if (token == COMMA) {
                GET_TOKEN();
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
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        case ID:
            GET_TOKEN();

            if (token == COMMA) {
                if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            }
            return fun_params();

        default:
            return ERR_SYNTAX;
    }
}


int fun() {
    ACCEPT(ID);
    ACCEPT(ROUNDL);

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

    ACCEPT(KEYWORD_END);

    //todo condition
    if (token != LEX_EOL && token != LEX_EOF) return ERR_SYNTAX;
    GET_TOKEN();
    return SYNTAX_OK;


}


int program() {
    switch (token) {
        case KEYWORD_DEF:
            GET_TOKEN();
            if (fun() != SYNTAX_OK) return ERR_SYNTAX;
            return program();
            //todo
        case ID:
            if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;
            return program();
        case EOF:
            return SYNTAX_OK;

            //todo temp kym nejde EOF
        case KEYWORD_NIL:
            printf("END\n");
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

    GET_TOKEN();
    result = program();

    switch (result){
        case SYNTAX_OK:
            printf("*****SYNTAX OK\"*****\n");
            break;
        case ERR_SYNTAX:
            printf("*****SYNTAX ERROR*****\n");
            break;
        case ERR_LEXICAL:
            printf("*****LEX ERROR*****\n");
            break;
        default:
            printf("Unknown Error\n");
    }

    return result;
}