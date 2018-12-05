/**
* School project to subject IFJ (Formal Languages and Compilers)
* Compiler implementation of imperative language IFJ18
*
* Module for main function - starting point of compiler
*
* Author: Jan Zauska
* Login:  xzausk00
*/

#include "parser.h"
#include <stdio.h>
#include "error.h"

int main() {
    int res = parse();
    if (res == SYNTAX_OK) return 0;
    else return res;
}
