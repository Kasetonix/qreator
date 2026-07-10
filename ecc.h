#ifndef ECC
#define ECC

#include "utils.h"
#include "encoding.h"

typedef struct Polynomial {
    u8 *coeff;
    u8 degree;
} Polynomial;

Polynomial create_generator_polynomial(u8 version, ECC_Level ecc_level);
u8 * generate_ec_codewords(Array_u8 *encoding, Polynomial *gen_pol);
Array_u8 interleaved_ec_codewords(Array_u8 *encoding, u8 version, ECC_Level ecc_level);

#endif
