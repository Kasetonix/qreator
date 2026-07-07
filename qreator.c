#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "utils.h"
#include "encoding.h"
// #include "qrcode_gen.h"
// #include "vis.h"

int main(void) {
    String text;
    // QR_Code qrcode;
    Array_u8 packed_enc;

    // fputs("Enter the message: ", stdout);
    read(&text);

    packed_enc = packed_encoding(text, ECC_Q);

    for (size_t i = 0; i < packed_enc.len; i++) {
        printf("%0*b\n", 8, packed_enc.elems[i]);
    }

    // init_qrcode(&qrcode, qrcode_version, encoding_mode, ECC_Q);
    // alloc_qrcode(&qrcode);
    // create_qrcode_blueprint(&qrcode);
    // draw_qrcode_small(&qrcode);

    free(text.chars);
    // free(qrcode.matrix);
    return 0;
}
