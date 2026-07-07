#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "encoding.h"
// #include "qrcode_gen.h"
// #include "vis.h"

int main(void) {
    String text;
    u8 qrcode_version;
    Mode encoding_mode;
    // QR_Code qrcode;
    Array_u16 encoding;
    u8 *word_lengths;

    // fputs("Enter the message: ", stdout);
    read(&text);

    encoding_mode = get_encoding_mode(text);
    qrcode_version = get_version(text, encoding_mode, ECC_Q);
    encode(text, qrcode_version, encoding_mode, ECC_Q, &encoding, &word_lengths);

    printf("Mode:   %0*b\n", word_lengths[0], encoding.elems[0]);
    printf("Length: %0*b\n", word_lengths[1], encoding.elems[1]);
    puts("Data:");
    for (size_t i = 2; i < encoding.len; i++) {
        printf("%0*b\n", word_lengths[i], encoding.elems[i]);
    }

    // init_qrcode(&qrcode, qrcode_version, encoding_mode, ECC_Q);
    // alloc_qrcode(&qrcode);
    // create_qrcode_blueprint(&qrcode);
    // draw_qrcode_small(&qrcode);

    free(text.chars);
    free(encoding.elems);
    // free(qrcode.matrix);
    return 0;
}
