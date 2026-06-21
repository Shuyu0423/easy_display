/*
 * @Author       : SuperYu 824229900@qq.com
 * @Date         : 2025-07-06 16:43
 * @LastEditors  : Please set LastEditors
 * @LastEditTime : 2025-09-18 23:22
 * @Description  : 
 */
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
#include "board.h"

/**内部函数*/
#define get_system_tick get_heart_tick_time

#endif
