add_executable(flash_firmware_cyw43
    flash_firmware.c
)
target_link_libraries(flash_firmware_cyw43 PRIVATE
    pico_stdlib
    pico_cyw43_driver_headers
    pico_btstack_flash_bank_headers
    pico_btstack_hci_transport_cyw43_headers
    hardware_flash
)
target_compile_definitions(flash_firmware_cyw43 PRIVATE
    CYW43_LWIP=0
)
target_include_directories(flash_firmware_cyw43 PRIVATE
    include
)
pico_add_extra_outputs(flash_firmware_cyw43)
