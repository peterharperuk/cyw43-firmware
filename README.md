# cyw43-firmware

The format and generation of the CYW43 firmware is quite complicated. The [make_firmware](make_firmware/) project tries to compile the firmware from "source" binaries. PICO_SDK_PATH has to be defined so that the results can be copied to the pico-sdk/lib/cyw43-driver/firmware folder. See the [makefile](make_firmware/Makefile) for more details. All firmware is stored in binary in header files with appropriate macros to define its size before and after compression.

By default the Wi-Fi and Bluetooth firmware is embedded in a PICO_W binary. The PICO_W program [flash_firmare](flash_firmware/) is defined as an example to show how you might flash the firmware to a devices internal flash. The first block written describes the firmware details (see cyw43_flash_firmware_details_t) and the firmware binaries follow.

The project [test_flash_firmware](test_flash_firmware/) is defined to show how to change how firmware is loaded. To configure a PICO_W program to retrieve the firmware from flash you need to change cyw43_get_firmware_funcs. CYW43_FIRMWARE_FLASH_PAGE should be set properly to be the flash page containing the firmware details. As the program no longer contains the Wi-Fi and Bluetooth firmware it should be around 150KB smaller (230KB if firmware is uncompressed).
