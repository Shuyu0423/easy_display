#include "mcu_touch_spi.h"
#include "board.h"
#include <stddef.h>

static TOUCH_BUS_OPS touch_spi_ops = {
    .init = mcu_touch_spi_init,
    .transfer = mcu_touch_spi_transfer,
    .read_int = mcu_touch_spi_read_int,
    .delay_ms = mcu_touch_spi_delay_ms,
};

static void touch_spi_delay(void) {
    delay_us(TOUCH_SPI_DELAY_US);
}

static void touch_spi_scl(uint8_t level) {
    if (level) {
        GPIO_SetBits(TOUCH_SPI_SCL_GPIO_PORT, TOUCH_SPI_SCL_GPIO_PIN);
    } else {
        GPIO_ResetBits(TOUCH_SPI_SCL_GPIO_PORT, TOUCH_SPI_SCL_GPIO_PIN);
    }
}

static void touch_spi_mosi(uint8_t level) {
    if (level) {
        GPIO_SetBits(TOUCH_SPI_MOSI_GPIO_PORT, TOUCH_SPI_MOSI_GPIO_PIN);
    } else {
        GPIO_ResetBits(TOUCH_SPI_MOSI_GPIO_PORT, TOUCH_SPI_MOSI_GPIO_PIN);
    }
}

static uint8_t touch_spi_miso(void) {
    return GPIO_ReadInputDataBit(TOUCH_SPI_MISO_GPIO_PORT, TOUCH_SPI_MISO_GPIO_PIN);
}

static void touch_spi_cs(uint8_t level) {
    if (level) {
        GPIO_SetBits(TOUCH_SPI_CS_GPIO_PORT, TOUCH_SPI_CS_GPIO_PIN);
    } else {
        GPIO_ResetBits(TOUCH_SPI_CS_GPIO_PORT, TOUCH_SPI_CS_GPIO_PIN);
    }
}

static uint8_t touch_spi_transfer_byte(uint8_t data) {
    uint8_t rx = 0U;

    for (uint8_t i = 0U; i < 8U; i++) {
        touch_spi_scl(0);
        touch_spi_mosi((data & 0x80U) ? 1U : 0U);
        data <<= 1U;
        touch_spi_delay();

        touch_spi_scl(1);
        rx <<= 1U;
        if (touch_spi_miso()) {
            rx |= 0x01U;
        }
        touch_spi_delay();
    }

    touch_spi_scl(0);
    return rx;
}

/**
 * @function:   mcu_touch_spi_get_ops
 * @breif:      Get board touch SPI operation table.
 * @param:      NULL
 * @retval:     TOUCH_BUS_OPS pointer.
 */
TOUCH_BUS_OPS *mcu_touch_spi_get_ops(void) {
    return &touch_spi_ops;
}

/**
 * @function:   mcu_touch_spi_init
 * @breif:      Initialize touch SPI GPIO pins.
 * @param:      NULL
 * @retval:     TOUCH_STATUS.
 */
uint8_t mcu_touch_spi_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(TOUCH_SPI_SCL_GPIO_CLK | TOUCH_SPI_MOSI_GPIO_CLK |
                               TOUCH_SPI_MISO_GPIO_CLK | TOUCH_SPI_CS_GPIO_CLK |
                               TOUCH_SPI_IRQ_GPIO_CLK,
                           ENABLE);

    GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_SCL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_SPI_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_MOSI_GPIO_PIN;
    GPIO_Init(TOUCH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_CS_GPIO_PIN;
    GPIO_Init(TOUCH_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_MISO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(TOUCH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TOUCH_SPI_IRQ_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(TOUCH_SPI_IRQ_GPIO_PORT, &GPIO_InitStructure);

    touch_spi_cs(1);
    touch_spi_scl(0);
    touch_spi_mosi(0);

    return TOUCH_OK;
}

/**
 * @function:        mcu_touch_spi_transfer
 * @breif:           Transfer bytes through board touch SPI.
 * @param tx_data:   TX buffer, NULL sends zeroes.
 * @param rx_data:   RX buffer, NULL discards received bytes.
 * @param len:       Transfer length in bytes.
 * @retval:          TOUCH_STATUS.
 */
uint8_t mcu_touch_spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len) {
    uint8_t tx;
    uint8_t rx;

    if (len == 0U) {
        return TOUCH_ERROR_PARAM;
    }

    touch_spi_cs(0);
    for (uint16_t i = 0U; i < len; i++) {
        tx = (tx_data != NULL) ? tx_data[i] : 0x00U;
        rx = touch_spi_transfer_byte(tx);
        if (rx_data != NULL) {
            rx_data[i] = rx;
        }
    }
    touch_spi_cs(1);

    return TOUCH_OK;
}

/**
 * @function:   mcu_touch_spi_read_int
 * @breif:      Read XPT2046 PENIRQ pin level.
 * @param:      NULL
 * @retval:     Interrupt pin level, low means pressed.
 */
uint8_t mcu_touch_spi_read_int(void) {
    return GPIO_ReadInputDataBit(TOUCH_SPI_IRQ_GPIO_PORT, TOUCH_SPI_IRQ_GPIO_PIN);
}

/**
 * @function:   mcu_touch_spi_delay_ms
 * @breif:      Delay helper for touch SPI driver.
 * @param ms:   Delay time in milliseconds.
 * @retval:     NULL
 */
void mcu_touch_spi_delay_ms(uint32_t ms) {
    delay_ms(ms);
}
