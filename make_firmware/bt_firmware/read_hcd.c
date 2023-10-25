#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "hcd.h"

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

void exit_error(const char *what)
{
   printf("ERROR: %s\n", what);
   exit(1);
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
int main(int argc, char *argv[]) {

    FILE *fin;

    if (argc < 2) {
       printf("Syntax: read_hcd <hcd filename>\n");
       return 1;
    }

    if ((fin = fopen(argv[1], "rb")) == NULL) exit_error("source file");
    fseek(fin, 0, SEEK_END);
    uint32_t hcd_len = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    uint8_t *source = (uint8_t *)malloc(hcd_len);
    if (source == NULL) exit_error("memory");
    if (fread(source, 1, hcd_len, fin) != hcd_len) exit_error("read");
    fclose(fin);

    const uint8_t *buf_start = source;
    const uint8_t *buf = buf_start;
    const uint8_t *buf_end = buf + hcd_len;

    // Take a copy of the filname
    size_t filename_length = strlen(argv[1]);
    uint8_t *filename = (uint8_t *)malloc(filename_length + 1);
    strcpy(filename, argv[1]);

    // Lose the extension
    if (filename[filename_length - 4] == '.') {
        filename[filename_length - 4] = 0;
        filename_length -= 4;
    }

    // Lose the path
    char *path = strrchr(filename, '/');
    if (path) {
        filename = path + 1;
        filename_length = strlen(filename);
    }

    assert(filename_length + 1 <= 0xff);
    dump_byte((uint8_t)filename_length + 1);
    printf("\n");
    dump_bytes(filename, filename_length + 1);

    size_t pos = 0;
    int records = 0;

    while(buf < buf_end) {
        assert(*buf == 0x4c || *buf == 0x4e);
        *buf++;
        assert(*buf++ == 0xfc);
        uint8_t length = *buf++;
        uint16_t addr = *buf | *(buf+1)<<8;
        buf += length;
        records++;
    }

    assert(records <= 0xff);
    printf("\n");
    dump_byte((uint8_t)records);
    buf = buf_start;

    // This puts the state machine into extended address mode?!
    dump_byte(0x02); // chars
    dump_byte(0x00); dump_byte(0x00); // address
    dump_byte(0x04); // see BTFW_HEX_LINE_TYPE_EXTENDED_ADDRESS
    dump_byte(0x00); dump_byte(0x21); // The "magic" high address?

    // Repeat for the data
    pos = 0;
    const size_t pos_end = buf_end - buf_start; 
    while(pos < pos_end) {
        uint8_t hdr = buf[pos];
        assert(hdr == 0x4c || hdr == 0x4e);
        pos += 1;
        assert(buf[pos] == 0xfc);
        pos += 1;
        printf("\n");

        // This should be the end!
        if (hdr == 0x4e) {
            assert(pos == pos_end - 5);
            dump_byte(0x00); // chars
            dump_byte(0x00); dump_byte(0x00); // address
            dump_byte(0x01); // see BTFW_HEX_LINE_TYPE_END_OF_DATA
            break;
        }
        uint32_t num_chars = buf[pos] - 4; // length include addr and mystery field?
        assert(num_chars > 0);

        assert(num_chars <= 0xff);
        dump_byte(num_chars);
        pos += 1;

        uint32_t addr = buf[pos] | buf[pos+1]<<8;
        dump_byte(addr >> 8);
        dump_byte(addr & 0xff);
        pos += 2;

        uint8_t type = 0;
        dump_byte(type);
        pos += 2; // mystery field

        if (num_chars > 0) {
            for(int i = 0; i < num_chars; i++) {
                uint32_t octet = buf[pos];
                if (i % 16 == 0) {
                    printf("\n");
                }
                dump_byte(octet);
                pos += 1;
            }
        }
    }

    return 0;
}