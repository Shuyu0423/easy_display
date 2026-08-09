cmake_minimum_required(VERSION 3.20)

project(easy_display C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(EASY_DISPLAY_BUILD_HOST_TESTS "Build host-side smoke tests" ON)
option(EASY_DISPLAY_BUILD_STM32F4_OBJECTS "Build STM32F4 demo object targets" OFF)
option(EASY_DISPLAY_BUILD_STM32F4_FIRMWARE "Build STM32F4 firmware image" OFF)

set(EASY_DISPLAY_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(easy_display_touch_core STATIC
    driver_pack/input/touch/core/src/touch_driver.c
)
target_include_directories(easy_display_touch_core PUBLIC
    driver_pack/input/touch/core/inc
)

add_library(easy_display_touch_cst816 STATIC
    driver_pack/input/touch/controller/cst816/src/cst816.c
)
target_include_directories(easy_display_touch_cst816 PUBLIC
    driver_pack/input/touch/controller/cst816/inc
)
target_link_libraries(easy_display_touch_cst816 PUBLIC
    easy_display_touch_core
)

add_library(easy_display_touch_xpt2046 STATIC
    driver_pack/input/touch/controller/xpt2046/src/xpt2046.c
)
target_include_directories(easy_display_touch_xpt2046 PUBLIC
    driver_pack/input/touch/controller/xpt2046/inc
)
target_link_libraries(easy_display_touch_xpt2046 PUBLIC
    easy_display_touch_core
)

if(EASY_DISPLAY_BUILD_HOST_TESTS)
    enable_testing()
    add_subdirectory(tests/host)
endif()

if(EASY_DISPLAY_BUILD_STM32F4_OBJECTS OR EASY_DISPLAY_BUILD_STM32F4_FIRMWARE)
    add_subdirectory(demo/stm32_f4/example_pro)
endif()
