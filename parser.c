#include "lex.h"
#include "error.h"
#include "parser.h"
#include "semantic.h"
#include "expr_parser.h"

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

int assign() {
    // pravidlo "ID" = <value>
    switch (token) {
        case ROUNDL:
        case ID:
            if (math_expr() != SYNTAX_OK) return ERR_SYNTAX;
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


int fun_params(char *fun_id) {
    // pravidlo <ITEM><ITEM_LIST
    switch (token) {
        case ROUNDR:
            GET_TOKEN();
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        case ID:
            if (semantic_add_fun_param(fun_id, value) == ERR_INTERNAL) return ERR_INTERNAL;
            GET_TOKEN();

            if (token == COMMA) {
                GET_TOKEN();
                if (token != ID)
                    return ERR_SYNTAX;
            }
            return fun_params(fun_id);

        default:
            return ERR_SYNTAX;
    }
}


int fun_declr() {
    ACCEPT(ID);
    char previous_token_value[100];  // TODO: refactor descend
    strcpy(previous_token_value, value);

    if (semantic_check_fun_definition(value) == ERR_SEMANTIC_DEFINITION) return ERR_SEMANTIC_DEFINITION;

    ACCEPT(ROUNDL);

    if (fun_params(previous_token_value) != SYNTAX_OK) return ERR_SYNTAX;

    if (stat_list() != SYNTAX_OK) return ERR_SYNTAX;

    ACCEPT(KEYWORD_END);

    //todo condition
    if (token != LEX_EOL && token != LEX_EOF) return ERR_SYNTAX;
    GET_TOKEN();
    return SYNTAX_OK;
}


int params(char *fun_id) {
    // pravidlo <ITEM><ITEM_LIST
    int err;
    static size_t params_count = 0;
    switch (token) {
        case LEX_EOL:
        case ROUNDR:
            err = semantic_check_fun_call_params(fun_id, params_count);
            params_count = 0;
            if (err != 0) return err;
            else return SYNTAX_OK;
        case INT:
        case FLOAT:
        case STRING:
        case ID:
            params_count++;
            GET_TOKEN();

            if (token == COMMA) {
                GET_TOKEN();
                if (token != ID) return ERR_SYNTAX;
                return params(fun_id);
            }

            return params(fun_id);
        default:
            return ERR_SYNTAX;
    }
}


int fun_call(char *fun_id) {
    int brackets = 0;
    if (token == ROUNDL) {
        brackets = 1;
        GET_TOKEN();
    }

    int err = params(fun_id);
    if (err != SYNTAX_OK) return err;

    if (brackets) {
        ACCEPT(ROUNDR);
    }
    ACCEPT(LEX_EOL);
    return SYNTAX_OK;

}

int stat_list() {
    int err;
    char previous_token_value[100];
    switch (token) {
        case KEYWORD_IF:
            // pravidlo IF <EXPR> EOL <STAT_LIST> ELSE EOL <STAT_LIST> END
            GET_TOKEN();
            if (bool_expr() != SYNTAX_OK) return ERR_SYNTAX;

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
            if (bool_expr() != SYNTAX_OK) return ERR_SYNTAX;

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
            strcpy(previous_token_value, value);
            GET_TOKEN();
            if (token == ASSIGN) {
                GET_TOKEN();
                if (assign() != SYNTAX_OK) return ERR_SYNTAX;
                return stat_list();
            }
            if (token != LEX_EOL || semantic_token_is_function(previous_token_value)) {
                if ((err = fun_call(previous_token_value)) != SYNTAX_OK) return err;
                return stat_list();

            }
            ACCEPT(LEX_EOL);
            return stat_list();

        default:
            return ERR_SYNTAX;

        case INPUTS:
        case INPUTF:
        case INPUTI:
        case PRINT:
        case ORD:
        case CHR:
        case SUBSTR:
            strcpy(previous_token_value, value); // TODO: refactor descend
            GET_TOKEN();
            if ((err = fun_call(previous_token_value)) != SYNTAX_OK) return err;
            return stat_list();
    }
}

int program() {
    int err;
    switch (token) {
        case KEYWORD_DEF:
            GET_TOKEN();
            if ((err = fun_declr()) != SYNTAX_OK) return err;
            return program();
            //todo

        case EOF:
            return SYNTAX_OK;

            //todo temp kym nejde EOF
        case KEYWORD_NIL:
            printf("END\n");
            return SYNTAX_OK;

            if ((token = getToken(value, &line)) == ERR_LEXICAL) return ERR_LEXICAL;
            //todo temp
            return program();

        case ERR_LEXICAL:
            return ERR_LEXICAL;
        default:
        case ID:
            if ((err = stat_list()) != SYNTAX_OK) return err;
            return program();
    }
}


int parse() {
    int result;
    if (semantic_prepare() == ERR_INTERNAL) return ERR_INTERNAL;

    GET_TOKEN();
    result = program();

    switch (result) {
        case SYNTAX_OK:
            printf("*****SYNTAX OK*****\n");
            break;
        case ERR_SYNTAX:
            printf("*****SYNTAX ERROR*****\n");
            break;
        case ERR_LEXICAL:
            printf("*****LEX ERROR*****\n");
            break;
        case ERR_SEMANTIC_DEFINITION:
            printf("*****SEMANTIC DEFINITION ERROR*****\n");
            break;
        case ERR_SEMANTIC_PARAMETERS_COUNT:
            printf("*****SEMANTIC PARAMETERS COUNT*****\n");
            break;
        default:
            printf("Unknown Error\n");
    }

    return result;
}