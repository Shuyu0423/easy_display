find_program(EASY_DISPLAY_HOST_CC NAMES gcc clang cc)
if(NOT EASY_DISPLAY_HOST_CC)
    set(EASY_DISPLAY_HOST_CC ${CMAKE_C_COMPILER})
endif()

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/log_host_syntax.stamp
    COMMAND ${EASY_DISPLAY_HOST_CC}
        -std=c99
        -fsyntax-only
        -I${EASY_DISPLAY_ROOT}/driver_pack/log/inc
        -I${EASY_DISPLAY_ROOT}/driver_pack/log/backend/rtt/inc
        -I${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt
        ${EASY_DISPLAY_ROOT}/driver_pack/log/tests/host/test_log_rtt_smoke.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/src/sc_log.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/backend/rtt/src/log_backend_rtt.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt/SEGGER_RTT.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt/SEGGER_RTT_printf.c
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/log_host_syntax.stamp
    DEPENDS
        ${EASY_DISPLAY_ROOT}/driver_pack/log/tests/host/test_log_rtt_smoke.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/src/sc_log.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/backend/rtt/src/log_backend_rtt.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt/SEGGER_RTT.c
        ${EASY_DISPLAY_ROOT}/driver_pack/log/third_party/segger_rtt/SEGGER_RTT_printf.c
    VERBATIM
)

add_custom_target(log_host_syntax ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/log_host_syntax.stamp
)

add_subdirectory(touch)

add_custom_target(host_tests
    DEPENDS
        log_host_syntax
        test_touch_cst816
        test_touch_xpt2046
)
