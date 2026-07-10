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
    u8 qrcode_version;
    Mode encoding_mode;
    ECC_Level ecc_level;
    QR_Code qrcode;

    // fputs("Enter the message: ", stdout);
    read(&text);

    ecc_level = ECC_M;
    encoding_mode = get_encoding_mode(text);
    qrcode_version = get_version(text, encoding_mode, ecc_level);
    packed_data_codewords = packed_encoding(text, encoding_mode, qrcode_version, ecc_level);
    codewords = final_codewords(&packed_data_codewords, qrcode_version, ecc_level);

    // puts("Final codewords:");
    // for (size_t i = 0; i < codewords.len; i++) {
    //     printf("%08b\n", codewords.elems[i]);
    // }

    free(text.chars);
    free(packed_data_codewords.elems);

    init_qrcode(&qrcode, qrcode_version, encoding_mode, ecc_level);
    alloc_qrcode(&qrcode);
    create_qrcode_blueprint(&qrcode);
    puts("TOUCH MARKERS:");
    draw_qrcode_small(&qrcode);
    remove_touch_markers(&qrcode);
    puts("QR CODE:");
    draw_qrcode_small(&qrcode);

    free(codewords.elems);
    for (size_t i = 0; i < qrcode.size; i++)
        free(qrcode.matrix[i]);
    free(qrcode.matrix);
    return 0;
}
