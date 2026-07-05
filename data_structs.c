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
