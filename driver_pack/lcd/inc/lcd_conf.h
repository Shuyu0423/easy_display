/**
 * @file: lcd_conf.h
 * @brief: configuration for the LCD/EPD driver module
 */

#ifndef LCD_CONF_H__
#define LCD_CONF_H__

#include <stdint.h>

#define AGREEMENT_ENABLE (1)

#define USE_EPD (0)
#define USE_LCD (1)

#if AGREEMENT_ENABLE

#define NOT_AGREEMENT (1 << 0)
#define AGREEMENT_SPI (1 << 1)
#define AGREEMENT_IIC (1 << 2)
#define AGREEMENT_ALL (AGREEMENT_IIC | AGREEMENT_SPI)
#define AGREEMENT_TARGETS (AGREEMENT_ALL)

#if (AGREEMENT_TARGETS & AGREEMENT_SPI)

#define DELAY_MS(tick) \
    do { \
        uint32_t start = get_system_tick(); \
        while ((uint32_t)(get_system_tick() - start) < (tick)); \
    } while (0)

#define SET_SCREEN_ORIENTATION (0)

#if (SET_SCREEN_ORIENTATION == 0) || (SET_SCREEN_ORIENTATION == 1)
#define LCD_W LCD_WIDTH
#define LCD_H LCD_HEIGHT
#else
#define LCD_W LCD_HEIGHT
#define LCD_H LCD_WIDTH
#endif

#if USE_EPD
#define LCD_WIDTH (152)
#define LCD_HEIGHT (152)
#else
#define LCD_WIDTH (240)
#define LCD_HEIGHT (284)
#endif

#define AGREEMENT_SPI_ENABLE

#define LCD_GET_TICK_FUNC get_heart_tick_time
#define get_system_tick LCD_GET_TICK_FUNC

#define SPI_INIT spi_gpio_init
#define SPI_SWAP_BYTE spi_transbyte
#define SPI_SET_CS spi_set_cs
#define SPI_SET_DC spi_set_dc

#define NEED_JUD_BUSY (1)
#define NEED_SIPPORT_RESET (1)
#define NEED_SPIPORT_BL (1)

#if NEED_JUD_BUSY
#define SPI_READ_BUSY spi_read_busy
#endif

#if NEED_SIPPORT_RESET
#define SPI_SET_RESET spi_set_reset
#endif

#if NEED_SPIPORT_BL
#define SPI_SET_BLE spi_set_bl
#endif

uint32_t LCD_GET_TICK_FUNC(void);
void SPI_INIT(void);
uint8_t SPI_SWAP_BYTE(uint8_t data);
void SPI_SET_CS(uint8_t level);
void SPI_SET_DC(uint8_t level);

#if NEED_JUD_BUSY
uint8_t SPI_READ_BUSY(void);
#endif

#if NEED_SIPPORT_RESET
void SPI_SET_RESET(uint8_t level);
#endif

#if NEED_SPIPORT_BL
void SPI_SET_BLE(uint8_t level);
#endif

#endif

#if (AGREEMENT_TARGETS & AGREEMENT_IIC)
#define AGREEMENT_IIC_ENABLE

#define SSD1306_ENABLE (0)

#endif

#endif

#endif
