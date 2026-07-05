#include <stdbool.h>
#include "encoding.h"
#include "luts.h"

bool is_numeric(char ch) {
    return ('0' <= ch) && (ch <= '9');
}

bool is_alphanumeric(char ch) {
    if (is_numeric(ch))
        return true;
    if (('A' <= ch) && (ch <= 'Z'))
        return true;
    if (ch == ' ' || ch == '$' || ch == '%' ||
        ch == '*' || ch == '+' || ch == '-' ||
        ch == '.' || ch == '/' || ch == ':')
        return true;
    return false;
}

Mode get_encoding_mode(String text) {
    char ch;
    bool numeric = true, alphanumeric = true;
    
    for (size_t i = 0; i < text.len; i++) {
        ch = text.chars[i];

        if ((32 > ch) || (ch > 126))
            error("Unsupported characters in text.");

        if (alphanumeric && !is_alphanumeric(ch)) {
            alphanumeric = false;
            numeric = false;
        }

        if (numeric && !is_numeric(ch))
            numeric = false;
    }

    if (numeric) return MODE_NUMERIC;
    if (alphanumeric) return MODE_ALPHANUMERIC;
    return MODE_BYTE;
}

u8 get_version(String text, Mode mode, ECC_Level ecc_level) {
    u8 version = 0;
    for (; version < VERSION_NUM; version++)
        if (qrcode_capacity[version][ecc_level][mode] >= text.len)
            break;

    return version + 1;
}
