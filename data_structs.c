#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"
#include "data_structs.h"

void read(String *str) {
    size_t buflen;
    ssize_t len;
    str->chars = NULL;
    str->len = 0;
    len = getline(&str->chars, &buflen, stdin);
    str->len = len - 1;
    *(str->chars + str->len) = '\0';
}

void init_qrcode(QR_Code *qrcode, u8 version, ECC_Level ecc_level) {
    if (version > 40)  error("Invalid QR code version.");
    if (ecc_level > 4) error("Invalid ECC level."); 

    qrcode->size = 17 + 4 * version;
    qrcode->ecc_level = ecc_level;
    qrcode->matrix = NULL;
}

static void add_position_detection(QR_Code *qrcode) {
    size_t size;
    bool **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;
    
    // top-left corner
    for (size_t y = 0; y < POSDET_MARKER_SIZE; y++)
        for (size_t x = 0; x < POSDET_MARKER_SIZE; x++)
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

void create_qrcode_blueprint(QR_Code *qrcode) {
    add_position_detection(qrcode);
    add_timing_patterns(qrcode);
}
