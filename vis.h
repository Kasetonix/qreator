#ifndef VIS
#define VIS

#include "utils.h"
#include "qrcode_gen.h"

#define DARK_MODE true

#define DRAW_CHAR      "  "
#define SIMPLE_VERTICAL_BAR   '|'
#define SIMPLE_HORIZONTAL_BAR '-'
#define CROSS_CHAR     '+'

#define FULL_BLOCK "█"
#define BOTTOM_HALF_BLOCK "▄"
#define TOP_HALF_BLOCK "▀"
#define EMPTY_BLOCK " "
#define HORIZONTAL_BORDER "─"
#define VERTICAL_BORDER "│"
#define TOP_LEFT_BORDER "┌"
#define TOP_RIGHT_BORDER "┐"
#define BOTTOM_LEFT_BORDER "└"
#define BOTTOM_RIGHT_BORDER "┘"

#define MARGIN 2

#define EC_RESET   "\033[0m"
#define EC_INVERSE "\033[7m"

void draw_qrcode_small(QR_Code *qrcode);

#endif
