cmake_minimum_required(VERSION 3.20)

project(easy_display C ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(EASY_DISPLAY_BUILD_HOST_TESTS "Build host-side smoke tests" ON)
option(EASY_DISPLAY_BUILD_STM32F4_OBJECTS "Build STM32F4 demo object targets" OFF)
option(EASY_DISPLAY_BUILD_STM32F4_FIRMWARE "Build STM32F4 firmware image" OFF)
option(EASY_DISPLAY_BUILD_STM32F4_PLATFORM "Build reusable STM32F4 board support library" OFF)

set(EASY_DISPLAY_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(easy_display STATIC
    src/display.c
    src/input.c
)
add_library(easy_display::easy_display ALIAS easy_display)
target_include_directories(easy_display PUBLIC
    $<BUILD_INTERFACE:${EASY_DISPLAY_ROOT}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(easy_display PUBLIC c_std_99)

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

add_library(easy_display_lcd_ili9341 STATIC
    driver_pack/lcd/src/lcd_driver.c
    driver_pack/lcd/src/ili9341.c
)
target_include_directories(easy_display_lcd_ili9341 PUBLIC
    driver_pack/lcd/inc
)

add_library(easy_display_ili9341_device STATIC
    driver_pack/adapter/src/ili9341_easy_display.c
)
target_include_directories(easy_display_ili9341_device PUBLIC
    driver_pack/adapter/inc
)
target_link_libraries(easy_display_ili9341_device PUBLIC
    easy_display
    easy_display_lcd_ili9341
)

add_library(easy_display_xpt2046_device STATIC
    driver_pack/adapter/src/xpt2046_easy_pointer.c
)
target_include_directories(easy_display_xpt2046_device PUBLIC
    driver_pack/adapter/inc
)
target_link_libraries(easy_display_xpt2046_device PUBLIC
    easy_display
    easy_display_touch_xpt2046
)

if(EASY_DISPLAY_BUILD_HOST_TESTS)
    enable_testing()
    add_subdirectory(tests/host)
endif()

if(EASY_DISPLAY_BUILD_STM32F4_OBJECTS OR EASY_DISPLAY_BUILD_STM32F4_FIRMWARE OR
   EASY_DISPLAY_BUILD_STM32F4_PLATFORM)
    add_subdirectory(demo/stm32_f4/example_pro)
endif()
