#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vis.h"

static char * monostring(char ch, size_t len) {
    char *str = malloc(len * sizeof(char));
    if (str == NULL)
        error("Couldn't allocate memory for a monostring");

    for (size_t i = 0; i < len; i++)
        str[i] = ch;
    return str;
}

static char * monostring_end(char ch, size_t len, char end_char, size_t end_len) {
    char *str = malloc((len + 2 * end_len) * sizeof(char));
    if (str == NULL)
        error("Couldn't allocate memory for a monostring");

    for (size_t i = 0; i < end_len; i++)
        str[i] = end_char;
    for (size_t i = end_len; i < len + end_len; i++)
        str[i] = ch;
    for (size_t i = end_len + len; i < len + 2 * end_len; i++)
        str[i] = end_char;

    return str;
}

void draw_qrcode_simple(QR_Code *qrcode) {
    bool prev_state, state;
    size_t size = qrcode->size;
    char *horizontal_bar = monostring_end(SIMPLE_HORIZONTAL_BAR, 2 * (size + 2 * MARGIN), CROSS_CHAR, 1);
    char *margin_vert = monostring_end(' ', 2 * (size + 2 * MARGIN), SIMPLE_VERTICAL_BAR, 1);
    char *margin_str = monostring(' ', 2 * MARGIN);
    puts(horizontal_bar);

    for (size_t i = 0; i < MARGIN; i++)
        puts(margin_vert);

    prev_state = qrcode->matrix[0][0];
    for (size_t y = 0; y < size; y++) {
        putchar(SIMPLE_VERTICAL_BAR);
        fputs(margin_str, stdout);
        fputs(prev_state ? EC_INVERSE : EC_RESET, stdout);

        for (size_t x = 0; x < size; x++) {
            state = qrcode->matrix[y][x];
            if (prev_state != state) {
                if (state) fputs(EC_INVERSE, stdout);
                else       fputs(EC_RESET,   stdout);
            }
            fputs(DRAW_CHAR, stdout);
            prev_state = state;
        }

        fputs(EC_RESET, stdout);
        fputs(margin_str, stdout);
        putchar(SIMPLE_VERTICAL_BAR);
        putchar('\n');
    }

    for (size_t i = 0; i < MARGIN; i++)
        puts(margin_vert);
    puts(horizontal_bar);

    free(horizontal_bar);
    free(margin_vert);
    free(margin_str);
}

void draw_qrcode_small(QR_Code *qrcode) {
    bool **matrix; size_t size;
    matrix = qrcode->matrix;
    size = qrcode->size;

    // TOP BORDER
    fputs(TOP_LEFT_BORDER, stdout);
    for (size_t x = 0; x < size + 2 * MARGIN; x++) {
        fputs(HORIZONTAL_BORDER, stdout);
    }
    fputs(TOP_RIGHT_BORDER, stdout);
    putchar('\n');

    // TOP VERTICAL MARGIN
    for (u8 i = 0; i < MARGIN / 2; i++) {
        fputs(VERTICAL_BORDER, stdout);
        for (size_t x = 0; x < size + 2 * MARGIN; x++) fputs(EMPTY_BLOCK, stdout);
        fputs(VERTICAL_BORDER, stdout);
    }
    putchar('\n');

    // QR CODE
    for (size_t y = 0; y < size - 1; y += 2) {
        fputs(VERTICAL_BORDER, stdout);
        for (u8 i = 0; i < MARGIN; i++) fputs(EMPTY_BLOCK, stdout);

        for (size_t x = 0; x < size; x++) {
            if (matrix[y][x] && matrix[y+1][x]) fputs(FULL_BLOCK, stdout);
            else if (matrix[y][x])              fputs(TOP_HALF_BLOCK, stdout);
            else if (matrix[y+1][x])            fputs(BOTTOM_HALF_BLOCK, stdout);
            else                                fputs(EMPTY_BLOCK, stdout);
        }

        for (u8 i = 0; i < MARGIN; i++) fputs(EMPTY_BLOCK, stdout);
        fputs(VERTICAL_BORDER, stdout);
        putchar('\n');
    }

    // LAST LINE OF QR CODE
    fputs(VERTICAL_BORDER, stdout);
    for (u8 i = 0; i < MARGIN; i++) fputs(EMPTY_BLOCK, stdout);

    for (size_t x = 0; x < size; x++) {
        if (matrix[size - 1][x]) fputs(TOP_HALF_BLOCK, stdout);
        else                     fputs(EMPTY_BLOCK, stdout);
    }

    for (u8 i = 0; i < MARGIN; i++) fputs(EMPTY_BLOCK, stdout);
    fputs(VERTICAL_BORDER, stdout);
    putchar('\n');

    // BOTTOM VERTICAL MARGIN
    for (u8 i = 0; i < MARGIN / 2; i++) {
        fputs(VERTICAL_BORDER, stdout);
        for (size_t x = 0; x < size + 2 * MARGIN; x++) fputs(EMPTY_BLOCK, stdout);
        fputs(VERTICAL_BORDER, stdout);
    }
    putchar('\n');

    // BOTTOM BORDER
    fputs(BOTTOM_LEFT_BORDER, stdout);
    for (size_t x = 0; x < size + 2 * MARGIN; x++) {
        fputs(HORIZONTAL_BORDER, stdout);
    }
    fputs(BOTTOM_RIGHT_BORDER, stdout);
    putchar('\n');
}
