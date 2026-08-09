#include "mcu_touch_iic.h"
#include "board.h"
#include <stddef.h>

static TOUCH_BUS_OPS touch_iic_ops = {
    .init = mcu_touch_iic_init,
    .read_reg = mcu_touch_iic_read_reg,
    .write_reg = mcu_touch_iic_write_reg,
    .set_reset = mcu_touch_set_reset,
    .read_int = mcu_touch_read_int,
    .delay_ms = mcu_touch_delay_ms,
};

static void touch_iic_delay(void) {
    delay_us(TOUCH_IIC_DELAY_US);
}

static void touch_iic_scl(uint8_t level) {
    if (level) {
        GPIO_SetBits(TOUCH_IIC_SCL_GPIO_PORT, TOUCH_IIC_SCL_GPIO_PIN);
    } else {
        GPIO_ResetBits(TOUCH_IIC_SCL_GPIO_PORT, TOUCH_IIC_SCL_GPIO_PIN);
    }
}

static void touch_iic_sda(uint8_t level) {
    if (level) {
        GPIO_SetBits(TOUCH_IIC_SDA_GPIO_PORT, TOUCH_IIC_SDA_GPIO_PIN);
    } else {
        GPIO_ResetBits(TOUCH_IIC_SDA_GPIO_PORT, TOUCH_IIC_SDA_GPIO_PIN);
    }
}

static uint8_t touch_iic_read_sda(void) {
    return GPIO_ReadInputDataBit(TOUCH_IIC_SDA_GPIO_PORT, TOUCH_IIC_SDA_GPIO_PIN);
}

static void touch_iic_sda_mode(GPIOMode_TypeDef mode) {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = TOUCH_IIC_SDA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = mode;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_IIC_SDA_GPIO_PORT, &GPIO_InitStructure);
}

static void touch_iic_start(void) {
    touch_iic_sda_mode(GPIO_Mode_OUT);
    touch_iic_sda(1);
    touch_iic_scl(1);
    touch_iic_delay();
    touch_iic_sda(0);
    touch_iic_delay();
    touch_iic_scl(0);
}

static void touch_iic_stop(void) {
    touch_iic_sda_mode(GPIO_Mode_OUT);
    touch_iic_scl(0);
    touch_iic_sda(0);
    touch_iic_delay();
    touch_iic_scl(1);
    touch_iic_delay();
    touch_iic_sda(1);
    touch_iic_delay();
}

static uint8_t touch_iic_wait_ack(void) {
    uint8_t timeout = 250U;

    touch_iic_sda_mode(GPIO_Mode_OUT);
    touch_iic_sda(1);
    touch_iic_delay();
    touch_iic_scl(1);
    touch_iic_delay();

    while (touch_iic_read_sda()) {
        if (timeout-- == 0U) {
            touch_iic_stop();
            return TOUCH_ERROR_BUS;
        }
    }

    touch_iic_scl(0);
    return TOUCH_OK;
}

static void touch_iic_ack(uint8_t ack) {
    touch_iic_scl(0);
    touch_iic_sda_mode(GPIO_Mode_OUT);
    touch_iic_sda(ack ? 0U : 1U);
    touch_iic_delay();
    touch_iic_scl(1);
    touch_iic_delay();
    touch_iic_scl(0);
}

static uint8_t touch_iic_write_byte(uint8_t data) {
    touch_iic_sda_mode(GPIO_Mode_OUT);

    for (uint8_t i = 0; i < 8U; i++) {
        touch_iic_scl(0);
        touch_iic_sda((data & 0x80U) ? 1U : 0U);
        data <<= 1U;
        touch_iic_delay();
        touch_iic_scl(1);
        touch_iic_delay();
    }

    touch_iic_scl(0);
    return touch_iic_wait_ack();
}

static uint8_t touch_iic_read_byte(uint8_t ack) {
    uint8_t data = 0U;

    touch_iic_sda_mode(GPIO_Mode_OUT);
    touch_iic_sda(1);
    for (uint8_t i = 0; i < 8U; i++) {
        touch_iic_scl(0);
        touch_iic_delay();
        touch_iic_scl(1);
        data <<= 1U;
        if (touch_iic_read_sda()) {
            data |= 0x01U;
        }
        touch_iic_delay();
    }
    touch_iic_scl(0);
    touch_iic_ack(ack);

    return data;
}

static void touch_iic_bus_recovery(void) {
    touch_iic_sda_mode(GPIO_Mode_OUT);
    touch_iic_sda(1);
    touch_iic_scl(1);
    touch_iic_delay();

    for (uint8_t i = 0; i < 9U; i++) {
        if (touch_iic_read_sda()) {
            break;
        }
        touch_iic_scl(0);
        touch_iic_delay();
        touch_iic_scl(1);
        touch_iic_delay();
    }

    touch_iic_stop();
}

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

    GPIO_InitStructure.GPIO_Pin = TOUCH_IIC_SCL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TOUCH_IIC_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = TOUCH_IIC_SDA_GPIO_PIN;
    GPIO_Init(TOUCH_IIC_SDA_GPIO_PORT, &GPIO_InitStructure);

    touch_iic_scl(1);
    touch_iic_sda(1);
    touch_iic_bus_recovery();

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
    if ((data == NULL) || (len == 0U)) {
        return TOUCH_ERROR_PARAM;
    }

    for (uint8_t retry = 0U; retry < TOUCH_IIC_RETRY_COUNT; retry++) {
        touch_iic_start();
        if (touch_iic_write_byte((uint8_t)(dev_addr << 1U)) != TOUCH_OK) {
            touch_iic_stop();
            continue;
        }
        if (touch_iic_write_byte(reg) != TOUCH_OK) {
            touch_iic_stop();
            continue;
        }

        touch_iic_start();
        if (touch_iic_write_byte((uint8_t)((dev_addr << 1U) | 0x01U)) != TOUCH_OK) {
            touch_iic_stop();
            continue;
        }

        for (uint16_t i = 0; i < len; i++) {
            data[i] = touch_iic_read_byte((i + 1U) < len);
        }

        touch_iic_stop();
        return TOUCH_OK;
    }

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
    uint8_t status;

    if ((data == NULL) || (len == 0U)) {
        return TOUCH_ERROR_PARAM;
    }

    for (uint8_t retry = 0U; retry < TOUCH_IIC_RETRY_COUNT; retry++) {
        status = TOUCH_OK;

        touch_iic_start();
        if (touch_iic_write_byte((uint8_t)(dev_addr << 1U)) != TOUCH_OK) {
            touch_iic_stop();
            continue;
        }
        if (touch_iic_write_byte(reg) != TOUCH_OK) {
            touch_iic_stop();
            continue;
        }

        for (uint16_t i = 0; i < len; i++) {
            if (touch_iic_write_byte(data[i]) != TOUCH_OK) {
                touch_iic_stop();
                status = TOUCH_ERROR_BUS;
                break;
            }
        }
        if (status != TOUCH_OK) {
            continue;
        }

        touch_iic_stop();
        return TOUCH_OK;
    }

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
