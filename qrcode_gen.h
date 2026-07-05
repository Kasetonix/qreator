#ifndef ALGS
#define ALGS

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "encoding.h"

#define POSDET_MARKER_SIZE 7

typedef uint8_t u8;

typedef struct QR_Code {
    bool **matrix;
    size_t size;
    Mode mode;
    ECC_Level ecc_level;
} QR_Code;

void init_qrcode(QR_Code *qrcode, u8 version, Mode mode, ECC_Level ecc_level);
void alloc_qrcode(QR_Code *qrcode);

void create_qrcode_blueprint(QR_Code *qrcode);

#endif
