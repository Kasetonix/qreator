#ifndef ALGS
#define ALGS

#include <stdlib.h>
#include <stdbool.h>

#include "utils.h"
#include "encoding.h"

#define BLUEPRINT_MARKER 1
#define ON_MARKER 2

#define POSDET_MARKER_SIZE 7
#define ALPAT_MARKER_SIZE 5
#define VER_WIDTH 3

#define MASK_NUMBER 8

typedef struct QR_Code {
    u8 **matrix;
    size_t size;
    u8 version;
    Mode mode;
    ECC_Level ecc_level;
} QR_Code;

typedef struct Pos {
    ssize_t y;
    ssize_t x;
} Pos;

void init_qrcode(QR_Code *qrcode, u8 version, Mode mode, ECC_Level ecc_level);
void alloc_qrcode(QR_Code *qrcode);
void copy_qrcode_matrix(QR_Code *qrcode, QR_Code *copy);
QR_Code copy_qrcode_params(QR_Code *qrcode);
void create_qrcode_blueprint(QR_Code *qrcode);
void add_codewords(QR_Code *qrcode, Array_u8 codewords);
void remove_touch_markers(QR_Code *qrcode);
void apply_mask(QR_Code *qrcode, u8 mask_number);
u32 calculate_penalty(QR_Code *qrcode);

#endif
