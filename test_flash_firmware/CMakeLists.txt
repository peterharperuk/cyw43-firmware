add_executable(test_flash_firmware_cyw43
    ${PICO_SDK_PATH}/lib/btstack/example/gatt_counter.c
    flash_firmware_details.c
    test_flash_firmware.c
    )
target_link_libraries(test_flash_firmware_cyw43 PRIVATE
    pico_stdlib
    pico_btstack_ble
    pico_btstack_cyw43
    pico_cyw43_arch_none
    )
target_compile_definitions(test_flash_firmware_cyw43 PRIVATE
    CYW43_FIRMWARE_FLASH_PAGE=0x1d80
    cyw43_get_firmware_funcs=cyw43_get_firmware_funcs_flash # Comment this out to embed firmware in the binary
    CYW43_LWIP=0
)
target_include_directories(test_flash_firmware_cyw43 PRIVATE
    .
    ../flash_firmware/include
)
pico_btstack_make_gatt_header(test_flash_firmware_cyw43 PRIVATE
    ${PICO_SDK_PATH}/lib/btstack/example/gatt_counter.gatt
)
pico_add_extra_outputs(test_flash_firmware_cyw43)
