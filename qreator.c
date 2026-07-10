#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils.h"
#include "encoding.h"
#include "ecc.h"
// #include "qrcode_gen.h"
// #include "vis.h"

int main(void) {
    String text;
    Array_u8 packed_data_codewords, codewords;
    u8 version;
    Mode encoding_mode;
    ECC_Level ecc_level;
    // QR_Code qrcode;

    // fputs("Enter the message: ", stdout);
    read(&text);

    ecc_level = ECC_M;
    encoding_mode = get_encoding_mode(text);
    version = get_version(text, encoding_mode, ecc_level);
    packed_data_codewords = packed_encoding(text, encoding_mode, version, ecc_level);
    codewords = final_codewords(&packed_data_codewords, version, ecc_level);

    puts("Final codewords:");
    for (size_t i = 0; i < codewords.len; i++) {
        printf("%08b\n", codewords.elems[i]);
    }

    // init_qrcode(&qrcode, qrcode_version, encoding_mode, ECC_Q);
    // alloc_qrcode(&qrcode);
    // create_qrcode_blueprint(&qrcode);
    // draw_qrcode_small(&qrcode);

    free(text.chars);
    free(packed_data_codewords.elems);
    free(codewords.elems);
    // free(qrcode.matrix);
    return 0;
}
