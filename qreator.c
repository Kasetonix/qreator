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
    Array_u8 data_codewords, idc, iecc;
    u8 version;
    Mode encoding_mode;
    ECC_Level ecc_level;
    // QR_Code qrcode;

    // fputs("Enter the message: ", stdout);
    read(&text);

    ecc_level = ECC_M;
    encoding_mode = get_encoding_mode(text);
    version = get_version(text, encoding_mode, ecc_level);
    data_codewords = packed_encoding(text, encoding_mode, version, ecc_level);

    iecc = interleaved_ec_codewords(&data_codewords, version, ecc_level);
    idc = interleaved_data_codewords(&data_codewords, version, ecc_level);

    puts("Data codewords:");
    for (size_t i = 0; i < data_codewords.len; i++) {
        printf("%03hhu ", data_codewords.elems[i]);
    } putchar('\n');

    putchar('\n');

    puts("Interleaved data codewords:");
    for (size_t i = 0; i < idc.len; i++) {
        if (i % 4 == 0 && i != 0) putchar('\n');
        printf("%03hhu ", idc.elems[i]);
    } putchar('\n');
    
    putchar('\n');

    puts("Interleaved Error Correction Codes:");
    for (size_t i = 0; i < iecc.len; i++) {
        printf("%03hhu ", iecc.elems[i]);
    } putchar('\n');

    // init_qrcode(&qrcode, qrcode_version, encoding_mode, ECC_Q);
    // alloc_qrcode(&qrcode);
    // create_qrcode_blueprint(&qrcode);
    // draw_qrcode_small(&qrcode);

    free(text.chars);
    free(data_codewords.elems);
    free(idc.elems);
    free(iecc.elems);
    // free(qrcode.matrix);
    return 0;
}
