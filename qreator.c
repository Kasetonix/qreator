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
    u8 version;
    Mode encoding_mode;
    ECC_Level ecc_level;
    // QR_Code qrcode;
    Array_u8 packed_enc;
    Polynomial gen_pol;
    Array_u8 ec_codewords;

    // fputs("Enter the message: ", stdout);
    read(&text);

    ecc_level = ECC_M;
    encoding_mode = get_encoding_mode(text);
    version = get_version(text, encoding_mode, ecc_level);

    packed_enc = packed_encoding(text, encoding_mode, version, ecc_level);
    gen_pol = create_generator_polynomial(version, ecc_level);
    ec_codewords = generate_ec_codewords(&packed_enc, &gen_pol);

    puts("Error correction codewords:");
    for (size_t i = 0; i < ec_codewords.len; i++) {
        printf("%hhu ", ec_codewords.elems[i]);
    } putchar('\n');

    // init_qrcode(&qrcode, qrcode_version, encoding_mode, ECC_Q);
    // alloc_qrcode(&qrcode);
    // create_qrcode_blueprint(&qrcode);
    // draw_qrcode_small(&qrcode);

    free(text.chars);
    free(packed_enc.elems);
    free(gen_pol.coeff);
    free(ec_codewords.elems);
    // free(qrcode.matrix);
    return 0;
}
