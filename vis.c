#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vis.h"

static char * monostring(char ch, size_t len) {
    char *str = malloc(len * sizeof(char));
    for (size_t i = 0; i < len; i++)
        str[i] = ch;
    return str;
}

static char * monostring_end(char ch, size_t len, char end_char, size_t end_len) {
    char *str = malloc((len + 2 * end_len) * sizeof(char));
    for (size_t i = 0; i < end_len; i++)
        str[i] = end_char;
    for (size_t i = end_len; i < len + end_len; i++)
        str[i] = ch;
    for (size_t i = end_len + len; i < len + 2 * end_len; i++)
        str[i] = end_char;

    return str;
}

void draw_qrcode(QR_Code *qrcode) {
    bool prev_state, state;
    size_t size = qrcode->size;
    char *horizontal_bar = monostring_end(HORIZONTAL_BAR, 2 * (size + 2 * MARGIN), CROSS_CHAR, 1);
    char *margin_vert = monostring_end(' ', 2 * (size + 2 * MARGIN), VERTICAL_BAR, 1);
    char *margin_str = monostring(' ', 2 * MARGIN);
    puts(horizontal_bar);

    for (size_t i = 0; i < MARGIN; i++)
        puts(margin_vert);

    prev_state = qrcode->matrix[0][0];
    for (size_t y = 0; y < size; y++) {
        putchar(VERTICAL_BAR);
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
        putchar(VERTICAL_BAR);
        putchar('\n');
    }

    for (size_t i = 0; i < MARGIN; i++)
        puts(margin_vert);
    puts(horizontal_bar);
}
