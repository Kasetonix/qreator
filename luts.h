#ifndef LUTS
#define LUTS
#include <stdint.h>

#define ALPHANUMERIC_SPECIAL_NUM 9
#define VERSION_NUM 40
#define ECC_LEVEL_NUM 4
#define MODE_NUM 3

typedef uint16_t u16;
extern char alphanumeric_special[ALPHANUMERIC_SPECIAL_NUM];
extern u16 qrcode_capacity[VERSION_NUM][ECC_LEVEL_NUM][MODE_NUM];
extern u16 codeword_num[VERSION_NUM][ECC_LEVEL_NUM][2];

#endif
