#include "error.h"
#include "symtable.h"
#include "semantic.h"

st st_global; // for global funcs

int semantic_prepare() {
    st_init(&st_global);
    if (add_builtin_funcs_to_st() == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

int add_builtin_funcs_to_st() {
    if (insert_fun_to_st("inputs", 0, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("inputi", 0, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("inputf", 0, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("print", 1, true, true) == ERR_INTERNAL) return ERR_INTERNAL; // variadic, but > 0
    if (insert_fun_to_st("length", 1, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("substr", 3, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("ord", 2, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    if (insert_fun_to_st("chr", 1, true, true) == ERR_INTERNAL) return ERR_INTERNAL;
    return 0;
}

/* initialize fun elem data */
void init_fun_elem_data(elem_data *data, size_t params_count, bool defined, bool is_builtin) {
    data->id = NULL;
    data->params = NULL;
    data->params_count = 0;
    data->defined = defined;
    data->is_builtin = is_builtin;
}

/* insert function to symtable */
int insert_fun_to_st(char *fun_id, size_t params_count, bool defined, bool is_builtin) {
    elem_data *data = (elem_data *) malloc(sizeof(elem_data));
    if (data == NULL) return ERR_INTERNAL;

    init_fun_elem_data(data, params_count, defined, is_builtin);

    data->id = (char *) malloc((strlen(fun_id) + 1) * sizeof(char));
    if (data->id == NULL) return ERR_INTERNAL;
    strcpy(data->id, fun_id);

    if (st_insert(&st_global, fun_id, data) == ERR_INTERNAL) return ERR_INTERNAL;
    else return 0;
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

/* add parameter to function declaration in symtable */
int semantic_add_fun_param(char *fun_id, char *param) {
    st_elem *elem = st_search(&st_global, fun_id);
    elem->data->params = (char **) realloc(elem->data->params, sizeof(*(elem->data->params)) + sizeof(char *));;
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
