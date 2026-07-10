#ifndef LUTS
#define LUTS

#include "utils.h"

#define ALPHANUM_SPECIAL_NUM 9
#define VERSION_NUM 40
#define ECC_LEVEL_NUM 4
#define MODE_NUM 3

#define AP_POS 7

#define IND_G1_BLOCKS 0
#define IND_G1_CODEWORD_PER_BLOCK 1
#define IND_G2_BLOCKS 2
#define IND_G2_CODEWORD_PER_BLOCK 3

extern char alphanumeric_special[ALPHANUM_SPECIAL_NUM];
extern u16 qrcode_capacity[VERSION_NUM][ECC_LEVEL_NUM][MODE_NUM];
extern u16 data_codeword_num[VERSION_NUM][ECC_LEVEL_NUM];
extern u8 ec_codewords_per_block[VERSION_NUM][ECC_LEVEL_NUM];
extern u8 block_division[VERSION_NUM][ECC_LEVEL_NUM][4];
extern u8 gf256_log2_table[256];
extern u8 gf256_antilog2_table[256];
extern u8 remainder_bits[VERSION_NUM];
extern u8 alignment_pattern_positions[VERSION_NUM][AP_POS];

#endif
