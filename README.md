# cyw43-firmware

The Wi-Fi and Bluetooth firmware for [cyw43-driver](https://github.com/georgerobotics/cyw43-driver) is rather complicated and about to get more complicated if firmware compression and flash support are added.

This project tries to compile the firmware from "source" binaries. PICO_SDK_PATH has to be defined so that the results can be copied to the pico-sdk/lib/cyw43-driver/firmware folder. See the [makefile](make_firmware/Makefile) for more details. All firmware is stored in binary in header files with appropriate macros to define its size.

The PICO_W program [flash_firmare](flash_firmware/) is defined to flash the firmware to a devices internal flash. The first block written describes the firmware details (see cyw43_flash_firmware_details_t in the pico-sdk) and the firmware binaries follow.

By default the Wi-Fi and Bluetooth firmware is embedded in a PICO_W binary. To configure a PICO_W program to retrieve the firmware from flash you need to define CYW43_FIRMWARE_FLASH_BLOCK to be the flash page containing the firmware details. As your program no longer contains the Wi-Fi and Bluetooth firmware, it should be around 230KB smaller.

