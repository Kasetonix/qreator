#ifndef ECC
#define ECC

#include "utils.h"
#include "encoding.h"

typedef struct Polynomial {
    u8 *coeff;
    u8 degree;
} Polynomial;

void mult_pol_by_mon(Polynomial *pol, u8 mon_coeff);
Polynomial create_generator_polynomial(u8 version, ECC_Level ecc_level);
Array_u8 generate_ec_codewords(Array_u8 *encoding, Polynomial *gen_pol);

#endif
