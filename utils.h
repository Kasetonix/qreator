#ifndef UTILS
#define UTILS

#include <stdio.h>

typedef struct String {
    char *chars;
    size_t len;
} String;

void warning(char *msg);
void error(char *msg);
size_t len(char *str);
void read(String *str);
String pack_into_string(char *str);

#endif
