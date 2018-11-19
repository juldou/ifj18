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
string *value;
string *temp;
int line;
int err;

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
        "IDF",
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

int assign() {
    // pravidlo "ID" = <value>
    strCopyString(temp, value);
    switch (token) {
        case ROUNDL:
            if ((err = math_expr(temp->str)) != SYNTAX_OK) return err;
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        case ID:
            if (semantic_token_is_function(temp->str)) {
                GET_TOKEN();
                if ((err = fun_call(temp->str)) != SYNTAX_OK) return err;
                return SYNTAX_OK;
            }

            if ((err = math_expr(temp->str)) != SYNTAX_OK) return err;
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        case PRINT:
        case INPUTF:
        case INPUTI:
        case INPUTS:
        case SUBSTR:
        case LENGTH:
        case CHR:
        case ORD:
            GET_TOKEN();
            if ((err = fun_call(temp->str)) != SYNTAX_OK) return err;
            return SYNTAX_OK;

        case NUM_INT:
        case NUM_FLOAT:
        case NUM_EXP:
        case STRING:
            if ((err = math_expr(temp->str)) != SYNTAX_OK) return err;
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        case ERR_LEXICAL:
            return ERR_LEXICAL;
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
            if (semantic_add_fun_param(fun_id, value->str) == ERR_INTERNAL) return ERR_INTERNAL;
            GET_TOKEN();

            if (token == COMMA) {
                GET_TOKEN();
                if (token != ID)
                    return ERR_SYNTAX;
            }
            return fun_params(fun_id);

        case ERR_LEXICAL:
            return ERR_LEXICAL;
        default:
            return ERR_SYNTAX;
    }
}


int fun_declr() {
    strCopyString(temp, value);
    if (token != ID && token != IDF)
        return ERR_SYNTAX;
    GET_TOKEN();

    if (semantic_check_fun_definition(temp->str) == ERR_SEMANTIC_DEFINITION) return ERR_SEMANTIC_DEFINITION;

    ACCEPT(ROUNDL);

    if ((err = fun_params(temp->str)) != SYNTAX_OK) return err;

    if ((err = stat_list()) != SYNTAX_OK) return err;
    ACCEPT(KEYWORD_END);

    //todo condition
    if (token != LEX_EOL && token != LEX_EOF) return ERR_SYNTAX;
    GET_TOKEN();
    return SYNTAX_OK;
}


int params(char *fun_id) {
    // pravidlo <ITEM><ITEM_LIST
    static size_t params_count = 0;
    switch (token) {
        case LEX_EOL:
        case ROUNDR:
            err = semantic_check_fun_call_params(fun_id, params_count);
            params_count = 0;
            if (err != 0) return err;
            return SYNTAX_OK;
        case NUM_INT:
        case NUM_FLOAT:
        case NUM_EXP:
        case STRING:
        case ID:
            params_count++;
            GET_TOKEN();

            if (token == COMMA) {
                GET_TOKEN();
                if (token != ID && token != NUM_INT && token != NUM_FLOAT && token != NUM_EXP && token != STRING)
                    return ERR_SYNTAX;
                return params(fun_id);
            }

            return params(fun_id);
        case ERR_LEXICAL:
            return ERR_LEXICAL;
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
    switch (token) {
        case KEYWORD_IF:
            // pravidlo IF <EXPR> EOL <STAT_LIST> ELSE EOL <STAT_LIST> END
            GET_TOKEN();
            if ((err = bool_expr("")) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_THEN);
            ACCEPT(LEX_EOL);

            if ((err = stat_list()) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_ELSE);
            ACCEPT(LEX_EOL);

            if ((err = stat_list()) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_END);
            ACCEPT(LEX_EOL);

            return stat_list();

        case KEYWORD_WHILE:
            // pravidlo WHILE <EXPR> DO EOL <STAT_LIST> END
            GET_TOKEN();
            if ((err = bool_expr("")) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_DO);
            ACCEPT(LEX_EOL);


            if ((err = stat_list()) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_END);
            ACCEPT(LEX_EOL);

            return stat_list();
        case LEX_EOL:
            GET_TOKEN();
            return stat_list();
        case KEYWORD_NIL: //todo remove
        case KEYWORD_DEF:
        case KEYWORD_END:
        case KEYWORD_ELSE:
            return SYNTAX_OK;

        case LEX_EOF:
            return SYNTAX_OK;

        case IDF:
            GET_TOKEN();
            if ((err = fun_call(temp->str)) != SYNTAX_OK) return err;
            return stat_list();
        case ID:
            strCopyString(temp, value);

            if (semantic_token_is_function(value->str)) {
                GET_TOKEN();
                if ((err = fun_call(temp->str)) != SYNTAX_OK) return err;
                return stat_list();
            }
            strCopyString(temp, value);

            //todo temp, kym julo nespravi
            GET_TOKEN();
            switch (token) {
                case ID:
                case ROUNDL:
                case NUM_EXP:
                case NUM_FLOAT:
                case STRING:
                case NUM_INT:
                    if ((err = fun_call(temp->str)) != SYNTAX_OK) return err;
                    return stat_list();
            }
            if (token == ASSIGN) {
                GET_TOKEN();
                if ((err = assign()) != SYNTAX_OK) return err;

                return stat_list();
            }

            ACCEPT(LEX_EOL);
            return stat_list();

        case NUM_EXP:
        case NUM_FLOAT:
        case STRING:
        case NUM_INT:
            GET_TOKEN();
            return stat_list();
        case INPUTS:
        case INPUTF:
        case INPUTI:
        case PRINT:
        case ORD:
        case CHR:
        case SUBSTR:
            strCopyString(temp, value);
            GET_TOKEN();
            if ((err = fun_call(temp->str)) != SYNTAX_OK) return err;
            return stat_list();
        case ERR_LEXICAL:
            return ERR_LEXICAL;
        default:
            return ERR_SYNTAX;
    }
}

int program() {
    switch (token) {
        case KEYWORD_DEF:
            GET_TOKEN();
            if ((err = fun_declr()) != SYNTAX_OK) return err;
            return program();
            //todo

        case KEYWORD_NIL:
        case LEX_EOF:
            printf("END\n");
            return SYNTAX_OK;

        case ERR_LEXICAL:
            return ERR_LEXICAL;
        case KEYWORD_END:
        case KEYWORD_ELSE:
            GET_TOKEN();
            return program();

        default:
            if ((err = stat_list()) != SYNTAX_OK) return err;
            return program();
    }
}


int parse() {
    int result;
    value = malloc(sizeof(string));
    temp = malloc(sizeof(string));

    if (value == NULL || temp == NULL) return ERR_INTERNAL;
    if (strInit(value) == STR_ERROR) return ERR_INTERNAL;
    if (strInit(temp) == STR_ERROR) return ERR_INTERNAL;

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

    strFree(value);
    free(value);

    strFree(temp);
    free(temp);
    return result;
}
