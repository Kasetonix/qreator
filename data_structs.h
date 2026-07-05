#ifndef DATA_STRUCTS
#define DATA_STRUCTS

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define POSDET_MARKER_SIZE 7

typedef uint8_t u8;
typedef enum ECC_Level { L, M, Q, H } ECC_Level;

typedef struct String {
    char *chars;
    size_t len;
} String;

typedef struct QR_Code {
    bool **matrix;
    size_t size;
    ECC_Level ecc_level;
} QR_Code;

void read(String *str);
void init_qrcode(QR_Code *qrcode, u8 version, ECC_Level ecc_level);
void alloc_qrcode(QR_Code *qrcode);

#endif
