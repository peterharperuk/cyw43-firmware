#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "wifi_fw_header.h"

#define ALIGN_UINT(val, align) (((val) + (align) - 1) & ~((align) - 1))

int main() {
   const int padded_wifi_fw_size = ALIGN_UINT(CYW43_WIFI_FW_LEN, 512);
    for(int i = padded_wifi_fw_size; i < padded_wifi_fw_size + CYW43_CLM_LEN; i++) {
        putc(w4343WA1_7_45_98_50_combined[i], stdout);
    }
    return 0;
}
