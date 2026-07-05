#include "qrcode_gen.h"

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
