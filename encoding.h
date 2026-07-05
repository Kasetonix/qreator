#ifndef ENCODING
#define ENCODING

#include <stdint.h>

#include "utils.h"
#include "luts.h"

typedef uint8_t u8;

typedef enum Mode { MODE_NUMERIC, MODE_ALPHANUMERIC, MODE_BYTE } Mode;
typedef enum ECC_Level { ECC_L, ECC_M, ECC_Q, ECC_H } ECC_Level;

Mode get_encoding_mode(String text);
u8 get_version(String text, Mode mode, ECC_Level ecc_level);

#endif
