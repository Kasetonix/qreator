#ifndef ALGS
#define ALGS

#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "encoding.h"

#define TOUCH_MARKER 1
#define ON_MARKER 2

#define POSDET_MARKER_SIZE 7
#define ALPAT_MARKER_SIZE 5
#define VER_WIDTH 3

typedef struct QR_Code {
    u8 **matrix;
    size_t size;
    u8 version;
    Mode mode;
    ECC_Level ecc_level;
} QR_Code;

void init_qrcode(QR_Code *qrcode, u8 version, Mode mode, ECC_Level ecc_level);
void alloc_qrcode(QR_Code *qrcode);
void create_qrcode_blueprint(QR_Code *qrcode);
void remove_touch_markers(QR_Code *qrcode);

#endif
