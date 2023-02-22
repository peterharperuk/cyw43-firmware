#include <stdio.h>
#include "cyw43_btfw_4343A1.h"

int main() {
    for(int i = 0; i < cyw43_btfw_4343A1_len; i++) {
        putc(cyw43_btfw_4343A1[i], stdout);
    }
    return 0;
}
