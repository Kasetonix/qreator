#ifndef LUTS
#define LUTS
#include <stdint.h>

#define ALPHANUM_SPECIAL_NUM 9
#define VERSION_NUM 40
#define ECC_LEVEL_NUM 4
#define MODE_NUM 3

#define IND_G1_BLOCKS 1
#define IND_G1_CODEWORD_PER_BLOCK 2
#define IND_G2_BLOCKS 3
#define IND_G2_CODEWORD_PER_BLOCK 4

typedef uint8_t u8;
typedef uint16_t u16;
extern char alphanumeric_special[ALPHANUM_SPECIAL_NUM];
extern u16 qrcode_capacity[VERSION_NUM][ECC_LEVEL_NUM][MODE_NUM];
extern u16 data_codeword_num[VERSION_NUM][ECC_LEVEL_NUM];
extern u8 block_division[VERSION_NUM][ECC_LEVEL_NUM][4];
extern u8 gf256_log2_table[256];
extern u8 gf256_antilog2_table[256];

#endif
