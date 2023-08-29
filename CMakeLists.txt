cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(modbus_v2 C CXX ASM)

set(CMAKE_C_STANDART 11)
set(CMAKE_CXX_STANDART 17)

include_directories(./ ./sensors)
set(SOURCES main.cpp
    modbus.cpp modbus.hpp ModbusPico.cpp ModbusPico.hpp
    sensors/ds18b20.cpp sensors/ds18b20.hpp
    sensors/one_wire.cpp sensors/one_wire.h)

pico_sdk_init()

add_executable(
    ${PROJECT_NAME}
    ${SOURCES}
)


# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})

# pull in common dependencies
target_link_libraries(
    ${PROJECT_NAME}
    pico_stdlib
    pico_multicore
    hardware_adc
)

pico_enable_stdio_usb(  ${PROJECT_NAME} 1)
pico_enable_stdio_uart( ${PROJECT_NAME} 0)
