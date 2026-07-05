#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "vis.h"
#include "data_structs.h"

int main(void) {
    u8 qrcode_version;
    QR_Code qrcode;

    printf("Enter QR code version: "); 
    scanf("%hhu", &qrcode_version); 

    init_qrcode(&qrcode, qrcode_version, L);
    alloc_qrcode(&qrcode);
    create_qrcode_blueprint(&qrcode);
    draw_qrcode_small(&qrcode);

    free(qrcode.matrix);
    return 0;
}
