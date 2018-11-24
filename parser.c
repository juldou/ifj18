#include "lex.h"
#include "error.h"
#include "parser.h"
#include "semantic.h"
#include "expr_parser.h"
#include "code_gen.h"

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

int assign(char *fun_id) {
    // pravidlo "ID" = <value>
    char previous_token_value[value->length + 1];  // TODO: refactor descend
    strcpy(previous_token_value, value->str);
    strCopyString(temp, value);
    switch (token) {
        case ROUNDL:
            if ((err = math_expr(fun_id)) != SYNTAX_OK) return err;
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;

        case ID:
            if (semantic_token_is_function(temp->str)) {
                GET_TOKEN();
                if ((err = fun_call(previous_token_value, fun_id)) != SYNTAX_OK) return err;
                return SYNTAX_OK;
            }
            //todo remove ked bude precedencna
            //GEN_INSTR("LF@%s ", previous_token_value);
            if ((err = math_expr(fun_id)) != SYNTAX_OK) return err;
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
            if ((err = fun_call(previous_token_value, fun_id)) != SYNTAX_OK) return err;
            return SYNTAX_OK;

        case NUM_INT:
            //todo result from precedence analysis
            //GEN_INSTR("int@%s", value->str);
            if ((err = math_expr(fun_id)) != SYNTAX_OK) return err;
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;
        case NUM_FLOAT:
        case NUM_EXP:
            //GEN_INSTR("float@%s", value->str);
            if ((err = math_expr(fun_id)) != SYNTAX_OK) return err;
            ACCEPT(LEX_EOL);
            return SYNTAX_OK;
        case STRING:
            //GEN_INSTR("string@%s", value->str);
            if ((err = math_expr(fun_id)) != SYNTAX_OK) return err;
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
            err = semantic_add_fun_param(fun_id, value->str);
            if (err != 0) return err;
            GEN_INSTR("DEFVAR LF@%s", value->str);

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
    char previous_token_value[value->length + 1];  // TODO: refactor descend
    strcpy(previous_token_value, value->str);
    strCopyString(temp, value);
    if (token != ID && token != IDF)
        return ERR_SYNTAX;
    GET_TOKEN();

    if (semantic_check_fun_definition(previous_token_value) == ERR_SEMANTIC_DEFINITION) return ERR_SEMANTIC_DEFINITION;

    if (gen_fun_header(previous_token_value) == ERR_INTERNAL) return ERR_INTERNAL;

    ACCEPT(ROUNDL);

    if ((err = fun_params(previous_token_value)) != SYNTAX_OK) return err;

    if ((err = stat_list(previous_token_value)) != SYNTAX_OK) return err;

    ACCEPT(KEYWORD_END);

    //todo condition
    if (token != LEX_EOL && token != LEX_EOF) return ERR_SYNTAX;
    GET_TOKEN();

    if (gen_fun_footer(previous_token_value) == ERR_INTERNAL) return ERR_INTERNAL;

    return SYNTAX_OK;
}


int params(char *fun_id, char *called_from_fun, unsigned *par_count) {
    // pravidlo <ITEM><ITEM_LIST
    char previous_token_value[value->length + 1];
    static size_t params_count = 0;
    switch (token) {
        case LEX_EOL:
        case ROUNDR:
            err = semantic_check_fun_call_params(fun_id, params_count);
            *par_count = params_count;
            params_count = 0;
            if (err != 0) return err;
            return SYNTAX_OK;
        case ID:
            strcpy(previous_token_value, value->str);
            if (semantic_check_var_defined(called_from_fun, previous_token_value) == ERR_SEMANTIC_DEFINITION)
                return ERR_SEMANTIC_DEFINITION;
        case NUM_INT:
        case NUM_FLOAT:
        case NUM_EXP:
        case STRING:
            params_count++;

            GEN_INSTR("DEFVAR TF@%%%d", params_count);

            //todo dat do <>
            if (token == NUM_INT) {
                GEN_INSTR("MOVE TF@%%%d int@%s", params_count, value->str);
            } else if (token == NUM_FLOAT || token == NUM_EXP) {
                GEN_INSTR("MOVE TF@%%%d float@%s", params_count, value->str);
            } else if (token == STRING) {
                GEN_INSTR("MOVE TF@%%%d string@%s", params_count, value->str);
            } else if (token == ID) {
                GEN_INSTR("MOVE TF@%%%d LF@%s", params_count, value->str);
            }

            if (strcmp("print", fun_id) == 0) GEN_INSTR("WRITE TF@%%%d", params_count);

            // check if param is defined
            GET_TOKEN();

            if (token == COMMA) {
                GET_TOKEN();
                if (token != ID && token != NUM_INT && token != NUM_FLOAT && token != NUM_EXP && token != STRING)
                    return ERR_SYNTAX;
                return params(fun_id, called_from_fun, par_count);
            }

            return params(fun_id, called_from_fun, par_count);
        case ERR_LEXICAL:
            return ERR_LEXICAL;
        default:
            return ERR_SYNTAX;
    }
}


int fun_call(char *fun_id, char *called_from_fun) {
    if (!semantic_token_is_function(fun_id)) return ERR_SEMANTIC_DEFINITION;

    int brackets = 0;
    if (token == ROUNDL) {
        brackets = 1;
        GET_TOKEN();
    }

    GEN_INSTR("%s", "CREATEFRAME");
    unsigned par_count; // TODO: try to move params_count from params here
    int err = params(fun_id, called_from_fun, &par_count);
    if (err != SYNTAX_OK) return err;

    if (brackets) {
        ACCEPT(ROUNDR);
    }
    ACCEPT(LEX_EOL);

    if (is_fun_builtin(fun_id) && !is_fun_defined(fun_id)) gen_builtin_fun(fun_id);

    if (!is_print_fun(fun_id)) GEN_INSTR("CALL *%s", fun_id);

    return SYNTAX_OK;
}

int stat_list(char *fun_id) {
    static int cnt = 0;
    char previous_token_value[value->length + 1];
    strcpy(previous_token_value, value->str);
    switch (token) {
        case KEYWORD_IF:
            cnt++;
            // pravidlo IF <EXPR> EOL <STAT_LIST> ELSE EOL <STAT_LIST> END
            GET_TOKEN();
            if ((err = bool_expr(fun_id)) != SYNTAX_OK) return err;
                GEN_INSTR("JUMPIFNEQ ELSE_LABEL_%d %s %s", cnt, "GF@expr_res", "bool@true");
            ACCEPT(KEYWORD_THEN);
            ACCEPT(LEX_EOL);

            if ((err = stat_list(fun_id)) != SYNTAX_OK) return err;

            GEN_INSTR("JUMP ELSE_END_%d", cnt);

            ACCEPT(KEYWORD_ELSE);
            GEN_INSTR("LABEL ELSE_LABEL_%d", cnt);
            ACCEPT(LEX_EOL);

            if ((err = stat_list(fun_id)) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_END);
            ACCEPT(LEX_EOL);
            GEN_INSTR("LABEL ELSE_END_%d", cnt);

            return stat_list(fun_id);

        case KEYWORD_WHILE:
            cnt++;
            GEN_INSTR("LABEL WHILE_START_%d", cnt);
            // pravidlo WHILE <EXPR> DO EOL <STAT_LIST> END
            GET_TOKEN();
            if ((err = bool_expr(fun_id)) != SYNTAX_OK) return err;

//todo jumpif condition
                GEN_INSTR("JUMP WHILE_END_%d", cnt);
            ACCEPT(KEYWORD_DO);
            ACCEPT(LEX_EOL);


            if ((err = stat_list(fun_id)) != SYNTAX_OK) return err;

            ACCEPT(KEYWORD_END);
            ACCEPT(LEX_EOL);
            GEN_INSTR("JUMP WHILE_START_%d", cnt);
            GEN_INSTR("LABEL WHILE_END_%d", cnt);
            return stat_list(fun_id);
        case LEX_EOL:
            GET_TOKEN();
            return stat_list(fun_id);
        case KEYWORD_NIL:
            GET_TOKEN();
            ACCEPT(LEX_EOL);
            return stat_list(fun_id);
        case KEYWORD_DEF:
        case KEYWORD_END:
        case KEYWORD_ELSE:
            return SYNTAX_OK;

        case LEX_EOF:
            return SYNTAX_OK;

        case IDF:
            GET_TOKEN();
            if ((err = fun_call(previous_token_value, fun_id)) != SYNTAX_OK) return err;
            return stat_list(fun_id);
        case ID:
            strcpy(previous_token_value, value->str);

            if (semantic_token_is_function(previous_token_value)) {
                GET_TOKEN();
                if ((err = fun_call(previous_token_value, fun_id)) != SYNTAX_OK) return err;
                return stat_list(fun_id);
            }
            strcpy(previous_token_value, value->str);

            GET_TOKEN();
            if (token == ASSIGN) {
                GET_TOKEN();
                if (!semantic_token_is_variable(previous_token_value, fun_id))
                    GEN_INSTR("DEFVAR LF@%s", previous_token_value);

                if ((err = insert_var_to_st(previous_token_value, fun_id, true)) == ERR_INTERNAL) return err;

                if ((err = assign(fun_id)) != SYNTAX_OK) return err; // TODO : maybe not
                GEN_INSTR("MOVE LF@%s GF@%s ", previous_token_value, "expr_res");

                return stat_list(fun_id);
            }

            if (semantic_check_var_defined(fun_id, previous_token_value) == ERR_SEMANTIC_DEFINITION)
                return ERR_SEMANTIC_DEFINITION;
            ACCEPT(LEX_EOL);
            return stat_list(fun_id);

        case NUM_EXP:
        case NUM_FLOAT:
        case STRING:
        case NUM_INT:
            GET_TOKEN();
            return stat_list(fun_id);
        case INPUTS:
        case INPUTF:
        case INPUTI:
        case PRINT:
        case LENGTH:
        case ORD:
        case CHR:
        case SUBSTR:
            strcpy(previous_token_value, value->str); // TODO: refactor descend
            GET_TOKEN();
            if ((err = fun_call(previous_token_value, fun_id)) != SYNTAX_OK) return err;
            return stat_list(fun_id);
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

        case LEX_EOF:
            return SYNTAX_OK;

        case ERR_LEXICAL:
            return ERR_LEXICAL;
        case KEYWORD_END:
        case KEYWORD_ELSE:
            GET_TOKEN();
            return program();

        default:
        case ID:
            if ((err = stat_list("")) != SYNTAX_OK) return err;
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
    if (code_gen_prepare() == ERR_INTERNAL) return ERR_INTERNAL;

    gen_header();
    GET_TOKEN();
    result = program();

    switch (result) {
        case SYNTAX_OK:
            //  printf("*****SYNTAX OK*****\n");
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
        case ERR_SEMANTIC_TYPE:
            printf("*****SEMANTIC TYPE ERROR*****\n");
            break;
        case ERR_SEMANTIC_PARAMETERS_COUNT:
            printf("*****SEMANTIC PARAMETERS COUNT ERROR*****\n");
            break;
        case ERR_SEMANTIC_OTHER:
            printf("*****SEMANTIC OTHER ERROR*****\n");
            break;
        default:
            printf("Unknown Error\n");
    }

    code_generate();

    if (semantic_clean() == ERR_INTERNAL) return ERR_INTERNAL;
    if (code_gen_clean() == ERR_INTERNAL) return ERR_INTERNAL;

    strFree(value);
    free(value);

    strFree(temp);
    free(temp);
    return result;
}
