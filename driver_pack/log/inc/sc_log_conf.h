/**
 * @file: sc_log_conf.h
 * @brief: configuration for the standalone log module
 */

#ifndef SC_LOG_CONF_H__
#define SC_LOG_CONF_H__

#include <stdint.h>

#ifdef _WIN32
#define ALIGN_4 __declspec(align(4))
#else
#define ALIGN_4 __attribute__((aligned(4)))
#endif

/****************************** LOG ******************************/

#define DEBUG_ENABLE (1)
#define LOG_PATH_ENABLE (1)

enum { INFO, ERR, WARN };

#if DEBUG_ENABLE

#define NOT_SUPPORT (1 << 0)
#define DEBUG_RTT_BIT (1 << 1)
#define DEBUG_UART_BIT (1 << 2)
#define DEBUG_FLASH_BIT (1 << 3)
#define DEBG_ALL_BIT (DEBUG_UART_BIT | DEBUG_RTT_BIT | DEBUG_FLASH_BIT)
#define DEBUG_TARGETS (DEBUG_RTT_BIT)

#if (DEBUG_TARGETS & DEBUG_RTT_BIT)
#define ERR_TERMINAL_ID (0)
#define ERR_CHANNEL_INDEX (0)
#define INFO_TERMINAL_ID (1)
#define INFO_CHANNEL_INDEX (0)
#define RAW_TERMINAL_ID (2)
#define RAW_CHANNEL_INDEX (0)
#define WARN_TERMINAL_ID (3)
#define WARN_CHANNEL_INDEX (0)

#define RTT_DEBUG_ENABLE
#endif

#if (DEBUG_TARGETS & DEBUG_UART_BIT)
#define UART_DEBUG_ENABLE

#define SEND_CHAR send_char
#define SUPPORT_DEBUG_LEN (128)

void SEND_CHAR(uint8_t data);
#endif

#if (DEBUG_TARGETS & DEBUG_FLASH_BIT)
#define FLASH_DEBUG_ENABLE

#define SUPPORT_MAX_WRITE_SIZE (16)
#define WRITE_FLAG (0xAA)

#define FLASH_USER_START_ADDR (0x08000000ul)
#define FLASH_APP_SIZE (1024 * 100)
#define FLASH_LOG_START_ADDR (0x08040000)
#define FLASH_START_ADDR (FLASH_LOG_START_ADDR)
#define FLAHS_SUPPOR_SIZE (1024 * 6)
#define FLASH_SECTOR_SIZE (FLAHS_SUPPOR_SIZE / 3)

#define FLASH_WRITE_FUNC mcu_flash_wirte
#define FLASH_READ_FUNC mcu_flash_read
#define FLASH_ERASE_FUNC mcu_flash_erase
#define LOG_TICK_FUNC TickVal

uint8_t FLASH_WRITE_FUNC(uint32_t addr, uint32_t *write_buff, uint32_t write_len);
uint8_t FLASH_READ_FUNC(uint32_t addr, uint32_t *read_buff, uint32_t read_len);
uint8_t FLASH_ERASE_FUNC(uint32_t addr, uint32_t erase_len);
uint32_t LOG_TICK_FUNC(void);

enum {
    LOG_OK,
    LOG_ERROR,
    LOG_INFO_SIZE_FULL,
    LOG_ERR_SIZE_FULL,
    LOG_RAWN_SIZE_FULL,
    LOG_INFO_FAIL,
    LOG_ERR_FAIL,
    LOG_WARN_FAIL,
    LOG_INFO_INDX_LARGER,
    LOG_ERR_INDX_LARGER,
    LOG_WARN_INDX_LARGER,
};

#endif

#if (DEBUG_TARGETS & NOT_SUPPORT)
#warning "Debug functionality is disabled"
#endif

#endif

/*************************** RING BUFFER ***************************/

#define RING_BUFFER_ENABLE (1)

#if RING_BUFFER_ENABLE
#define RING_BUFFER_SIZE (256)
#define PUSH_CHAR (0)
#define PUSH_STRING (1)
#endif

#endif
