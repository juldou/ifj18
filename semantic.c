#include "error.h"
#include "symtable.h"

st st_global; // for global funcs

void semantic_prepare() {
    st_init(&st_global);
}

/* initialize fun elem data */
void init_fun_elem_data(elem_data *data) {
    data->id = NULL;
    data->params = NULL;
    data->params_count = 0;
}

/* insert function to symtable */
int insert_fun_to_st(char *fun_id) {
    elem_data *data = (elem_data *) malloc(sizeof(elem_data));
    if (data == NULL) return ERR_INTERNAL;

    init_fun_elem_data(data);

    data->id = (char *) malloc((strlen(fun_id) + 1) * sizeof(char *));
    if (data->id == NULL) return ERR_INTERNAL;
    strcpy(data->id, fun_id);

    if (st_insert(&st_global, fun_id, data) == ERR_INTERNAL) return ERR_INTERNAL;
    else return 0;
}

/* check if function with given fun_id was defined previously */
int semantic_check_fun_defined(char *fun_id) {
    if (st_search(&st_global, fun_id) == NULL) return insert_fun_to_st(fun_id);
    else return ERR_SEMANTIC_DEFINITION;
}

/* add parameter to function declaration in symtable */
int semantic_add_fun_param(char *fun_id, char *param) {
    st_elem *elem = st_search(&st_global, fun_id);
    elem->data->params = (char **) realloc(elem->data->params, sizeof(char **));
    if (elem->data->params == NULL) return ERR_INTERNAL;

    elem->data->params[elem->data->params_count] = (char *) malloc((strlen(param) + 1) * sizeof(char *));
    if (elem->data->params[elem->data->params_count] == NULL)
        return ERR_INTERNAL;
    else {
        strcpy(elem->data->params[elem->data->params_count], param);
        elem->data->params_count++;
    }

    return 0;
}
