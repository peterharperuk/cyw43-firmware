#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "hcd.h"

int main() {
    printf("length: %u\n", CYW4343A2_001_003_016_0066_0000_Generic_SDIO_37_4MHz_wlbga_BU_dl_signed_hcd_len);
    const uint8_t *buf_start = CYW4343A2_001_003_016_0066_0000_Generic_SDIO_37_4MHz_wlbga_BU_dl_signed_hcd;
    const uint8_t *buf = buf_start;
    const uint8_t *buf_end = buf + CYW4343A2_001_003_016_0066_0000_Generic_SDIO_37_4MHz_wlbga_BU_dl_signed_hcd_len;
    int records = 0;
    while(buf < buf_end) {
        assert(*buf == 0x4c || *buf == 0x4e);
        *buf++;
        assert(*buf == 0xfc);
        *buf++;
        uint8_t length = *buf;
        *buf++;
        printf("chars: %d\n", length - 4);
        uint16_t addr = *buf | *(buf+1)<<8;
        uint16_t high_addr = *(buf+2) | *(buf+3)<<8;
        //assert(*(buf+2) == 0x21);
        //assert(*(buf+3) == 0);
        printf("addr: %04x\n", addr);
        printf("high addr: %02x%02x\n", (high_addr >> 8) & 0xff, high_addr & 0xff);
        buf += length;
        records++;
    }
    printf("records: %d\n", records);
    return 0;
}
