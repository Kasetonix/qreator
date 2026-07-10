#ifndef ECC
#define ECC

#include "utils.h"
#include "encoding.h"

typedef struct Polynomial {
    u8 *coeff;
    u8 degree;
} Polynomial;

Array_u8 final_codewords(Array_u8 *data_codewords, u8 version, ECC_Level ecc_level);

#endif
