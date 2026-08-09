#ifndef MCU_TOUCH_IIC_H__
#define MCU_TOUCH_IIC_H__

#include "stm32f4xx.h"
#include "touch_driver.h"

#define TOUCH_IIC_SCL_GPIO_PORT GPIOB
#define TOUCH_IIC_SCL_GPIO_PIN GPIO_Pin_6
#define TOUCH_IIC_SCL_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_IIC_SDA_GPIO_PORT GPIOB
#define TOUCH_IIC_SDA_GPIO_PIN GPIO_Pin_7
#define TOUCH_IIC_SDA_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_RST_GPIO_PORT GPIOB
#define TOUCH_RST_GPIO_PIN GPIO_Pin_8
#define TOUCH_RST_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_INT_GPIO_PORT GPIOB
#define TOUCH_INT_GPIO_PIN GPIO_Pin_9
#define TOUCH_INT_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_IIC_DELAY_US (10U)
#define TOUCH_IIC_RETRY_COUNT (3U)

TOUCH_BUS_OPS *mcu_touch_iic_get_ops(void);
uint8_t mcu_touch_iic_init(void);
uint8_t mcu_touch_iic_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
uint8_t mcu_touch_iic_write_reg(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len);
void mcu_touch_set_reset(uint8_t level);
uint8_t mcu_touch_read_int(void);
void mcu_touch_delay_ms(uint32_t ms);

#endif
