#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "encoding.h"
#include "luts.h"
#include "utils.h"

static inline bool is_numeric(char ch) {
    return ('0' <= ch) && (ch <= '9');
}

static bool is_alphanumeric(char ch) {
    if (is_numeric(ch))
        return true;
    if (('A' <= ch) && (ch <= 'Z'))
        return true;
    for (size_t i = 0; i < ALPHANUM_SPECIAL_NUM; i++)
        if (ch == alphanumeric_special[i])
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
    if (alphanumeric) return MODE_ALPHANUM;
    return MODE_BYTE;
}

u8 get_version(String text, Mode mode, ECC_Level ecc_level) {
    u8 version;
    for (version = 0; version < VERSION_NUM; version++)
        if (qrcode_capacity[version][ecc_level][mode] >= text.len)
            break;

    return version + 1;
}

static void encode_numeric(String text, u8 version, Array_u16 *encoding, u8 **word_lengths) {
    u8 digit1, digit2, digit3, leftover;

    leftover = text.len % NUMERIC_GROUP_SIZE;
    encoding->len = 2 + text.len / NUMERIC_GROUP_SIZE + (leftover > 0 ? 1 : 0);
    encoding->elems = malloc(encoding->len * sizeof(u16));
    *word_lengths = malloc(encoding->len * sizeof(u8));

    encoding->elems[0] = MODE_INDICATOR_NUMERIC;
    (*word_lengths)[0] = MODE_INDICATOR_LEN;
    encoding->elems[1] = text.len;
    if (version <= 9)       (*word_lengths)[1] = LEN_INDICATOR_NUMERIC_9_LEN;
    else if (version <= 26) (*word_lengths)[1] = LEN_INDICATOR_NUMERIC_26_LEN;
    else                    (*word_lengths)[1] = LEN_INDICATOR_NUMERIC_40_LEN;

    for (size_t i = 2; i < encoding->len - (leftover > 0 ? 1 : 0); i++) {
        digit1 = text.chars[NUMERIC_GROUP_SIZE * (i - 2)] - '0';
        digit2 = text.chars[NUMERIC_GROUP_SIZE * (i - 2) + 1] - '0';
        digit3 = text.chars[NUMERIC_GROUP_SIZE * (i - 2) + 2] - '0';

        encoding->elems[i] = 100 * digit1 + 10 * digit2 + digit3;

        if (digit1 == 0)
            if (digit2 == 0) (*word_lengths)[i] = NUMERIC_1WORD_LEN;
            else (*word_lengths)[i] = NUMERIC_2WORD_LEN;
        else (*word_lengths)[i] = NUMERIC_3WORD_LEN;
    }

    if (leftover == 2) {
        digit1 = text.chars[text.len - 2] - '0';
        digit2 = text.chars[text.len - 1] - '0';
        encoding->elems[encoding->len - 1] = 10 * digit1 + digit2;
        (*word_lengths)[encoding->len - 1] = NUMERIC_2WORD_LEN;
    }

    if (leftover == 1) {
        digit1 = text.chars[text.len - 1] - '0';
        encoding->elems[encoding->len - 1] = digit1;
        (*word_lengths)[encoding->len - 1] = NUMERIC_1WORD_LEN;
    }
}

static u8 encode_alphanumeric_char(char ch) {
    if (is_numeric(ch))
        return ch - '0';
    if (('A' <= ch) && (ch <= 'Z'))
        return ch - 'A' + 10;

    for (size_t i = 0; i < ALPHANUM_SPECIAL_NUM; i++)
        if (ch == alphanumeric_special[i])
            return i + 36;

    return ALPHANUM_SPECIAL_NUM + 36 + 1;
}

static void encode_alphanumeric(String text, u8 version, Array_u16 *encoding, u8 **word_lengths) {
    u8 left_char, right_char, leftover;

    leftover = text.len % ALPHANUM_GROUP_SIZE;

    encoding->len = 2 + text.len / ALPHANUM_GROUP_SIZE + leftover;
    encoding->elems = malloc(encoding->len * sizeof(u16));
    *word_lengths = malloc(encoding->len * sizeof(u8));

    encoding->elems[0] = MODE_INDICATOR_ALPHANUM;
    (*word_lengths)[0] = MODE_INDICATOR_LEN;
    encoding->elems[1] = text.len;
    if (version <= 9)       (*word_lengths)[1] = LEN_INDICATOR_ALPHANUM_9_LEN;
    else if (version <= 26) (*word_lengths)[1] = LEN_INDICATOR_ALPHANUM_26_LEN;
    else                    (*word_lengths)[1] = LEN_INDICATOR_ALPHANUM_40_LEN;

    for (size_t i = 2; i < encoding->len - leftover; i++) {
        left_char = encode_alphanumeric_char(text.chars[ALPHANUM_GROUP_SIZE * (i - 2)]);
        right_char = encode_alphanumeric_char(text.chars[ALPHANUM_GROUP_SIZE * (i - 2) + 1]);
        encoding->elems[i] = 45 * left_char + right_char;
        (*word_lengths)[i] = ALPHANUM_FULLWORD_LEN;
    }

    if (leftover == 1) {
        left_char = encode_alphanumeric_char(text.chars[text.len - 1]);
        encoding->elems[encoding->len - 1] = left_char;
        (*word_lengths)[encoding->len - 1] = ALPHANUM_HALFWORD_LEN;
    }
}

static void encode_byte(String text, u8 version, Array_u16 *encoding, u8 **word_lengths) {
    encoding->len = 2 + text.len;
    encoding->elems = malloc(text.len * sizeof(u16));
    *word_lengths = malloc(text.len * sizeof(u8));

    encoding->elems[0] = MODE_INDICATOR_BYTE;
    (*word_lengths)[0] = MODE_INDICATOR_LEN;
    encoding->elems[1] = text.len;
    if (version <= 9)       (*word_lengths)[1] = LEN_INDICATOR_BYTE_9_LEN;
    else if (version <= 26) (*word_lengths)[1] = LEN_INDICATOR_BYTE_26_LEN;
    else                    (*word_lengths)[1] = LEN_INDICATOR_BYTE_40_LEN;

    for (size_t i = 2; i < encoding->len; i++) {
        encoding->elems[i] = text.chars[i - 2];
        (*word_lengths)[i] = BYTE_WORD_LEN;
    }
}

void encode(String text, u8 version, Mode mode, ECC_Level ecc_level, Array_u16 *encoding, u8 **word_lengths) {
    switch (mode) {
        case MODE_NUMERIC: 
            encode_numeric(text, version, encoding, word_lengths); break;
        case MODE_ALPHANUM: 
            encode_alphanumeric(text, version, encoding, word_lengths); break;
        case MODE_BYTE: 
            encode_byte(text, version, encoding, word_lengths); break;
    }
}

Array_u8 packed_encoding(String text, Mode encoding_mode, u8 version, ECC_Level ecc_level) {
    Array_u8 packed;
    u8 *word_lengths;
    Array_u16 encoding;
    size_t bitstring_len, len_diff;

    packed.len = data_codeword_num[version-1][ecc_level];
    packed.elems = malloc(packed.len * sizeof(u8));

    encode(text, version, encoding_mode, ecc_level, &encoding, &word_lengths);

    bitstring_len = 0;
    for (size_t i = 0; i < encoding.len; i++) {
        bitstring_len += word_lengths[i];
    }

    len_diff = packed.len - bitstring_len;
    if (len_diff > 0) {
        bitstring_len += len_diff <= MAX_TERMINATOR_LEN ? len_diff : MAX_TERMINATOR_LEN;
    }
    bitstring_len += (8 - bitstring_len % 8);

    pack_into_bytes(encoding.elems, encoding.len, word_lengths, &packed);

    for (size_t i = bitstring_len / 8; i < packed.len; i++) {
        if (i % 2 == 0) packed.elems[i] = PAD_BYTE_LEFT;
        else            packed.elems[i] = PAD_BYTE_RIGHT; 
    }

    free(encoding.elems);
    free(word_lengths);
    return packed;
}
