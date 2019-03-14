#include <stdint.h>
#include <stdbool.h>

#include "spi.h"
#include "chksum2.h"

void chksum_test(void)
{
    // 4-bit checksum test
    // input:   1011 1000 1110
    // size:    3 nibbles (12 bits)
    // output:  1000

    uint16_t data_4bit      = 0b101110001110;
    uint8_t  chks_4bit      = chksum2.gen_4bit(data_4bit, 3);
    bool     vali_4bit      = chksum2.val_8bit(data_4bit, 2, chks_4bit);

    // 8-bit checksum test
    // input:   10101011 10001110
    // size:    2 bytes (16 bits)
    // output:  11010110

    uint8_t  data_8bit[2]   = { 0b10101011, 0b10001110 };
    uint8_t  chks_8bit      = chksum2.gen_8bit(data_8bit, 2);
    bool     vali_8bit      = chksum2.val_8bit(data_8bit, 2, chks_8bit);
}
