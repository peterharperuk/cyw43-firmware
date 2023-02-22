#include <stdio.h>
#include "wifi_nvram_43439.h"
//#include "wifi_nvram_1dx.h"

int main() {
    for(int i = 0; i < sizeof(wifi_nvram_4343); i++) {
        putc(wifi_nvram_4343[i], stdout);
    }
    return 0;
}
