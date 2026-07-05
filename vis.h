#ifndef VIS
#define VIS

#include "data_structs.h"

#define DRAW_CHAR      "  "
#define VERTICAL_BAR   '|'
#define HORIZONTAL_BAR '-'
#define CROSS_CHAR     '+'

#define MARGIN 2

#define EC_RESET   "\e[0m"
#define EC_INVERSE "\e[7m"

void draw_qrcode(QR_Code *qrcode);

#endif
