#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SYMTABLE_SIZE 101

typedef char *t_key;

typedef enum {
    FUNCTION,
    VARIABLE
} st_elem_types;

typedef struct elem_data {
    char *id;
    char **params;
    size_t params_count;
    struct st_elem *fun; // if elem is var then it points to function where its defined
    bool defined;
    bool is_builtin;
} elem_data;

typedef struct st_elem {
    t_key key;
    st_elem_types elem_type;
    elem_data *data;
    struct st_elem *ptrnext;
} st_elem;

typedef st_elem *st[SYMTABLE_SIZE];

int hash_code(t_key key);

void st_init(st *st_ptr);

st_elem *st_search(st *st_ptr, t_key key);

int st_insert(st *st_ptr, t_key key, st_elem_types elem_type, elem_data *data);

void st_clear_elem_data(st_elem *elem);

void st_clear_all(st *st_ptr);

#endif
