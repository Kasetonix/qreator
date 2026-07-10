#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "qrcode_gen.h"
#include "encoding.h"
#include "utils.h"

void init_qrcode(QR_Code *qrcode, u8 version, Mode mode, ECC_Level ecc_level) {
    if (version >= 40)     error("Invalid QR code version.");
    if (mode > MODE_BYTE)  error("Invalid encoding mode.");
    if (ecc_level > ECC_H) error("Invalid ECC level.");

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
        qrcode->matrix[y] = malloc(qrcode->size * sizeof(bool));
        if (qrcode->matrix[y] == NULL)
            error("Couldn't allocate qrcode array.");
    }
}

static void add_position_detection(QR_Code *qrcode) {
    size_t size;
    bool **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t y = 0; y < POSDET_MARKER_SIZE; y++) {
        for (size_t x = 0; x < POSDET_MARKER_SIZE; x++) {
            if (x == 0 || x == POSDET_MARKER_SIZE - 1 ||
                y == 0 || y == POSDET_MARKER_SIZE - 1) {
                matrix[y][x] = true;
                matrix[y][x + size - POSDET_MARKER_SIZE] = true;
                matrix[y + size - POSDET_MARKER_SIZE][x] = true;
            }
            else if (x != 1 && x != POSDET_MARKER_SIZE - 2 &&
                     y != 1 && y != POSDET_MARKER_SIZE - 2) {

                matrix[y][x] = true;
                matrix[y][x + size - POSDET_MARKER_SIZE] = true;
                matrix[y + size - POSDET_MARKER_SIZE][x] = true;
            }
        }
    }
}

static void add_timing_patterns(QR_Code *qrcode) {
    size_t size;
    bool **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t i = POSDET_MARKER_SIZE + 1; i < size - POSDET_MARKER_SIZE - 1; i += 2) {
        matrix[POSDET_MARKER_SIZE - 1][i] = true;
        matrix[i][POSDET_MARKER_SIZE - 1] = true;
    }
}

void create_qrcode_blueprint(QR_Code *qrcode) {
    add_position_detection(qrcode);
    add_timing_patterns(qrcode);
}
