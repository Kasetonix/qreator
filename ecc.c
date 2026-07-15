#include "ecc.h"

static inline u8 gf256_mult(u8 a, u8 b) {
    return gf256_antilog2_table[ (gf256_log2_table[a] + gf256_log2_table[b]) % 255 ];
}

/* Multiplies a polynomial by a monomial of a form (x + mon_coeff) */
static void mult_pol_by_mon(Polynomial *pol, u8 mon_coeff) {
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
        pol->coeff[i] ^= gf256_mult(pol->coeff[i + 1], mon_coeff);
}

static Polynomial create_generator_polynomial(u8 version, ECC_Level ecc_level) {
    Polynomial gen_pol;
    u8 deg;

    deg = ec_codewords_per_block[version][ecc_level];
    gen_pol.degree = deg;
    gen_pol.coeff = calloc(gen_pol.degree, sizeof(u8));
    if (gen_pol.coeff == NULL)
        error("Couldn't allocate memory for generator polynomial.");

    gen_pol.degree = 0;
    for (u8 i = 0; i < deg; i++)
        mult_pol_by_mon(&gen_pol, gf256_antilog2_table[i]);

    return gen_pol;
}

static u8 * generate_ec_codewords(Array_u8 *encoding, Polynomial *gen_pol) {
    u8 *ec_codewords;
    u8 gen_offset, steps, top_coeff;

    steps = encoding->len;

    // Create shifted message polynomial
    Polynomial msg_pol;
    msg_pol.degree = encoding->len - 1 + gen_pol->degree;
    msg_pol.coeff = calloc(msg_pol.degree + 1, sizeof(u8));
    if (msg_pol.coeff == NULL)
        error("Couldn't allocate memory for message polynomial.");

    for (u8 i = gen_pol->degree; i <= msg_pol.degree; i++) {
        msg_pol.coeff[i] = encoding->elems[msg_pol.degree - i];
    }

    ec_codewords = malloc(gen_pol->degree * sizeof(u8));
    if (ec_codewords == NULL)
        error("Couldn't allocate memory for error correction codewords.");

    for (u8 step = 0; step < steps; step++) {
        gen_offset = msg_pol.degree - gen_pol->degree;
        for (u8 i = gen_offset; i <= msg_pol.degree; i++) {
            top_coeff = msg_pol.coeff[msg_pol.degree];
            msg_pol.coeff[i] ^= gf256_mult(gen_pol->coeff[i - gen_offset], top_coeff);
        }

        msg_pol.degree--;
    }

    for (u8 i = 0; i < gen_pol->degree; i++) {
        ec_codewords[i] = msg_pol.coeff[msg_pol.degree - i];
    }

    free(msg_pol.coeff);
    return ec_codewords;
}

static Array_u8 interleaved_ec_codewords(Array_u8 *data_codewords, u8 version, ECC_Level ecc_level) {
    Array_u8 interleaved_eccs, dc_block;
    u8 **eccs;
    size_t block_number, eccs_per_block;
    u8 *block_div;
    Polynomial gen_pol;

    gen_pol = create_generator_polynomial(version, ecc_level);

    block_div = block_division[version][ecc_level];
    block_number = block_div[IND_G1_BLOCKS] + block_div[IND_G2_BLOCKS];
    eccs_per_block = ec_codewords_per_block[version][ecc_level];

    interleaved_eccs.len = block_number * eccs_per_block;
    interleaved_eccs.elems = malloc(interleaved_eccs.len * sizeof(u8));
    if (interleaved_eccs.elems == NULL)
        error("Couldn't allocate memory for interleaved error correction codewords.");

    eccs = malloc(block_number * sizeof(u8 *));
    if (eccs == NULL)
        error("Couldn't allocate memory for error correction codeword array for interleaving.");

    dc_block.elems = data_codewords->elems;
    dc_block.len = block_div[IND_G1_CODEWORD_PER_BLOCK];

    for (size_t i = 0; i < block_div[IND_G1_BLOCKS]; i++) {
        eccs[i] = generate_ec_codewords(&dc_block, &gen_pol);
        dc_block.elems += dc_block.len;
    }

    dc_block.len = block_div[IND_G2_CODEWORD_PER_BLOCK];
    for (size_t i = block_div[IND_G1_BLOCKS]; i < block_number; i++) {
        eccs[i] = generate_ec_codewords(&dc_block, &gen_pol);
        dc_block.elems += dc_block.len;
    }

    for (size_t i = 0; i < eccs_per_block; i++)
        for (size_t j = 0; j < block_number; j++)
            interleaved_eccs.elems[i * block_number + j] = eccs[j][i];

    for (size_t i = 0; i < block_number; i++)
        free(eccs[i]);
    free(eccs);
    free(gen_pol.coeff);
    return interleaved_eccs;
}

static Array_u8 interleaved_data_codewords(Array_u8 *data_codewords, u8 version, ECC_Level ecc_level) {
    Array_u8 interleaved_dcs;
    size_t i, block_number, col_number;
    u8 *block_div, *dc_ptr;
    
    block_div = block_division[version][ecc_level];
    block_number = block_div[IND_G1_BLOCKS] + block_div[IND_G2_BLOCKS];
    if (block_div[IND_G1_CODEWORD_PER_BLOCK] > block_div[IND_G2_CODEWORD_PER_BLOCK])
        col_number = block_div[IND_G1_CODEWORD_PER_BLOCK];
    else
        col_number = block_div[IND_G2_CODEWORD_PER_BLOCK];

    interleaved_dcs.len = data_codewords->len;
    interleaved_dcs.elems = malloc(interleaved_dcs.len * sizeof(u8));
    if (interleaved_dcs.elems == NULL)
        error("Couldn't allocate memory for interleaved data codewords.");

    i = 0;
    for (size_t col = 0; col < col_number; col++) {
        dc_ptr = &data_codewords->elems[col];
        for (size_t block_ind = 0; block_ind < block_div[IND_G1_BLOCKS]; block_ind++) {
            if (col < block_div[IND_G1_CODEWORD_PER_BLOCK]) {
                interleaved_dcs.elems[i] = *dc_ptr;
                i++;
            }
            dc_ptr += block_div[IND_G1_CODEWORD_PER_BLOCK];
        }

        for (size_t block_ind = block_div[IND_G1_BLOCKS]; block_ind < block_number; block_ind++) {
            if (col < block_div[IND_G2_CODEWORD_PER_BLOCK]) {
                interleaved_dcs.elems[i] = *dc_ptr;
                i++;
            }
            dc_ptr += block_div[IND_G2_CODEWORD_PER_BLOCK];
        }
    }

    return interleaved_dcs;
}

Array_u8 final_codewords(Array_u8 *data_codewords, u8 version, ECC_Level ecc_level) {
    Array_u8 codewords, iecc, idc;

    iecc = interleaved_ec_codewords(data_codewords, version, ecc_level);
    idc  = interleaved_data_codewords(data_codewords, version, ecc_level);

    codewords.len = iecc.len + idc.len;
    codewords.elems = calloc(codewords.len, sizeof(u8));
    if (codewords.elems == NULL)
        error("Couldn't allocate memory for final codeword array.");

    for (size_t i = 0; i < idc.len; i++)
        codewords.elems[i] = idc.elems[i];
    for (size_t i = 0; i < iecc.len; i++)
        codewords.elems[i + idc.len] = iecc.elems[i];

    free(iecc.elems);
    free(idc.elems);
    return codewords;
}
