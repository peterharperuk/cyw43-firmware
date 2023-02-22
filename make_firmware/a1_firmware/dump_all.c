#include <stdio.h>
#include <stdint.h>
#include "wifi_fw_header.h"

int main() {
    for(int i = 0; i < sizeof(w4343WA1_7_45_98_50_combined); i++) {
        putc(w4343WA1_7_45_98_50_combined[i], stdout);
    }
    return 0;
}
