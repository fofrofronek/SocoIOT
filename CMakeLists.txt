# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(SocoIOT C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_BOARD pico_w)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()


add_subdirectory(lib/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)

add_executable(${PROJECT_NAME} 
        main.cpp
        uart_dma.c
        lcd_fc.cpp
        SuperSocoParser.cpp
        lwip_iot.cpp
        ble.cpp
        st7789.c
        SuperSocoParser.cpp
        buttons.cpp
        LEDs.cpp
        SdConfData.cpp
        sd_hw.c
        ConfigParser.cpp
        )




# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
#WIFI_SSID=\"${WIFI_SSID}\"
#WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
#TEST_TCP_SERVER_IP=\"${TEST_TCP_SERVER_IP}\"
NO_SYS=1
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
#    ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
    $ENV{PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    #pico_cyw43_arch_none
    pico_multicore 
    hardware_uart
    hardware_spi
    hardware_rtc
#    pico_cyw43_arch_lwip_threadsafe_background     # we need Wifi to access the GPIO, but we don't need anything else
    pico_cyw43_arch_lwip_poll    
    pico_rand
    pico_lwip_nosys
    pico_lwip_mqtt
    pico_lwip_mbedtls
    pico_mbedtls
    pico_stdlib
    FatFs_SPI
)

# It doesn't make sense to run this program from flash. Always build a
# RAM-only binary.
#pico_set_binary_type(${PROJECT_NAME} copy_to_ram)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)



# pull in common dependencies and additional uart hardware support
#target_link_libraries(uart_advanced pico_stdlib hardware_uart)

# create map/bin/hex file etc.
#pico_add_extra_outputs(uart_advanced)

# add url via pico_set_program_url
#example_auto_set_url(uart_advanced)
