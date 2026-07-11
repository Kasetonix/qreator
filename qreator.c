#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils.h"
#include "encoding.h"
#include "ecc.h"
#include "qrcode_gen.h"
#include "vis.h"

int main(void) {
    String text;
    Array_u8 packed_data_codewords, codewords;
    u8 qrcode_version, chosen_mask;
    u32 penalty, min_penalty;
    Mode encoding_mode;
    ECC_Level ecc_level;
    QR_Code qrcode, masked_qrcode;

    // fputs("Enter the message: ", stdout);
    read(&text);

    ecc_level = ECC_M;
    encoding_mode = get_encoding_mode(text);
    qrcode_version = get_version(text, encoding_mode, ecc_level);
    packed_data_codewords = packed_encoding(text, encoding_mode, qrcode_version, ecc_level);
    codewords = final_codewords(&packed_data_codewords, qrcode_version, ecc_level);

    free(text.chars);
    free(packed_data_codewords.elems);

    init_qrcode(&qrcode, qrcode_version, encoding_mode, ecc_level);
    alloc_qrcode(&qrcode);
    create_qrcode_blueprint(&qrcode);
    add_codewords(&qrcode, codewords);

    masked_qrcode = copy_qrcode_params(&qrcode);
    min_penalty = UINT32_MAX;
    for (u8 i = 0; i < MASK_NUMBER; i++) {
        copy_qrcode_matrix(&qrcode, &masked_qrcode);
        apply_mask(&masked_qrcode, i);
        penalty = calculate_penalty(&masked_qrcode);
        if (penalty < min_penalty) {
            min_penalty = penalty;
            chosen_mask = i;
        }

        // remove_touch_markers(&masked_qrcode);
        // printf("[MASK: %hhu || PENALTY: %04u]\n", i + 1, penalty);
        // draw_qrcode_small(&masked_qrcode);
        // putchar('\n');
    }

    free(masked_qrcode.matrix[0]);
    free(masked_qrcode.matrix);

    apply_mask(&qrcode, chosen_mask);

    printf("%hhu\n", chosen_mask + 1);
    printf("format string: %015b\n", create_format_string(&qrcode, chosen_mask));
    printf("version: %hhu | version string: %018b\n", qrcode.version + 1, create_version_string(&qrcode));

    remove_touch_markers(&qrcode);
    draw_qrcode_small(&qrcode);

    free(codewords.elems);
    free(qrcode.matrix[0]);
    free(qrcode.matrix);

    return 0;
}
