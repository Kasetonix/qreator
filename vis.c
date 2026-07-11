#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "vis.h"

void draw_qrcode_small(QR_Code *qrcode) {
    u8 **matrix; size_t size;
    matrix = qrcode->matrix;
    size = qrcode->size;

    if (DARK_MODE)
        fputs(EC_INVERSE, stdout);

    // TOP BORDER
    fputs(TOP_LEFT_BORDER, stdout);
    for (size_t x = 0; x < size + 2 * MARGIN; x++)
        fputs(HORIZONTAL_BORDER, stdout);
    fputs(TOP_RIGHT_BORDER, stdout);
    putchar('\n');

    // TOP VERTICAL MARGIN
    for (u8 i = 0; i < MARGIN / 2; i++) {
        fputs(VERTICAL_BORDER, stdout);
        for (size_t x = 0; x < size + 2 * MARGIN; x++)
            fputs(EMPTY_BLOCK, stdout);
        fputs(VERTICAL_BORDER, stdout);
        putchar('\n');
    }

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
        putchar('\n');
    }

    // BOTTOM BORDER
    fputs(BOTTOM_LEFT_BORDER, stdout);
    for (size_t x = 0; x < size + 2 * MARGIN; x++) {
        fputs(HORIZONTAL_BORDER, stdout);
    }
    fputs(BOTTOM_RIGHT_BORDER, stdout);
    putchar('\n');

    if (DARK_MODE)
        fputs(EC_RESET, stdout);
}
