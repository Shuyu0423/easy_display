#include "mcu_touch_iic.h"
#include "board.h"

static TOUCH_BUS_OPS touch_iic_ops = {
    .init = mcu_touch_iic_init,
    .read_reg = mcu_touch_iic_read_reg,
    .write_reg = mcu_touch_iic_write_reg,
    .set_reset = mcu_touch_set_reset,
    .read_int = mcu_touch_read_int,
    .delay_ms = mcu_touch_delay_ms,
};

/**
 * @function:   mcu_touch_iic_get_ops
 * @breif:      Get board touch IIC operation table.
 * @param:      NULL
 * @retval:     TOUCH_BUS_OPS pointer.
 */
TOUCH_BUS_OPS *mcu_touch_iic_get_ops(void) {
    return &touch_iic_ops;
}

/**
 * @function:   mcu_touch_iic_init
 * @breif:      Initialize touch IIC GPIO and control pins.
 * @param:      NULL
 * @retval:     TOUCH_STATUS.
 */
uint8_t mcu_touch_iic_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(TOUCH_IIC_SCL_GPIO_CLK | TOUCH_IIC_SDA_GPIO_CLK |
                               TOUCH_RST_GPIO_CLK | TOUCH_INT_GPIO_CLK,
                           ENABLE);

    GPIO_InitStructure.GPIO_Pin = TOUCH_IIC_SCL_GPIO_PIN | TOUCH_IIC_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_IIC_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TOUCH_RST_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_RST_GPIO_PORT, &GPIO_InitStructure);
    mcu_touch_set_reset(1);

    GPIO_InitStructure.GPIO_Pin = TOUCH_INT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(TOUCH_INT_GPIO_PORT, &GPIO_InitStructure);

    return TOUCH_OK;
}

/**
 * @function:       mcu_touch_iic_read_reg
 * @breif:          Read touch controller register through board IIC.
 * @param dev_addr: 7-bit IIC device address.
 * @param reg:      Register address.
 * @param data:     Read buffer.
 * @param len:      Read length.
 * @retval:         TOUCH_STATUS.
 */
uint8_t mcu_touch_iic_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len) {
    (void)dev_addr;
    (void)reg;
    (void)data;
    (void)len;

    /* TODO: implement software IIC or bind STM32 hardware I2C read transaction. */
    return TOUCH_ERROR_BUS;
}

/**
 * @function:       mcu_touch_iic_write_reg
 * @breif:          Write touch controller register through board IIC.
 * @param dev_addr: 7-bit IIC device address.
 * @param reg:      Register address.
 * @param data:     Write buffer.
 * @param len:      Write length.
 * @retval:         TOUCH_STATUS.
 */
uint8_t mcu_touch_iic_write_reg(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len) {
    (void)dev_addr;
    (void)reg;
    (void)data;
    (void)len;

    /* TODO: implement software IIC or bind STM32 hardware I2C write transaction. */
    return TOUCH_ERROR_BUS;
}

/**
 * @function:    mcu_touch_set_reset
 * @breif:       Set touch reset pin level.
 * @param level: Pin level, 0 low and non-zero high.
 * @retval:      NULL
 */
void mcu_touch_set_reset(uint8_t level) {
    if (level) {
        GPIO_SetBits(TOUCH_RST_GPIO_PORT, TOUCH_RST_GPIO_PIN);
    } else {
        GPIO_ResetBits(TOUCH_RST_GPIO_PORT, TOUCH_RST_GPIO_PIN);
    }
}

/**
 * @function:   mcu_touch_read_int
 * @breif:      Read touch interrupt pin level.
 * @param:      NULL
 * @retval:     Interrupt pin level.
 */
uint8_t mcu_touch_read_int(void) {
    return GPIO_ReadInputDataBit(TOUCH_INT_GPIO_PORT, TOUCH_INT_GPIO_PIN);
}

/**
 * @function:   mcu_touch_delay_ms
 * @breif:      Delay helper for touch driver.
 * @param ms:   Delay time in milliseconds.
 * @retval:     NULL
 */
void mcu_touch_delay_ms(uint32_t ms) {
    delay_ms(ms);
}
