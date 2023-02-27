#ifndef CYW43_FLASH_FIRMWARE_DEFS_H
#define CYW43_FLASH_FIRMWARE_DEFS_H

#define CYW43_FLASH_FIRMWARE_MARKER 0x43593433

/*!
 * \brief Structure to store firmware details in flash. This should be stored in flash at page CYW43_FIRMWARE_FLASH_PAGE
 */
//!\{
typedef struct cyw43_flash_firmware_details {
    uint32_t marker;            ///< The value CYW43_FLASH_FIRMWARE_MARKER
    uint32_t version;           ///< Structure version;
    uint32_t xip_flash_offset;  ///< XIP offset of start of firmware
    uint32_t raw_wifi_fw_len;   ///< Size in bytes of the combined wifi firmware data before extraction
    uint32_t wifi_fw_len;       ///< Size of the wifi firmware in bytes after extraction
    uint32_t clm_len;           ///< Size of the clm blob in bytes after extraction
    uint32_t wifi_nvram_len;    ///< Size of nvram data
    uint32_t raw_bt_fw_len;     ///< size of bluetooth firmware data before extraction
    uint32_t bt_fw_len;         ///< size of bluetooth firmware data after extraction
} cyw43_flash_firmware_details_t;
//!\}

#endif