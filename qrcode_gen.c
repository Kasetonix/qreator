#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "qrcode_gen.h"
#include "encoding.h"
#include "luts.h"
#include "utils.h"

void init_qrcode(QR_Code *qrcode, u8 version, Mode mode, ECC_Level ecc_level) {
    if (version >= 40)     error("Invalid QR code version.");
    if (mode > MODE_BYTE)  error("Invalid encoding mode.");
    if (ecc_level > ECC_H) error("Invalid ECC level.");

    qrcode->version = version;
    qrcode->size = 17 + 4 * (version + 1);
    qrcode->mode = mode;
    qrcode->ecc_level = ecc_level;
    qrcode->matrix = NULL;
}

void alloc_qrcode(QR_Code *qrcode) {
    u8 *ptr;

    qrcode->matrix = malloc(qrcode->size * sizeof(u8 *));
    if (qrcode->matrix == NULL)
        error("Couldn't allocate qrcode array.");

    ptr = calloc(qrcode->size * qrcode->size, sizeof(u8));
    if (ptr == NULL)
        error("Couldn't allocate qrcode array.");

    for (size_t y = 0; y < qrcode->size; y++)
        qrcode->matrix[y] = ptr + y * qrcode->size;
}

void copy_qrcode_matrix(QR_Code *qrcode, QR_Code *copy) {
    u8 **orig_matrix, **copy_matrix;
    size_t size;
    
    size = copy->size;
    orig_matrix = qrcode->matrix;
    copy_matrix = copy->matrix;

    for (size_t y = 0; y < size; y++)
        for (size_t x = 0; x < size; x++)
            copy_matrix[y][x] = orig_matrix[y][x];
}

QR_Code copy_qrcode_params(QR_Code *qrcode) {
    QR_Code copy;

    copy.version = qrcode->version;
    copy.size = qrcode->size;
    copy.mode = qrcode->mode;
    copy.ecc_level = qrcode->ecc_level;
    copy.matrix = NULL;

    alloc_qrcode(&copy);
    return copy;
}

static void add_position_detection(QR_Code *qrcode) {
    size_t size;
    u8 **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t y = 0; y < POSDET_MARKER_SIZE; y++) {
        for (size_t x = 0; x < POSDET_MARKER_SIZE; x++) {
            if (x == 0 || x == POSDET_MARKER_SIZE - 1 ||
                y == 0 || y == POSDET_MARKER_SIZE - 1) {
                matrix[y][x]            |= ON_MARKER;
                matrix[y][size - x - 1] |= ON_MARKER;
                matrix[size - y - 1][x] |= ON_MARKER;
            }
            else if (x != 1 && x != POSDET_MARKER_SIZE - 2 &&
                     y != 1 && y != POSDET_MARKER_SIZE - 2) {
                matrix[y][x]            |= ON_MARKER;
                matrix[y][size - x - 1] |= ON_MARKER;
                matrix[size - y - 1][x] |= ON_MARKER;
            }
        }
    }

    for (size_t y = 0; y < POSDET_MARKER_SIZE + 1; y++) {
        for (size_t x = 0; x < POSDET_MARKER_SIZE + 1; x++) {
            matrix[y][x]            |= BLUEPRINT_MARKER;
            matrix[y][size - x - 1] |= BLUEPRINT_MARKER;
            matrix[size - y - 1][x] |= BLUEPRINT_MARKER;
        }
    }
}

static void add_timing_patterns(QR_Code *qrcode) {
    size_t size;
    u8 **matrix;
    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t i = POSDET_MARKER_SIZE + 1; i < size - POSDET_MARKER_SIZE - 1; i++) {
        matrix[POSDET_MARKER_SIZE - 1][i] |= BLUEPRINT_MARKER;
        matrix[i][POSDET_MARKER_SIZE - 1] |= BLUEPRINT_MARKER;
        if (i % 2 == 0) {
            matrix[POSDET_MARKER_SIZE - 1][i] |= ON_MARKER;
            matrix[i][POSDET_MARKER_SIZE - 1] |= ON_MARKER;
        }
    }
}

static bool is_ap_placable(QR_Code *qrcode, size_t y, size_t x) {
    u8 **matrix;
    matrix = qrcode->matrix;

    for (ssize_t dy = -ALPAT_MARKER_SIZE / 2; dy <= ALPAT_MARKER_SIZE / 2; dy++)
        for (ssize_t dx = -ALPAT_MARKER_SIZE / 2; dx <= ALPAT_MARKER_SIZE / 2; dx++)
            if ((matrix[y + dy][x + dx] & BLUEPRINT_MARKER) == BLUEPRINT_MARKER) return false;

    return true;
}

static void draw_ap(QR_Code *qrcode, size_t y, size_t x) {
    u8 **matrix;
    matrix = qrcode->matrix;

    matrix[y][x] |= ON_MARKER;

    for (ssize_t dy = -ALPAT_MARKER_SIZE / 2; dy <= ALPAT_MARKER_SIZE / 2; dy++) {
        for (ssize_t dx = -ALPAT_MARKER_SIZE / 2; dx <= ALPAT_MARKER_SIZE / 2; dx++) {
            matrix[y + dy][x + dx] |= BLUEPRINT_MARKER;
            if (dx == -ALPAT_MARKER_SIZE / 2 || dx == ALPAT_MARKER_SIZE / 2)
                matrix[y + dy][x + dx] |= ON_MARKER;
            if (dy == -ALPAT_MARKER_SIZE / 2 || dy == ALPAT_MARKER_SIZE / 2)
                matrix[y + dy][x + dx] |= ON_MARKER;
        }
    }
}

static void add_alignment_patterns(QR_Code *qrcode) {
    u8 version;
    u8 *alpos;

    version = qrcode->version;
    alpos = alignment_pattern_positions[version];
    for (size_t i = 0; i < AP_POS; i++) {
        if (alpos[i] == 0) break;
        for (size_t j = 0; j < AP_POS; j++) {
            if (alpos[j] == 0) break;
            if (is_ap_placable(qrcode, alpos[j], alpos[i]))
                draw_ap(qrcode, alpos[j], alpos[i]);
        }
    }
}

static inline void add_dark_module(QR_Code *qrcode) {
    qrcode->matrix[qrcode->size - 8][8] |= ON_MARKER | BLUEPRINT_MARKER;
}

static void touch_format_area(QR_Code *qrcode) {
    u8 **matrix;
    size_t size;

    size = qrcode->size;
    matrix = qrcode->matrix;

    for (size_t y = 0; y < POSDET_MARKER_SIZE + 2; y++)
        matrix[y][POSDET_MARKER_SIZE + 1] |= BLUEPRINT_MARKER;

    for (size_t y = size - POSDET_MARKER_SIZE - 1; y < size; y++)
        matrix[y][POSDET_MARKER_SIZE + 1] |= BLUEPRINT_MARKER;

    for (size_t x = 0; x < POSDET_MARKER_SIZE + 2; x++)
        matrix[POSDET_MARKER_SIZE + 1][x] |= BLUEPRINT_MARKER;

    for (size_t x = size - POSDET_MARKER_SIZE - 1; x < size; x++)
        matrix[POSDET_MARKER_SIZE + 1][x] |= BLUEPRINT_MARKER;
}

static void touch_version_area(QR_Code *qrcode) {
    u8 **matrix;
    size_t size;

    matrix = qrcode->matrix;
    size = qrcode->size;
    
    for (size_t y = 0; y < POSDET_MARKER_SIZE - 1; y++)
        for (size_t x = 0; x < VER_WIDTH; x++)
            matrix[y][size - POSDET_MARKER_SIZE - 2 - x] |= BLUEPRINT_MARKER;

    for (size_t y = 0; y < VER_WIDTH; y++)
        for (size_t x = 0; x < POSDET_MARKER_SIZE - 1; x++)
            matrix[size - POSDET_MARKER_SIZE - 2 - y][x] |= BLUEPRINT_MARKER;
}

void create_qrcode_blueprint(QR_Code *qrcode) {
    add_position_detection(qrcode);
    add_alignment_patterns(qrcode);
    add_timing_patterns(qrcode);
    add_dark_module(qrcode);
    touch_format_area(qrcode);
    if (qrcode->version >= 6)
        touch_version_area(qrcode);
}

static inline bool is_blueprint(QR_Code *qrcode, size_t y, size_t x) {
    return (qrcode->matrix[y][x] & BLUEPRINT_MARKER) == BLUEPRINT_MARKER;
}

static Pos next_module_pos(QR_Code *qrcode, Pos pos) {
    static bool upwards = true;
    static bool horizontal = false;
    static bool switching = false;
    Pos np = pos;

    do {
        horizontal = !horizontal;

        if (upwards && np.y == 0) {
            upwards = false;
            switching = true;
        }

        if (!upwards && np.y == (ssize_t) qrcode->size - 1) {
            upwards = true;
            switching = true;
        }

        // Vertical timing pattern
        if (np.x == POSDET_MARKER_SIZE - 1)
            np.x--;

        if (!horizontal && switching) {
            switching = false;
            np.x--;
        } else if (horizontal) {
            np.x--;
        } else {
            np.x++;
            if (upwards) np.y--;
            else         np.y++;
        }

        if (np.y < 0 || np.y >= (ssize_t) qrcode->size ||
            np.x < 0 || np.x >= (ssize_t) qrcode->size) {
            return (Pos) { .y = 0, .x = 0 };
        }
    } while (is_blueprint(qrcode, np.y, np.x));

    return np;
}

void add_codewords(QR_Code *qrcode, Array_u8 codewords) {
    Pos pos = { .y = qrcode->size - 1, .x = qrcode->size - 1 };
    u8 **matrix;
    u8 codeword;

    matrix = qrcode->matrix;

    for (size_t i = 0; i < codewords.len; i++) {
        codeword = codewords.elems[i];
        for (u8 j = 0; j < 8; j++) {
            if (((codeword >> j) & 1) == 1)
                matrix[pos.y][pos.x] |= ON_MARKER;
            pos = next_module_pos(qrcode, pos);
            if (pos.x == 0 && pos.y == 0 && i != codewords.len - 1 && j != 7)
                error("Went out of the qr code when writing codewords!");
        }
    }
}

void remove_touch_markers(QR_Code *qrcode) {
    for (size_t y = 0; y < qrcode->size; y++)
        for (size_t x = 0; x < qrcode->size; x++)
            qrcode->matrix[y][x] >>= 1;
}

static inline bool mask0_func(size_t y, size_t x) { return (y + x) % 2 == 0; }
static inline bool mask1_func(size_t y, size_t x) { x = x; return y % 2 == 0; }
static inline bool mask2_func(size_t y, size_t x) { y = y; return x % 3 == 0; }
static inline bool mask3_func(size_t y, size_t x) { return (y + x) % 3 == 0; }
static inline bool mask4_func(size_t y, size_t x) { return (y / 2 + x / 3) % 2 == 0; }
static inline bool mask5_func(size_t y, size_t x) { return ((y * x) % 2) + ((y * x) % 3) == 0; }
static inline bool mask6_func(size_t y, size_t x) { return (((y * x) % 2) + ((y * x) % 3)) % 2 == 0; }
static inline bool mask7_func(size_t y, size_t x) { return (((y + x) % 2) + ((y * x) % 3)) % 2 == 0; }

void apply_mask(QR_Code *qrcode, u8 mask_number) {
    bool (*mask_func)(size_t, size_t);

    if (mask_number >= MASK_NUMBER)
        error("Incorrect mask number.");

    switch (mask_number) {
        case 0: mask_func = mask0_func; break;    
        case 1: mask_func = mask1_func; break;    
        case 2: mask_func = mask2_func; break;    
        case 3: mask_func = mask3_func; break;    
        case 4: mask_func = mask4_func; break;    
        case 5: mask_func = mask5_func; break;    
        case 6: mask_func = mask6_func; break;    
        case 7: mask_func = mask7_func; break;    
    }

    for (size_t y = 0; y < qrcode->size; y++)
        for (size_t x = 0; x < qrcode->size; x++)
            if (!is_blueprint(qrcode, y, x) && mask_func(y, x))
                qrcode->matrix[y][x] ^= ON_MARKER;
}

static u32 calculate_stripe_penalty(QR_Code *qrcode) {
    u32 penalty;
    u8 consecutive;
    u8 **matrix;
    size_t size;
    u8 current, last;

    matrix = qrcode->matrix;
    size = qrcode->size;
    penalty = 0;

    // Horizontal
    for (size_t y = 0; y < size; y++) {
        last = 1;
        consecutive = 0;

        for (size_t x = 0; x < size; x++) {
            current = matrix[y][x] >> 1;
            if (current == last)
                consecutive++;
            else {
                if (consecutive >= 5)
                    penalty += consecutive - 2;
                consecutive = 0;
            }

            last = current;
        }

        if (consecutive >= 5)
            penalty += consecutive - 2;
    }

    // Vertical
    for (size_t x = 0; x < size; x++) {
        last = 1;
        consecutive = 0;

        for (size_t y = 0; y < size; y++) {
            current = matrix[y][x] >> 1;
            if (current == last)
                consecutive++;
            else {
                if (consecutive >= 5)
                    penalty += consecutive - 2;
                consecutive = 0;
            }

            last = current;
        }

        if (consecutive >= 5)
            penalty += consecutive - 2;
    }

    return penalty;
}

static u32 calculate_square_penalty(QR_Code *qrcode) {
    u32 penalty;
    u8 square;
    u8 **matrix;
    size_t size;

    matrix = qrcode->matrix;
    size = qrcode->size;
    penalty = 0;

    for (size_t y = 0; y < size - 1; y++) {
        square = 0;
        square |= ((matrix[  y  ][0] >> 1)     );
        square |= ((matrix[y + 1][0] >> 1) << 1);
        for (size_t x = 0; x < size - 1; x++) {
            square |= ((matrix[  y  ][x + 1] >> 1) << 2);
            square |= ((matrix[y + 1][x + 1] >> 1) << 3);

            if (square == 15 || square == 0) // 0b1111 or 0b0000
                penalty += 3;
            
            square >>= 2;
        }
    }

    return penalty;
}

static u32 calculate_finder_pattern_penalty(QR_Code *qrcode) {
    u32 penalty;
    u16 stripe;
    u8 **matrix;
    size_t size;

    matrix = qrcode->matrix;
    size = qrcode->size;
    penalty = 0;

    for (size_t y = 0; y < size; y++) {
        stripe = 0;
        for (u8 i = 0; i < 10; i++) {
            stripe |= ((matrix[y][i] >> 1) << i);
        }

        for (size_t x = 10; x < size; x++) {
            stripe |= ((matrix[y][x] >> 1) << 10);
            if (stripe == 1488 || stripe == 93) // 0b10111010000 or 0b00001011101
                penalty += 40;
            
            stripe >>= 1;
        }
    }

    for (size_t x = 0; x < size; x++) {
        stripe = 0;
        for (u8 i = 0; i < 10; i++) {
            stripe |= ((matrix[i][x] >> 1) << i);
        }

        for (size_t y = 10; y < size; y++) {
            stripe |= ((matrix[y][x] >> 1) << 10);
            if (stripe == 1488 || stripe == 93) // 0b10111010000 or 0b00001011101
                penalty += 40;

            stripe >>= 1;
        }
    }

    return penalty;
}

static u32 calculate_ratio_penalty(QR_Code *qrcode) {
    u16 dark_modules, all_modules;
    u8 percentage, down_dev, up_dev;
    u8 **matrix;
    size_t size;

    matrix = qrcode->matrix;
    size = qrcode->size;
    all_modules = size * size;
    dark_modules = 0;

    for (size_t y = 0; y < size; y++)
        for (size_t x = 0; x < size; x++)
            if ((matrix[y][x] >> 1) == 1)
                dark_modules++;

    percentage = dark_modules * 100 / all_modules; 

    down_dev = absval(50 - (percentage - (percentage % 5)));
    up_dev = absval(50 - (percentage + (5 - percentage % 5)));

    return 2 * (up_dev <= down_dev ? up_dev : down_dev);
}

u32 calculate_penalty(QR_Code *qrcode) {
    u32 p_stripe, p_square, p_finder_pattern, p_ratio;

    p_stripe = calculate_stripe_penalty(qrcode);
    p_square = calculate_square_penalty(qrcode);
    p_finder_pattern = calculate_finder_pattern_penalty(qrcode);
    p_ratio = calculate_ratio_penalty(qrcode);

    return p_stripe + p_square + p_finder_pattern + p_ratio;
}

u16 create_format_string(QR_Code *qrcode, u8 chosen_mask) {
    u16 format_string, gen_pol, ecc_bits;
    u8 ecc_level_code, ecc_bit_len, diff; 

    gen_pol = FORMAT_ECC_GEN_POL; 
    ecc_level_code = 2 * (qrcode->ecc_level / 2) + (qrcode->ecc_level + 1) % 2;
    format_string = (ecc_level_code << 3) | chosen_mask;
    
    ecc_bits = format_string << FORMAT_ECC_LEN;
    ecc_bit_len = bitstring_len(ecc_bits);
    gen_pol <<= (ecc_bit_len - FORMAT_ECC_LEN - 1);

    while (ecc_bit_len > FORMAT_ECC_LEN) {
        ecc_bits ^= gen_pol;
        diff = ecc_bit_len - bitstring_len(ecc_bits);
        ecc_bit_len -= diff;
        gen_pol >>= diff;
    }

    return ((format_string << FORMAT_ECC_LEN) | ecc_bits) ^ FORMAT_ECC_MASK;
}

u32 create_version_string(QR_Code *qrcode) {
    u32 version_string, gen_pol, ecc_bits;
    u8 ecc_bit_len, diff;

    gen_pol = VERSION_ECC_GEN_POL;
    version_string = qrcode->version + 1;

    ecc_bits = version_string << VERSION_ECC_LEN;
    ecc_bit_len = bitstring_len(ecc_bits);
    gen_pol <<= (ecc_bit_len - VERSION_ECC_LEN - 1);

    while (ecc_bit_len > VERSION_ECC_LEN) {
        ecc_bits ^= gen_pol;
        diff = ecc_bit_len - bitstring_len(ecc_bits);
        ecc_bit_len -= diff;
        gen_pol >>= diff;
    }

    return (version_string << VERSION_ECC_LEN) | ecc_bits;
}
