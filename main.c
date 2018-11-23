#include "parser.h"
#include <stdio.h>

int main() {
    int res = parse();
 //   printf("%d\n", res);
    if (res == SYNTAX_OK) return 0;  // TODO: remove it
    else return res;
}
