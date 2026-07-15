#ifndef ALGS
#define ALGS

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "utils.h"
#include "encoding.h"
#include "luts.h"

#define BLUEPRINT_MARKER 1
#define ON_MARKER 2

#define VERSION_MARKER_THRESHOLD 6

#define POSDET_MARKER_SIZE 7
#define ALPAT_MARKER_SIZE 5
#define VER_LENGTH 6
#define VER_WIDTH 3

#define MASK_NUMBER 8

#define FORMAT_LEN 15
#define FORMAT_ECC_LEN 10
#define FORMAT_ECC_GEN_POL 1335 // 0b10100110111 
#define FORMAT_ECC_MASK 21522 // 0b101010000010010

#define VERSION_ECC_LEN 12
#define VERSION_ECC_GEN_POL 7973 // 0b1111100100101

typedef struct QR_Code {
    u8 **matrix;
    size_t size;
    u8 version;
    Mode mode;
    ECC_Level ecc_level;
    u8 mask;
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
void apply_mask(QR_Code *qrcode, u8 mask_number);
u32 calculate_penalty(QR_Code *qrcode);
u16 create_format_string(QR_Code *qrcode);
u32 create_version_string(QR_Code *qrcode);
void add_format_string(QR_Code *qrcode);
void add_version_string(QR_Code *qrcode);

#endif
