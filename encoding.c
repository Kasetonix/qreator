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

size_t get_base_encoding_len(String text, Mode mode) {
    if (mode == MODE_ALPHANUM) {
        size_t packed_len_bits;

        packed_len_bits = ALPHANUM_FULLWORD_LEN * (text.len / ALPHANUM_GROUP_SIZE)
                        + (text.len % ALPHANUM_GROUP_SIZE) * 6;

        if (packed_len_bits % 8 != 0)
            return packed_len_bits / 8 + 1;
        else
            return packed_len_bits / 8;
    }

    if (mode == MODE_NUMERIC) {
        size_t bit_len = 0;
        for (size_t i = 0; i <= text.len - NUMERIC_GROUP_SIZE; i += NUMERIC_GROUP_SIZE) {
            if (text.chars[i] == '0')
                if (text.chars[i+1] == '0')
                    bit_len += NUMERIC_1WORD_LEN;
                else
                    bit_len += NUMERIC_2WORD_LEN;
            else
                bit_len += NUMERIC_3WORD_LEN;
        }

        if (text.len % NUMERIC_GROUP_SIZE == 1)
            bit_len += NUMERIC_1WORD_LEN;
        else if (text.len % NUMERIC_GROUP_SIZE == 2)
            bit_len += NUMERIC_2WORD_LEN;

        if (bit_len % 8 != 0) return bit_len / 8 + 1;
        else                  return bit_len / 8;
    }

    // mode == MODE_BYTE
    return text.len;
}

static Array_u8 encode_numeric(String text) {
    Array_u8 encoding;
    u16 *initial_encoding;
    u8 *word_lengths;
    u8 right_padding, digit1, digit2, digit3;
    size_t ie_len;

    ie_len = text.len / NUMERIC_GROUP_SIZE;

    switch (text.len % NUMERIC_GROUP_SIZE) {
        case 0: right_padding = 0; break;
        case 1: right_padding = NUMERIC_1WORD_LEN; break;
        case 2: right_padding = NUMERIC_2WORD_LEN; break;
    }

    initial_encoding = malloc(ie_len * sizeof(u16));
    word_lengths = malloc(ie_len * sizeof(u8));

    for (size_t i = 0; i < ie_len; i++) {
        digit1 = text.chars[NUMERIC_GROUP_SIZE * i] - '0';
        digit2 = text.chars[NUMERIC_GROUP_SIZE * i + 1] - '0';
        digit3 = text.chars[NUMERIC_GROUP_SIZE * i + 2] - '0';

        initial_encoding[i] = 100 * digit1 + 10 * digit2 + digit3;

        if (digit1 == 0)
            if (digit2 == 0) word_lengths[i] = NUMERIC_1WORD_LEN;
            else word_lengths[i] = NUMERIC_2WORD_LEN;
        else word_lengths[i] = NUMERIC_3WORD_LEN;
    }

    encoding.len = get_base_encoding_len(text, MODE_NUMERIC);
    encoding.elements = calloc(encoding.len, sizeof(u8));
    pack_into_bytes_var(initial_encoding, ie_len, word_lengths, &encoding);

    free(initial_encoding);
    return encoding;
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

static Array_u8 encode_alphanumeric(String text) {
    Array_u8 encoding;
    u16 *initial_encoding;
    u8 left_char, right_char, right_padding, free_bits, offset;
    size_t ie_len;

    ie_len = text.len / ALPHANUM_GROUP_SIZE;
    if (text.len % ALPHANUM_GROUP_SIZE != 0)
        right_padding = ALPHANUM_HALFWORD_LEN;
    else
        right_padding = 0;

    initial_encoding = malloc(ie_len * sizeof(u16));

    for (size_t i = 0; i < ie_len; i++) {
        left_char = encode_alphanumeric_char(text.chars[ALPHANUM_GROUP_SIZE * i]);
        right_char = encode_alphanumeric_char(text.chars[ALPHANUM_GROUP_SIZE * i + 1]);
        initial_encoding[i] = 45 * left_char + right_char;
    }

    encoding.len = get_base_encoding_len(text, MODE_ALPHANUM);
    encoding.elements = calloc(encoding.len, sizeof(u8));

    pack_into_bytes(initial_encoding, ie_len, ALPHANUM_FULLWORD_LEN, &encoding);

    if (text.len % ALPHANUM_GROUP_SIZE != 0) {
        left_char = encode_alphanumeric_char(text.chars[text.len - 1]);
        free_bits = (8 * encoding.len) - (ie_len * ALPHANUM_FULLWORD_LEN);
        offset = free_bits - ALPHANUM_HALFWORD_LEN;
        encoding.elements[encoding.len - 1] |= left_char << offset;
        if (free_bits > 8)
            encoding.elements[encoding.len - 2] |= left_char >> (8 - offset);
    }

    free(initial_encoding);
    return encoding;
}

static Array_u8 encode_byte(String text) {
    Array_u8 encoding;
    encoding.len = text.len;
    encoding.elements = malloc(text.len * sizeof(u8));
    for (size_t i = 0; i < text.len; i++) {
        encoding.elements[i] = text.chars[i];
    }

    return encoding;
}

Array_u8 encode(String text, u8 version, Mode mode, ECC_Level ecc_level) {
    switch (mode) {
        case MODE_NUMERIC: return encode_numeric(text); break;
        case MODE_ALPHANUM: return encode_alphanumeric(text); break;
        case MODE_BYTE: return encode_byte(text); break;
    }
}
