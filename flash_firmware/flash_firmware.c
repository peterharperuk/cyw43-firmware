#include <stdio.h>
#include "pico/stdio.h"
#include "pico/btstack_flash_bank.h"
#include "hardware/flash.h"
#include "cyw43.h"
#include "cyw43_firmware_defs.h"
#include "cyw43_flash_firmware_defs.h"

#define ALIGN_UINT(val, align) (((val) + (align) - 1) & ~((align) - 1))

#include CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE
#include CYW43_WIFI_NVRAM_INCLUDE_FILE
#include CYW43_BT_FIRMWARE_INCLUDE_FILE

#if !CYW43_ENABLE_BLUETOOTH
#error Bluetooth should be enabled
#endif

#define CYW43_NVRAM_ADDR wifi_nvram_4343
#define CYW43_NVRAM_LEN sizeof(wifi_nvram_4343) // todo: Was padded to 64 bytes which seems wrong

static size_t do_flash(size_t offset, const uint8_t *data, size_t len_bytes) {
    size_t len_first_write_bytes = len_bytes & ~(FLASH_PAGE_SIZE - 1);

    for(size_t addr = offset; addr < offset + len_first_write_bytes; addr += FLASH_PAGE_SIZE) {
        uint8_t page[FLASH_PAGE_SIZE];
        memcpy(page, data + (addr - offset), FLASH_PAGE_SIZE); // copy as it's in flash!
        flash_range_program(addr, page, FLASH_PAGE_SIZE);
        assert(memcmp((const uint8_t *)(XIP_BASE + addr), page, FLASH_PAGE_SIZE) == 0);
    }

    // Write the last page padded to 1 page padded with zeros
    if (len_bytes > len_first_write_bytes) {
        uint8_t padding[FLASH_PAGE_SIZE] = {0};
        assert(len_bytes - len_first_write_bytes < FLASH_PAGE_SIZE);
        memcpy(padding, data + len_first_write_bytes, len_bytes - len_first_write_bytes);
        flash_range_program(offset + len_first_write_bytes, padding, FLASH_PAGE_SIZE);
    }

    // Return offset for next write
    return offset + ALIGN_UINT(len_bytes, FLASH_PAGE_SIZE);
}

static size_t do_verify(size_t offset, const uint8_t *data, size_t len_bytes) {
    const uint8_t *flash = (const uint8_t *)(XIP_BASE + offset);
    if (memcmp(flash, data, len_bytes) != 0) {
        panic("Firmware verification failed");
    }
    return offset + ALIGN_UINT(len_bytes, FLASH_PAGE_SIZE);
}

int main() {
    stdio_init_all();

    // Get length of everything
    const size_t wifi_fw_len = ALIGN_UINT(CYW43_RAW_WIFI_FW_LEN, FLASH_PAGE_SIZE);
    const size_t nvram_len = ALIGN_UINT(CYW43_NVRAM_LEN, FLASH_PAGE_SIZE);
    const size_t bt_fw_len = ALIGN_UINT(CYW43_RAW_BT_FW_LEN, FLASH_PAGE_SIZE);

    // 1 page to describe what's in flash
    const size_t total_len_bytes = FLASH_PAGE_SIZE + wifi_fw_len + nvram_len + bt_fw_len;
    const size_t total_pages = ALIGN_UINT(total_len_bytes, FLASH_PAGE_SIZE) / FLASH_PAGE_SIZE;
    const size_t total_sectors = ALIGN_UINT(total_len_bytes, FLASH_SECTOR_SIZE) / FLASH_SECTOR_SIZE;
    const size_t flash_offset = PICO_FLASH_BANK_STORAGE_OFFSET - (total_sectors * FLASH_SECTOR_SIZE);

    printf("total size %lu bytes\n", total_len_bytes);
    printf("total pages %lu\n", total_pages);
    printf("total sectors %lu\n", total_sectors);
    printf("#define CYW43_FIRMWARE_FLASH_PAGE 0x%x\n", flash_offset / FLASH_PAGE_SIZE);

    // Erase flash
    flash_range_erase(flash_offset, total_sectors * FLASH_SECTOR_SIZE);

    // write details in first page
    cyw43_flash_firmware_details_t details = {
        .marker = CYW43_FLASH_FIRMWARE_MARKER,
        .version = 1,
        .xip_flash_offset = XIP_BASE + flash_offset + FLASH_PAGE_SIZE,
        .raw_wifi_fw_len = CYW43_RAW_WIFI_FW_LEN,
        .wifi_fw_len = CYW43_WIFI_FW_LEN,
        .clm_len = CYW43_CLM_LEN,
        .wifi_nvram_len = CYW43_NVRAM_LEN,
        .raw_bt_fw_len = CYW43_RAW_BT_FW_LEN,
        .bt_fw_len = CYW43_BT_FW_LEN,
    };
    uint8_t page[FLASH_SECTOR_SIZE] = {0};
    memcpy(page, &details, sizeof(details));
    flash_range_program(flash_offset, page, FLASH_PAGE_SIZE);
    if (memcmp((const uint8_t *)(XIP_BASE + flash_offset), page, FLASH_PAGE_SIZE) != 0) {
        panic("Firmware details verification failed");
    }

    // Write firmware
    size_t current_offset = flash_offset + FLASH_PAGE_SIZE;
    current_offset = do_flash(current_offset, CYW43_WIFI_FW_ADDR, CYW43_RAW_WIFI_FW_LEN);
    current_offset = do_flash(current_offset, CYW43_NVRAM_ADDR, CYW43_NVRAM_LEN);
    current_offset = do_flash(current_offset, CYW43_BT_FW_ADDR, CYW43_RAW_BT_FW_LEN);
    assert((current_offset - flash_offset) /  FLASH_PAGE_SIZE == total_pages);

    // Check firmware can be read
    current_offset = flash_offset + FLASH_PAGE_SIZE;
    current_offset = do_verify(current_offset, CYW43_WIFI_FW_ADDR, CYW43_RAW_WIFI_FW_LEN);
    current_offset = do_verify(current_offset, CYW43_NVRAM_ADDR, CYW43_NVRAM_LEN);
    current_offset = do_verify(current_offset, CYW43_BT_FW_ADDR, CYW43_RAW_BT_FW_LEN);

    printf("Firmware written\n");
    return 0;
}