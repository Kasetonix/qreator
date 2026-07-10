#ifndef ECC
#define ECC

#include "utils.h"
#include "encoding.h"

typedef struct Polynomial {
    u8 *coeff;
    u8 degree;
} Polynomial;

Array_u8 interleaved_ec_codewords(Array_u8 *encoding, u8 version, ECC_Level ecc_level);

#endif
