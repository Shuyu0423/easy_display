/**
 * @file: user_conf.h
 * @brief: 用户注册头文件，在此进行注册
 * @info: v1.0 2025-7-6     @sy
 * @note: 给用户提供结接口的头文件
 */

#ifndef USER_CONF_H__
#define USER_CONF_H__

/*内部文件*/
#include "c_unit.h"
#include "cbtimer.h"

/**输入你的文件信息 */
#include "stm32f4xx.h"
#include "bsp_uart.h"
#include "mcu_flash.h"
#include "mcu_spi.h"
#include "mcu_touch_spi.h"
#include "board.h"

/**内部函数*/
#define get_system_tick get_heart_tick_time

/* Set to 1 to run the CST816 touch calibration test from app/main.c. */
#ifndef TOUCH_CST816_CALIBRATION_TEST_ENABLE
#define TOUCH_CST816_CALIBRATION_TEST_ENABLE (0)
#endif

/* Set to 1 to run the ILI9341 + XPT2046 board test from app/main.c. */
#ifndef ILI9341_XPT2046_TEST_ENABLE
#define ILI9341_XPT2046_TEST_ENABLE (1)
#endif

#endif
