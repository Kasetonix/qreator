#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "encoding.h"
#include "qrcode_gen.h"
#include "vis.h"

int main(void) {
    u8 qrcode_version;
    QR_Code qrcode;
    String text;
    Mode encoding_mode;

    fputs("Enter the message: ", stdout);
    read(&text);

    encoding_mode = get_encoding_mode(text);
    qrcode_version = get_version(text, encoding_mode, ECC_Q);

    init_qrcode(&qrcode, qrcode_version, encoding_mode, ECC_Q);
    alloc_qrcode(&qrcode);
    create_qrcode_blueprint(&qrcode);
    draw_qrcode_small(&qrcode);

    free(text.chars);
    free(qrcode.matrix);
    return 0;
}
