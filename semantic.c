#include "error.h"
#include "symtable.h"
#include "semantic.h"

st st_global; // for global funcs
st st_local; // for local vars

int semantic_prepare() {
    st_init(&st_global);
    st_init(&st_local);
    if (add_builtin_funcs_to_st() == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int semantic_clean() {
    st_clear_all(&st_global);
    st_clear_all(&st_local);
    return 0;
}

int add_builtin_funcs_to_st() {
    if (insert_fun_to_st("inputs", 0, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("inputi", 0, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("inputf", 0, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("print", 1, false, true) == ERR_INTERNAL) return ERR_INTERNAL; // variadic, but > 0
    if (insert_fun_to_st("length", 1, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("substr", 3, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("ord", 2, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("chr", 1, false, true) == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

/* initialize var elem data */
void init_var_elem_data(elem_data *data, st_elem *fun,bool defined) {
    data->id = NULL;
    data->defined = defined;
    data->fun = fun;
}

/* initialize fun elem data */
void init_fun_elem_data(elem_data *data, size_t params_count, bool defined, bool is_builtin) {
    data->id = NULL;
    data->params = NULL;
    data->params_count = params_count;
    data->defined = defined;
    data->is_builtin = is_builtin;
}

/* insert var to symtable */
int insert_var_to_st(char *var_id, char *fun_id, bool defined) {
    size_t size = strlen(var_id) + strlen(fun_id) + 2;
    char var_key_in_ht[size];
    strcpy(var_key_in_ht, "\0");
    strcat(var_key_in_ht, var_id);
    strcat(var_key_in_ht, "_");
    strcat(var_key_in_ht, fun_id);

    st_elem *var = st_search(&st_local, var_key_in_ht);
    if (var != NULL) return 0;

    elem_data *data = (elem_data *) malloc(sizeof(elem_data));
    if (data == NULL) return ERR_INTERNAL;
    st_elem *fun = st_search(&st_global, fun_id);
    init_var_elem_data(data, fun, defined);

    data->id = (char *) malloc((strlen(var_id) + 1) * sizeof(char));
    if (data->id == NULL) return ERR_INTERNAL;
    strcpy(data->id, var_id);

    if (st_insert(&st_local, var_key_in_ht, VARIABLE, data) == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

/* insert function to symtable */
int insert_fun_to_st(char *fun_id, size_t params_count, bool defined, bool is_builtin) {
    elem_data *data = (elem_data *) malloc(sizeof(elem_data));
    if (data == NULL) return ERR_INTERNAL;

    init_fun_elem_data(data, params_count, defined, is_builtin);

    data->id = (char *) malloc((strlen(fun_id) + 1) * sizeof(char));
    if (data->id == NULL) return ERR_INTERNAL;
    strcpy(data->id, fun_id);

    if (st_insert(&st_global, fun_id, FUNCTION, data) == ERR_INTERNAL) return ERR_INTERNAL;
    else return 0;
}

/* check is variable is defined */ // TODO: that function is bad! problem with ""
int semantic_check_var_defined(char *fun_id, char *var_id) {
    size_t size = strlen(var_id) + strlen(fun_id) + 2;
    char var_key_in_ht[size];
    strcpy(var_key_in_ht, "\0");
    strcat(var_key_in_ht, var_id);
    strcat(var_key_in_ht, "_");
    strcat(var_key_in_ht, fun_id);

    st_elem *fun = st_search(&st_global, fun_id);
    st_elem *var = st_search(&st_local, var_key_in_ht);

    /* handle of case when var doesn't exist and we are not in any function decl */
    if (var == NULL && ( strcmp(fun_id, "") == 0 )) return ERR_SEMANTIC_DEFINITION;

    if (function_parameter_exists(fun, var_id)) return 0;
    if (var != NULL && var->data->fun == fun) return 0;
    return ERR_SEMANTIC_DEFINITION;
}

/* check if function with given fun_id was defined. If not -> add it to symtable */
int semantic_check_fun_definition(char *fun_id) {
    if (st_search(&st_global, fun_id) == NULL) return insert_fun_to_st(fun_id, 0, true, false);
    else return ERR_SEMANTIC_DEFINITION;
}

/* check that number of parameters in funtion call is correct */
int semantic_check_fun_call_params(char *fun_id, size_t params_count) {
    st_elem *elem = st_search(&st_global, fun_id);
    if (elem == NULL) {  // function is called but not defined yet
        if (insert_fun_to_st(fun_id, 0, false, false) == ERR_INTERNAL) return ERR_INTERNAL;
    } else {
        /* handle builtin function print (must have > 0 parameters) */
        if (strcmp(elem->data->id, "print") == 0) {
            if (params_count == 0) return ERR_SEMANTIC_PARAMETERS_COUNT;
        }
            /* compare parameters count in function definition and function call */
        else if (elem->data->params_count != params_count) return ERR_SEMANTIC_PARAMETERS_COUNT;
    }
    return 0;
}

bool function_parameter_exists(st_elem *elem, char *param) {
    if (elem == NULL) return false;
    for (size_t j = 0; j < elem->data->params_count; j++) {
        if (strcmp(elem->data->params[j], param) == 0) {
            return true;
        }
    }
    return false;
}

/* add parameter to function declaration in symtable */
int semantic_add_fun_param(char *fun_id, char *param) {
    st_elem *elem = st_search(&st_global, fun_id);

    if (function_parameter_exists(elem, param)) return ERR_SEMANTIC_OTHER;

    elem->data->params = (char **) realloc(elem->data->params, sizeof(*(elem->data->params)) * (elem->data->params_count + 1));
    if (elem->data->params == NULL) return ERR_INTERNAL;

    elem->data->params[elem->data->params_count] = (char *) malloc((strlen(param) + 1) * sizeof(char));
    if (elem->data->params[elem->data->params_count] == NULL)
        return ERR_INTERNAL;
    else {
        strcpy(elem->data->params[elem->data->params_count], param);
        elem->data->params_count++;
    }

    return 0;
}

int set_fun_defined(char *fun_id) {
    st_elem *elem = st_search(&st_global, fun_id);
    return elem->data->defined = true;
}

bool semantic_token_is_function(char *fun_id) {  // TODO: rename to just token_is_function
    st_elem *elem = st_search(&st_global, fun_id);
    if (elem == NULL) return false;
    else return true;
}

bool is_fun_builtin(char *fun_id) {
    st_elem *elem = st_search(&st_global, fun_id);
    return elem->data->is_builtin;
}

bool is_fun_defined(char *fun_id) {
    st_elem *elem = st_search(&st_global, fun_id);
    return elem->data->defined;
}

bool semantic_token_is_variable(char *var_id, char *fun_id) {
    size_t size = strlen(var_id) + strlen(fun_id) + 2;
    char var_key_in_ht[size];
    strcpy(var_key_in_ht, "\0");
    strcat(var_key_in_ht, var_id);
    strcat(var_key_in_ht, "_");
    strcat(var_key_in_ht, fun_id);

    st_elem *var = st_search(&st_local, var_key_in_ht);

    if (var == NULL) return false;
    return true;
}
