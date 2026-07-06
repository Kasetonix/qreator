#ifndef ENCODING
#define ENCODING

#include <stdint.h>

#include "utils.h"
#include "luts.h"

#define NUMERIC_GROUP_SIZE 3
#define NUMERIC_3WORD_LEN 10
#define NUMERIC_2WORD_LEN 7
#define NUMERIC_1WORD_LEN 4

#define ALPHANUMERIC_GROUP_SIZE 2
#define ALPHANUMERIC_FULLWORD_LEN 11
#define ALPHANUMERIC_HALFWORD_LEN 6

typedef uint8_t u8;

typedef enum Mode { MODE_NUMERIC, MODE_ALPHANUMERIC, MODE_BYTE } Mode;
typedef enum ECC_Level { ECC_L, ECC_M, ECC_Q, ECC_H } ECC_Level;

Mode get_encoding_mode(String text);
u8 get_version(String text, Mode mode, ECC_Level ecc_level);
Array_u8 encode(String text, Mode mode);

#endif
