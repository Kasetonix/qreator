#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;

typedef struct String {
    char *chars;
    size_t len;
} String;

typedef struct Array_u8 {
    size_t len;
    u8 *elems;
} Array_u8;

typedef struct Array_u16 {
    size_t len;
    u16 *elems;
} Array_u16;

void warning(char *msg);
void error(char *msg);
size_t len(char *str);
void read(String *str);
String pack_into_string(char *str);
void pack_into_bytes(u16 *array, size_t array_len, u8 single_size, Array_u8 *packed);
void pack_into_bytes_var(u16 *array, size_t array_len, u8 *single_size, Array_u8 *packed);

#endif
