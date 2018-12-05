#include "parser.h"
#include <stdio.h>
#include "error.h"

int main() {
    int res = parse();
    if (res == SYNTAX_OK) return 0;
    else return res;
}
