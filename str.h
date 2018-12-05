/**
 * School project to subject IFJ (Formal Languages and Compilers)
 * Compiler implementation of imperative language IFJ18
 *
 * Header of module for operations with string, inspired by the one used in "jednoduchy_interpret"
 */

#define STR_ERROR   1
#define STR_SUCCESS 0

typedef struct {
    char *str;
    int length;
    int allocSize;
} string;


int strInit(string *s);

void strFree(string *s);

void strClear(string *s);

int strAddChar(string *s1, char c);

int strCopyString(string *s1, string *s2);
