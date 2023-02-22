#include <stdio.h>
#include <stdint.h>
#include "wifi_fw_header.h"

#define ALIGN_UINT(val, align) (((val) + (align) - 1) & ~((align) - 1))

int main() {
    for(int i = 0; i < CYW43_WIFI_FW_LEN; i++) {
        putc(w4343WA1_7_45_98_50_combined[i], stdout);
    }
    return 0;
}
