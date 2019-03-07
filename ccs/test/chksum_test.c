#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "spi.h"
#include "chksum.h"

void chksum_test(void)
{

    // test
    SPI_FRAME frm1  = {ENC1, 170, 0};
    uint16_t  frm1d = (frm1.addr << 12) | (frm1.data << 4);
    bool      frm1b;

    frm1.chksum     = chksum.generate(frm1d);
    frm1b           = chksum.validate(frm1d, frm1.chksum);

    printf("Checksum is: %u", frm1.chksum);
    printf("Checksum validation: %u", frm1b);
}
