/*
 * @file         : mcu_spi.c
 * @Author       : shuyu
 * @LastEditTime : 2025-08-17 23:07
 * @Description  : spi底层驱动，示例代码使用软件SPI
 */
#include "mcu_spi.h"

/**
 * @function:   spi_gpio_init
 * @breif:      Initialize GPIO pins used by software SPI display bus.
 * @param:      NULL
 * @retval:     NULL
 */
void spi_gpio_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(SPI_SCL_GPIO_CLK | SPI_SDA_GPIO_CLK | SPI_RES_GPIO_CLK | SPI_DC_GPIO_CLK
                               | SPI_CS_GPIO_CLK | SPI_BUSY_GPIO_CLK | SPI_BL_GPIO_CLK,
                           ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI_SCL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI_SCL_GPIO_PORT, &GPIO_InitStructure);
    SPI_SCL_Set();

    GPIO_InitStructure.GPIO_Pin = SPI_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI_SDA_GPIO_PORT, &GPIO_InitStructure);
    SPI_SDA_Set();

    GPIO_InitStructure.GPIO_Pin = SPI_RES_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI_RES_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI_DC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI_DC_GPIO_PORT, &GPIO_InitStructure);
    SPI_DC_Set();

    GPIO_InitStructure.GPIO_Pin = SPI_CS_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI_CS_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI_BUSY_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SPI_BUSY_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI_BL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(SPI_BL_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @function:   spi_set_cs
 * @breif:      Set SPI chip-select pin level.
 * @param mode: Pin level, 0 low and non-zero high.
 * @retval:     NULL
 */
void spi_set_cs(uint8_t mode) {
    if (mode) {
        GPIO_SetBits(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN);
    } else {
        GPIO_ResetBits(SPI_CS_GPIO_PORT, SPI_CS_GPIO_PIN);
    }
}

/**
 * @function:   spi_set_dc
 * @breif:      Set display data/command pin level.
 * @param mode: Pin level, 0 command and non-zero data.
 * @retval:     NULL
 */
void spi_set_dc(uint8_t mode) {
    if (mode) {
        GPIO_SetBits(SPI_DC_GPIO_PORT, SPI_DC_GPIO_PIN);
    } else {
        GPIO_ResetBits(SPI_DC_GPIO_PORT, SPI_DC_GPIO_PIN);
    }
}

/**
 * @function:   spi_read_busy
 * @breif:      Read display busy pin level.
 * @param:      NULL
 * @retval:     Busy pin level.
 */
uint8_t spi_read_busy(void) {
    return GPIO_ReadInputDataBit(SPI_BUSY_GPIO_PORT, SPI_BUSY_GPIO_PIN);
}

/**
 * @function:   spi_set_reset
 * @breif:      Set display reset pin level.
 * @param mode: Pin level, 0 low and non-zero high.
 * @retval:     NULL
 */
void spi_set_reset(uint8_t mode) {
    if (mode) {
        GPIO_SetBits(SPI_RES_GPIO_PORT, SPI_RES_GPIO_PIN);
    } else {
        GPIO_ResetBits(SPI_RES_GPIO_PORT, SPI_RES_GPIO_PIN);
    }
}

/**
 * @function:   spi_set_bl
 * @breif:      Set display backlight pin level.
 * @param mode: Pin level, 0 low and non-zero high.
 * @retval:     NULL
 */
void spi_set_bl(uint8_t mode) {
    if (mode) {
        GPIO_SetBits(SPI_BL_GPIO_PORT, SPI_BL_GPIO_PIN);
    } else {
        GPIO_ResetBits(SPI_BL_GPIO_PORT, SPI_BL_GPIO_PIN);
    }
}

/**
 * @function:   spi_transbyte
 * @breif:      Send one byte through software SPI.
 * @param dat:  Byte to send.
 * @retval:     Received byte, unused by current display bus.
 */
uint8_t spi_transbyte(uint8_t dat) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        SPI_SCL_Clr();
        if (dat & 0x80) {
            SPI_SDA_Set();
        } else {
            SPI_SDA_Clr();
        }
        SPI_SCL_Set();
        dat <<= 1;
    }
    return 0;
}
