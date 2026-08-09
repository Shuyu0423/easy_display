set(STM32F4_DEMO_SOURCES
    app/main.c
    board/board.c
    board/mcu_flash.c
    board/mcu_spi.c
    board/mcu_touch_iic.c
    board/mcu_touch_spi.c
    board/syscalls_gcc.c
    board/time_tick.c
    bsp/uart/bsp_uart.c
    module/stm32f4xx_it.c
    libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    libraries/STM32F4xx_StdPeriph_Driver/src/misc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cec.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dfsdm.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dsi.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash_ramfunc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmpi2c.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_lptim.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spdifrx.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c
    libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c
    ${EASY_DISPLAY_ROOT}/driver_pack/command/src/command.c
    ${EASY_DISPLAY_ROOT}/driver_pack/input/touch/core/src/touch_driver.c
    ${EASY_DISPLAY_ROOT}/driver_pack/input/touch/controller/cst816/src/cst816.c
    ${EASY_DISPLAY_ROOT}/driver_pack/input/touch/controller/xpt2046/src/xpt2046.c
    ${EASY_DISPLAY_ROOT}/driver_pack/lcd/src/epd.c
    ${EASY_DISPLAY_ROOT}/driver_pack/lcd/src/ili9341.c
    ${EASY_DISPLAY_ROOT}/driver_pack/lcd/src/lcd_driver.c
    ${EASY_DISPLAY_ROOT}/driver_pack/lcd/src/lcd_ui.c
    ${EASY_DISPLAY_ROOT}/driver_pack/lcd/src/st7789.c
    ${EASY_DISPLAY_ROOT}/driver_pack/log/backend/rtt/src/log_backend_rtt.c
    ${EASY_DISPLAY_ROOT}/driver_pack/log/src/sc_log.c
    ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt/SEGGER_RTT.c
    ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt/SEGGER_RTT_printf.c
    ${EASY_DISPLAY_ROOT}/driver_pack/tool/src/c_unit.c
    ${EASY_DISPLAY_ROOT}/driver_pack/tool/src/cbtimer.c
    ${EASY_DISPLAY_ROOT}/tests/target/touch/cst816_calibration_test.c
    ${EASY_DISPLAY_ROOT}/tests/target/touch/ili9341_xpt2046_test.c
)

set(STM32F4_DEMO_INCLUDE_DIRS
    app
    board
    bsp
    bsp/uart
    module
    libraries/CMSIS/Include
    libraries/CMSIS/Device/ST/STM32F4xx/Include
    libraries/STM32F4xx_StdPeriph_Driver/inc
    ${EASY_DISPLAY_ROOT}/driver_pack
    ${EASY_DISPLAY_ROOT}/driver_pack/command/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/input/touch/core/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/input/touch/controller/cst816/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/input/touch/controller/xpt2046/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/lcd/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/log/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/log/backend/rtt/inc
    ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt
    ${EASY_DISPLAY_ROOT}/driver_pack/tool/inc
    ${EASY_DISPLAY_ROOT}/tests/target/touch
)

set(STM32F4_DEMO_DEFINES
    USE_STDPERIPH_DRIVER
    STM32F40_41xxx
)

if(EASY_DISPLAY_BUILD_STM32F4_OBJECTS)
    add_library(easy_display_stm32f4_demo_objects OBJECT
        ${STM32F4_DEMO_SOURCES}
    )
    target_compile_definitions(easy_display_stm32f4_demo_objects PRIVATE
        ${STM32F4_DEMO_DEFINES}
    )
    target_include_directories(easy_display_stm32f4_demo_objects PRIVATE
        ${STM32F4_DEMO_INCLUDE_DIRS}
    )
endif()

if(EASY_DISPLAY_BUILD_STM32F4_FIRMWARE)
    set(STM32F4_LINKER_SCRIPT ${EASY_DISPLAY_ROOT}/cmake/stm32f40_41xxx_flash.ld)
    set(STM32F4_HEX ${CMAKE_CURRENT_BINARY_DIR}/easy_display_stm32f4.hex)
    set(STM32F4_BIN ${CMAKE_CURRENT_BINARY_DIR}/easy_display_stm32f4.bin)
    set(STM32F4_MAP ${CMAKE_CURRENT_BINARY_DIR}/easy_display_stm32f4.map)
    set(STM32F4_JLINK_DEVICE "STM32F407VE" CACHE STRING "J-Link device name")
    set(STM32F4_OPENOCD_INTERFACE "interface/stlink.cfg" CACHE STRING "OpenOCD interface cfg")
    set(STM32F4_OPENOCD_TARGET "target/stm32f4x.cfg" CACHE STRING "OpenOCD target cfg")
    set(STM32F4_STARTUP
        libraries/CMSIS/Device/ST/STM32F4xx/Source/Templates/SW4STM32/startup_stm32f40xx.s
    )
    find_program(JLINK_EXE
        NAMES JLink.exe
        HINTS
            "C:/Program Files/SEGGER/JLink_V870"
            "C:/Program Files (x86)/SEGGER/JLinkARM_V482"
            "C:/Program Files (x86)/SEGGER/JLink_V634b"
    )
    find_program(OPENOCD_EXE NAMES openocd.exe openocd)

    add_executable(easy_display_stm32f4
        ${STM32F4_STARTUP}
        ${STM32F4_DEMO_SOURCES}
    )
    set_target_properties(easy_display_stm32f4 PROPERTIES
        OUTPUT_NAME easy_display_stm32f4
        SUFFIX ".elf"
    )

    target_compile_definitions(easy_display_stm32f4 PRIVATE
        ${STM32F4_DEMO_DEFINES}
    )
    target_include_directories(easy_display_stm32f4 PRIVATE
        ${STM32F4_DEMO_INCLUDE_DIRS}
    )

    target_compile_options(easy_display_stm32f4 PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfloat-abi=soft
        -ffunction-sections
        -fdata-sections
        -Wall
        -Wno-unused-parameter
        -Wno-missing-braces
    )

    target_link_options(easy_display_stm32f4 PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfloat-abi=soft
        -specs=nano.specs
        -specs=nosys.specs
        -T${STM32F4_LINKER_SCRIPT}
        -Wl,-Map=${STM32F4_MAP}
        -Wl,--gc-sections
        -Wl,--no-warn-rwx-segments
    )

    add_custom_command(TARGET easy_display_stm32f4 POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:easy_display_stm32f4>
            ${STM32F4_HEX}
        COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:easy_display_stm32f4>
            ${STM32F4_BIN}
        COMMAND ${CMAKE_SIZE}
            $<TARGET_FILE:easy_display_stm32f4>
        VERBATIM
    )

    set(STM32F4_JLINK_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/flash_jlink.jlink)
    file(GENERATE OUTPUT ${STM32F4_JLINK_SCRIPT} CONTENT
"r
h
loadfile \"${STM32F4_HEX}\"
r
g
q
")

    if(JLINK_EXE)
        add_custom_target(flash-jlink
            COMMAND ${JLINK_EXE}
                -device ${STM32F4_JLINK_DEVICE}
                -if SWD
                -speed 4000
                -autoconnect 1
                -CommanderScript ${STM32F4_JLINK_SCRIPT}
            DEPENDS easy_display_stm32f4
            USES_TERMINAL
            VERBATIM
        )
    endif()

    if(OPENOCD_EXE)
        add_custom_target(flash-openocd
            COMMAND ${OPENOCD_EXE}
                -f ${STM32F4_OPENOCD_INTERFACE}
                -f ${STM32F4_OPENOCD_TARGET}
                -c "program ${STM32F4_HEX} verify reset exit"
            DEPENDS easy_display_stm32f4
            USES_TERMINAL
            VERBATIM
        )
        add_custom_target(flash
            DEPENDS flash-openocd
        )
    elseif(JLINK_EXE)
        add_custom_target(flash
            DEPENDS flash-jlink
        )
    endif()
endif()
