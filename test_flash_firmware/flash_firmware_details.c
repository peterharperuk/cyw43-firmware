#include <stdint.h>
#include <stdlib.h>

#include "cyw43_flash_firmware_defs.h"
#include "cyw43_firmware_defs.h"
#include "cyw43.h"

#ifndef CYW43_FIRMWARE_FLASH_PAGE
#error Need to set CYW43_FIRMWARE_FLASH_PAGE
#endif

#define ALIGN_UINT(val, align) (((val) + (align) - 1) & ~((align) - 1))

static const cyw43_firmware_details_t* cyw43_flash_firmware_details_func(void) {
    static_assert(CYW43_FLASH_BLOCK_SIZE == FLASH_PAGE_SIZE);
    static cyw43_firmware_details_t details;
    if (details.raw_wifi_fw_len == 0) {
        cyw43_flash_firmware_details_t *flash_details = (cyw43_flash_firmware_details_t*)(XIP_BASE + CYW43_FIRMWARE_FLASH_PAGE * FLASH_PAGE_SIZE);
        assert(flash_details->marker == CYW43_FLASH_FIRMWARE_MARKER && flash_details->version == 1);
        if (flash_details->marker == CYW43_FLASH_FIRMWARE_MARKER && flash_details->version == 1) {
            printf("Loading firmware from flash, details in block %x\n", CYW43_FIRMWARE_FLASH_PAGE);
            details.raw_wifi_fw_len = flash_details->raw_wifi_fw_len;
            details.wifi_fw_len = flash_details->wifi_fw_len;
            details.clm_len = flash_details->clm_len;
            details.wifi_nvram_len = flash_details->wifi_nvram_len;
            details.wifi_fw_addr = (const uint8_t*)flash_details->xip_flash_offset;
            details.clm_addr = details.wifi_fw_addr + ALIGN_UINT(flash_details->wifi_fw_len, CYW43_FLASH_BLOCK_SIZE);
            details.wifi_nvram_addr = details.wifi_fw_addr + ALIGN_UINT(flash_details->raw_wifi_fw_len, FLASH_PAGE_SIZE);
            #if CYW43_ENABLE_BLUETOOTH
            details.raw_bt_fw_len = flash_details->raw_bt_fw_len;
            details.bt_fw_len = flash_details->bt_fw_len;
            details.bt_fw_addr = details.wifi_nvram_addr + ALIGN_UINT(flash_details->wifi_nvram_len, FLASH_PAGE_SIZE);
            #endif
        } else {
            panic("Flash format failure. Try running flash_firmware. Check CYW43_FIRMWARE_FLASH_PAGE value");
        }
    }
    return &details;
}

#if CYW43_ENABLE_FIRMWARE_COMPRESSION
const cyw43_firmware_funcs_t *cyw43_get_firmware_funcs_flash(void) {
    static const cyw43_firmware_funcs_t funcs = {
        .firmware_details = cyw43_flash_firmware_details_func,
        .start_wifi_fw = cyw43_start_compressed_wifi_firmware,
        .start_bt_fw = cyw43_start_compressed_bt_firmware,
        .get_wifi_fw = cyw43_read_compressed_firmware,
        .get_bt_fw = cyw43_read_compressed_firmware,
        .get_nvram = cyw43_read_uncompressed_firmware, // never compressed
        .get_clm = cyw43_read_compressed_firmware,
        .end = cyw43_end_compressed_firmware,
    };
    return &funcs;
}
#else
const cyw43_firmware_funcs_t *cyw43_get_firmware_funcs_flash(void) {
    static const cyw43_firmware_funcs_t funcs = {
        .firmware_details = cyw43_flash_firmware_details_func,
        .start_wifi_fw = NULL,
        .start_bt_fw = NULL,
        .get_wifi_fw = cyw43_read_uncompressed_firmware,
        .get_bt_fw = cyw43_read_uncompressed_firmware,
        .get_nvram = cyw43_read_uncompressed_firmware,
        .get_clm = cyw43_read_uncompressed_firmware,
        .end = NULL,
    };
    return &funcs;
}
#endif