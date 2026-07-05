#include "utils.h"
#include <stdlib.h>

void warning(char *msg) {
    fprintf(stdout, "\e[1;33m[WARN ]\e[0m: %s\n", msg);
}

void error(char *msg) {
    fprintf(stderr, "\e[1;31m[ERROR]\e[0m: %s\n", msg);
    exit(1);
}

size_t len(char *str) {
    size_t i = 0;
    while (str[i] != '\0')
        i++;
    return i;
}

void read(String *str) {
    size_t buflen;
    ssize_t len;
    str->chars = NULL;
    str->len = 0;
    len = getline(&str->chars, &buflen, stdin);
    str->len = len - 1;
    *(str->chars + str->len) = '\0';
}

String pack_into_string(char *str) {
    String string;
    string.len = len(str);
    string.chars = malloc(string.len * sizeof(char));

    for (size_t i = 0; i < string.len; i++)
        string.chars[i] = str[i];

    return string;
}
