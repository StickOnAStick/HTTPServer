cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "/home/nick/School/CMPE189/HTTPServer/pico-sdk/" CACHE STRING "Path to the Pico SDK")


set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(pico_http_server C CXX ASM)

pico_sdk_init()

message("Running generate_fsdata python script")
execute_process(COMMAND
    python3 ${CMAKE_CURRENT_LIST_DIR}/generate_fsdata.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(pico_http_server
    main.c
    src/core0.c
    src/core1.c
    src/queue.c
    src/request.c
    src/response.c
    src/generated_fs.c
)

target_include_directories(pico_http_server PRIVATE
    inc
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(pico_http_server
    pico_stdlib
    pico_multicore
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_cyw43_driver
    pico_lwip
)


pico_enable_stdio_usb(pico_http_server 1)
pico_enable_stdio_uart(pico_http_server 0)

pico_add_extra_outputs(pico_http_server)


