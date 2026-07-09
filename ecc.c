#include <stdlib.h>
#include "ecc.h"
#include "encoding.h"
#include "luts.h"
#include "utils.h"

/* Multiplies a polynomial by a monomial of a form (x + mon_coeff) */
void mult_pol_by_mon(Polynomial *pol, u8 mon_coeff) {
    if (pol->degree == 0) {
        pol->degree = 1;
        pol->coeff[0] = mon_coeff;
        pol->coeff[1] = 1;

        return;
    }

    pol->degree++;
    for (u8 i = pol->degree; i > 0; i--)
        pol->coeff[i] = pol->coeff[i - 1];

    pol->coeff[0] = 0;
    for (u8 i = 0; i < pol->degree; i++)
        pol->coeff[i] ^= gf256_mult(pol->coeff[i+1], mon_coeff); 
}

Polynomial create_generator_polynomial(u8 version, ECC_Level ecc_level) {
    Polynomial gen_pol;
    u8 deg;

    deg = ec_codewords_per_block[version-1][ecc_level];
    gen_pol.degree = deg; 
    gen_pol.coeff = calloc(gen_pol.degree, sizeof(u8));
    

    gen_pol.degree = 0;
    for (u8 i = 0; i < deg; i++)
        mult_pol_by_mon(&gen_pol, gf256_antilog2_table[i]);

    return gen_pol;
}

#define STEP 1

Array_u8 generate_ec_codewords(Array_u8 *encoding, Polynomial *gen_pol) {
    Array_u8 ec_codewords;
    u8 gen_offset, steps, top_coeff;
    steps = encoding->len;

    // Create shifted message polynomial
    Polynomial msg_pol;
    msg_pol.degree = encoding->len - 1 + gen_pol->degree;
    msg_pol.coeff = calloc(msg_pol.degree + 1, sizeof(u8));
    for (u8 i = gen_pol->degree; i <= msg_pol.degree; i++) {
        msg_pol.coeff[i] = encoding->elems[msg_pol.degree - i];
    }

    ec_codewords.len = gen_pol->degree;
    ec_codewords.elems = malloc(ec_codewords.len * sizeof(u8));

    for (u8 step = 0; step < steps; step++) {
        gen_offset = msg_pol.degree - gen_pol->degree;
        for (u8 i = gen_offset; i <= msg_pol.degree; i++) {
            top_coeff = msg_pol.coeff[msg_pol.degree];
            msg_pol.coeff[i] ^= gf256_mult(gen_pol->coeff[i - gen_offset], top_coeff);
        }

        msg_pol.degree--;
    }

    for (u8 i = 0; i < ec_codewords.len; i++) {
        ec_codewords.elems[i] = msg_pol.coeff[msg_pol.degree - i];
    }

    free(msg_pol.coeff);
    return ec_codewords;
}
