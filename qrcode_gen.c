#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "qrcode_gen.h"
#include "encoding.h"
#include "luts.h"
#include "utils.h"

void init_qrcode(QR_Code *qrcode, u8 version, Mode mode, ECC_Level ecc_level) {
    if (version >= 40)     error("Invalid QR code version.");
    if (mode > MODE_BYTE)  error("Invalid encoding mode.");
    if (ecc_level > ECC_H) error("Invalid ECC level.");

    qrcode->version = version;
    qrcode->size = 17 + 4 * (version + 1);
    qrcode->mode = mode;
    qrcode->ecc_level = ecc_level;
    qrcode->matrix = NULL;
}

void alloc_qrcode(QR_Code *qrcode) {
    qrcode->matrix = malloc(qrcode->size * sizeof(bool *));
    if (qrcode->matrix == NULL)
        error("Couldn't allocate qrcode array.");

    for (size_t y = 0; y < qrcode->size; y++) {
        qrcode->matrix[y] = calloc(qrcode->size, sizeof(bool));

        if (qrcode->matrix[y] == NULL)
            error("Couldn't allocate qrcode array.");
    }
}

static void add_position_detection(QR_Code *qrcode) {
    size_t size;
    u8 **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t y = 0; y < POSDET_MARKER_SIZE; y++) {
        for (size_t x = 0; x < POSDET_MARKER_SIZE; x++) {
            if (x == 0 || x == POSDET_MARKER_SIZE - 1 ||
                y == 0 || y == POSDET_MARKER_SIZE - 1) {
                matrix[y][x]            |= ON_MARKER;
                matrix[y][size - x - 1] |= ON_MARKER;
                matrix[size - y - 1][x] |= ON_MARKER;
            }
            else if (x != 1 && x != POSDET_MARKER_SIZE - 2 &&
                     y != 1 && y != POSDET_MARKER_SIZE - 2) {
                matrix[y][x]            |= ON_MARKER;
                matrix[y][size - x - 1] |= ON_MARKER;
                matrix[size - y - 1][x] |= ON_MARKER;
            }
        }
    }

    for (size_t y = 0; y < POSDET_MARKER_SIZE + 1; y++) {
        for (size_t x = 0; x < POSDET_MARKER_SIZE + 1; x++) {
            matrix[y][x]            |= TOUCH_MARKER;
            matrix[y][size - x - 1] |= TOUCH_MARKER;
            matrix[size - y - 1][x] |= TOUCH_MARKER;
        }
    }
}

static void add_timing_patterns(QR_Code *qrcode) {
    size_t size;
    u8 **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t i = POSDET_MARKER_SIZE + 1; i < size - POSDET_MARKER_SIZE - 1; i++) {
        matrix[POSDET_MARKER_SIZE - 1][i] |= TOUCH_MARKER;
        matrix[i][POSDET_MARKER_SIZE - 1] |= TOUCH_MARKER;
        if (i % 2 == 0) {
            matrix[POSDET_MARKER_SIZE - 1][i] |= ON_MARKER;
            matrix[i][POSDET_MARKER_SIZE - 1] |= ON_MARKER;
        }
    }
}

static bool is_ap_placable(QR_Code *qrcode, size_t y, size_t x) {
    u8 **matrix;
    matrix = qrcode->matrix;

    for (ssize_t dy = -ALPAT_MARKER_SIZE / 2; dy <= ALPAT_MARKER_SIZE / 2; dy++)
        for (ssize_t dx = -ALPAT_MARKER_SIZE / 2; dx <= ALPAT_MARKER_SIZE / 2; dx++)
            if ((matrix[y + dy][x + dx] & TOUCH_MARKER) == TOUCH_MARKER) return false;

    return true;
}

static void draw_ap(QR_Code *qrcode, size_t y, size_t x) {
    u8 **matrix;
    matrix = qrcode->matrix;

    matrix[y][x] |= ON_MARKER;

    for (ssize_t dy = -ALPAT_MARKER_SIZE / 2; dy <= ALPAT_MARKER_SIZE / 2; dy++) {
        for (ssize_t dx = -ALPAT_MARKER_SIZE / 2; dx <= ALPAT_MARKER_SIZE / 2; dx++) {
            matrix[y + dy][x + dx] |= TOUCH_MARKER;
            if (dx == -ALPAT_MARKER_SIZE / 2 || dx == ALPAT_MARKER_SIZE / 2)
                matrix[y + dy][x + dx] |= ON_MARKER;
            if (dy == -ALPAT_MARKER_SIZE / 2 || dy == ALPAT_MARKER_SIZE / 2)
                matrix[y + dy][x + dx] |= ON_MARKER;
        }
    }
}

static void add_alignment_patterns(QR_Code *qrcode) {
    u8 version;
    u8 *alpos;

    version = qrcode->version;
    alpos = alignment_pattern_positions[version];
    for (size_t i = 0; i < AP_POS; i++) {
        if (alpos[i] == 0) break;
        for (size_t j = 0; j < AP_POS; j++) {
            if (alpos[j] == 0) break;
            if (is_ap_placable(qrcode, alpos[j], alpos[i]))
                draw_ap(qrcode, alpos[j], alpos[i]);
        }
    }
}

static inline void add_dark_module(QR_Code *qrcode) {
    qrcode->matrix[qrcode->size - 8][8] |= ON_MARKER | TOUCH_MARKER;
}

static void touch_format_area(QR_Code *qrcode) {
    u8 **matrix;
    size_t size;

    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t y = 0; y < POSDET_MARKER_SIZE + 2; y++)
        matrix[y][POSDET_MARKER_SIZE + 1] |= TOUCH_MARKER;

    for (size_t y = size - POSDET_MARKER_SIZE - 1; y < size; y++)
        matrix[y][POSDET_MARKER_SIZE + 1] |= TOUCH_MARKER;

    for (size_t x = 0; x < POSDET_MARKER_SIZE + 2; x++)
        matrix[POSDET_MARKER_SIZE + 1][x] |= TOUCH_MARKER;

    for (size_t x = size - POSDET_MARKER_SIZE - 1; x < size; x++)
        matrix[POSDET_MARKER_SIZE + 1][x] |= TOUCH_MARKER;
}

static void touch_version_area(QR_Code *qrcode) {
    u8 **matrix;
    size_t size;

    matrix = qrcode->matrix;
    size = qrcode->size;
    
    for (size_t y = 0; y < POSDET_MARKER_SIZE - 1; y++)
        for (size_t x = 0; x < VER_WIDTH; x++)
            matrix[y][size - POSDET_MARKER_SIZE - 2 - x] |= TOUCH_MARKER;

    for (size_t y = 0; y < VER_WIDTH; y++)
        for (size_t x = 0; x < POSDET_MARKER_SIZE - 1; x++)
            matrix[size - POSDET_MARKER_SIZE - 2 - y][x] |= TOUCH_MARKER;
}

void create_qrcode_blueprint(QR_Code *qrcode) {
    add_position_detection(qrcode);
    add_alignment_patterns(qrcode);
    add_timing_patterns(qrcode);
    add_dark_module(qrcode);
    touch_format_area(qrcode);
    if (qrcode->version >= 6)
        touch_version_area(qrcode);
}

void remove_touch_markers(QR_Code *qrcode) {
    for (size_t y = 0; y < qrcode->size; y++)
        for (size_t x = 0; x < qrcode->size; x++)
            qrcode->matrix[y][x] >>= 1;
}
