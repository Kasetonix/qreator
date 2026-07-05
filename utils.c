#include "utils.h"
#include <stdlib.h>

void warning(char *msg) {
    fprintf(stdout, "\e[1;33m[WARN ]\e[0m: %s\n", msg);
}

void error(char *msg) {
    fprintf(stderr, "\e[1;31m[ERROR]\e[0m: %s\n", msg);
    exit(1);
}
