#ifndef ENCODING
#define ENCODING

#include <stdint.h>

#include "utils.h"
#include "luts.h"

#define MODE_INDICATOR_LEN 4
#define MODE_INDICATOR_NUMERIC  0b0001
#define MODE_INDICATOR_ALPHANUM 0b0010
#define MODE_INDICATOR_BYTE     0b0100

#define LEN_INDICATOR_NUMERIC_9_LEN  10
#define LEN_INDICATOR_NUMERIC_26_LEN 12
#define LEN_INDICATOR_NUMERIC_40_LEN 14
#define LEN_INDICATOR_ALPHANUM_9_LEN  9
#define LEN_INDICATOR_ALPHANUM_26_LEN 11
#define LEN_INDICATOR_ALPHANUM_40_LEN 13
#define LEN_INDICATOR_BYTE_9_LEN  8
#define LEN_INDICATOR_BYTE_26_LEN 16
#define LEN_INDICATOR_BYTE_40_LEN 16

#define NUMERIC_GROUP_SIZE 3
#define NUMERIC_3WORD_LEN 10
#define NUMERIC_2WORD_LEN 7
#define NUMERIC_1WORD_LEN 4

#define ALPHANUM_GROUP_SIZE 2
#define ALPHANUM_FULLWORD_LEN 11
#define ALPHANUM_HALFWORD_LEN 6

#define BYTE_WORD_LEN 8

#define MAX_TERMINATOR_LEN 4

#define PAD_BYTE_LEFT  0b11101100
#define PAD_BYTE_RIGHT 0b00010001

typedef uint8_t u8;

typedef enum Mode { MODE_NUMERIC, MODE_ALPHANUM, MODE_BYTE } Mode;
typedef enum ECC_Level { ECC_L, ECC_M, ECC_Q, ECC_H } ECC_Level;

Mode get_encoding_mode(String text);
u8 get_version(String text, Mode mode, ECC_Level ecc_level);
void encode(String text, u8 version, Mode mode, ECC_Level ecc_level, Array_u16 *encoding, u8 **word_lengths);
Array_u8 packed_encoding(String text, ECC_Level ecc_level);

#endif
