#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <stdlib.h>
#include <string.h>

#define SYMTABLE_SIZE 101

typedef char *t_key;

typedef struct elem_data {
    int some_data;
} elem_data;

typedef struct st_elem {
    t_key key;
    elem_data *data;
    struct st_elem *ptrnext;
} st_elem;


typedef st_elem *st[SYMTABLE_SIZE];

int hash_code(t_key key);

void st_init(st *st_ptr);

st_elem *st_search(st *st_ptr, t_key key);

int st_insert(st *st_ptr, t_key key, elem_data *data);

elem_data *st_read(st *st_ptr, t_key key);

void st_delete(st *st_ptr, t_key key);

void st_clear_all(st *st_ptr);

#endif