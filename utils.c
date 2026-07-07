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

void pack_into_bytes(u16 *array, size_t array_len, u8 single_size, Array_u8 *packed) {
    size_t array_i, packed_i, packed_len;
    i8 offset;
    u8 top_bits, bottom_bits;
    u16 bits;

    offset = 16 - single_size;

    array_i = 0; packed_i = 0;
    while (array_i < array_len) {
        bits = array[array_i] << offset;
        top_bits = (bits & 0xFF00) >> 8;
        bottom_bits = bits & 0x00FF;

        packed->elems[packed_i] |= top_bits;
        packed->elems[packed_i + 1] |= bottom_bits;

        offset = 8 - (single_size - offset);
        if (offset < 0) {
            offset += 8;
            packed->elems[packed_i + 1] |= (array[array_i + 1] >> (16 - offset));
            packed_i++;
        }

        array_i++;
        packed_i++;
    }
}

void pack_into_bytes_var(u16 *array, size_t array_len, u8 *single_size, Array_u8 *packed) {
    size_t array_i, packed_i, packed_len;
    i8 offset;
    u8 top_bits, bottom_bits;
    u16 bits;

    offset = 16 - single_size[0];

    array_i = 0; packed_i = 0;
    while (array_i < array_len) {
        bits = array[array_i] << offset;
        top_bits = (bits & 0xFF00) >> 8;
        bottom_bits = bits & 0x00FF;

        packed->elems[packed_i] |= top_bits;
        packed->elems[packed_i + 1] |= bottom_bits;

        offset = 8 - (single_size[array_i + 1] - offset);
        if (offset < 0) {
            offset += 8;
            packed->elems[packed_i + 1] |= (array[array_i + 1] >> (16 - offset));
            packed_i++;
        }

        array_i++;
        packed_i++;
    }
}
