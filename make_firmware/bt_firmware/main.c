#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

extern const char brcm_patch_version[];
extern const uint8_t brcm_patchram_buf[];
extern const int brcm_patch_ram_length;

void dump_byte(uint8_t data) {
    printf("%02x ", data);
}

void dump_bytes(const uint8_t *data, size_t len) {
    for(int i=0; i<len; i++) {
        if (i > 0 && i % 16 == 0) {
            printf("\n");
        }
        dump_byte(data[i]);
    }
}

static inline uint8_t nibble_for_char(char c){
    if ((c >= '0') && (c <= '9')) return c - '0';
    if ((c >= 'A') && (c <= 'F')) return c - 'A' + 10;
    return -1;
}

static inline uint8_t read_hex_byte(const char *str) {
    return nibble_for_char(*str) << 4 | nibble_for_char(*(str + 1));
}

static uint32_t read_hex(const char *str, int nchars) {
    uint32_t result = 0;
    assert(nchars > 0 && nchars <= 8 && nchars % 2 == 0);
    for(int pos = 0; pos < nchars; pos += 2) {
        result <<= 8;
        result |= read_hex_byte(str + pos);
    }
    return result;
}

// Dumps data in the following format
// 1 byte: number of characters in version string including null terminator
// n bytes: version string
// 1 byte: number of records...
// ...
// 1 byte: data count
// 2 bytes: address
// 1 byte: address type
// n bytes: data
//
// Pipe the output of this to get the raw data to append to firmware
// ./bt_fw.out | xxd -r -p - > out.dat

int main() {
    size_t len = strlen(brcm_patch_version);
    assert(len <= 0xff);
    dump_byte((uint8_t)len + 1);
    printf("\n");
    dump_bytes(brcm_patch_version, len + 1);

    size_t pos = 0;
    int records = 0;

    // Count records
    while(pos < brcm_patch_ram_length) {
        pos += 1;
        uint32_t num_chars = read_hex(brcm_patchram_buf + pos, 2);
        pos += 8 + num_chars * 2 + 2;
        assert(brcm_patchram_buf[pos++] == 0x0A);
        records++;
    }
    assert(records <= 0xff);
    printf("\n");
    dump_byte((uint8_t)records);

    // Repeat for the data
    pos = 0;
    while(pos < brcm_patch_ram_length) {
        assert(brcm_patchram_buf[pos] == ':');
        pos += 1;

        uint32_t num_chars = read_hex(brcm_patchram_buf + pos, 2);
        assert(num_chars <= 0xff);
        printf("\n");
        dump_byte(num_chars);
        pos += 2;

        uint32_t addr = read_hex(brcm_patchram_buf + pos, 4);
        dump_byte(addr >> 8);
        dump_byte(addr & 0xff);
        pos += 4;

        uint32_t type = read_hex(brcm_patchram_buf + pos, 2);
        dump_byte(type);
        pos += 2;

        printf("\n");
        if (num_chars > 0) {
            for(int i = 0; i < num_chars; i++) {
                uint32_t octet = read_hex(brcm_patchram_buf + pos, 2);
                if (i > 0 && i % 16 == 0) {
                    printf("\n");
                }
                dump_byte(octet);
                pos += 2;
            }
        }
        pos += 2; // ignored?
        assert(brcm_patchram_buf[pos++] == 0x0A);
    }

    return 0;
}