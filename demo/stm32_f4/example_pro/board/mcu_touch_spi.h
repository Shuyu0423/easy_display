#ifndef MCU_TOUCH_SPI_H__
#define MCU_TOUCH_SPI_H__

#include "stm32f4xx.h"
#include "touch_driver.h"

#define TOUCH_SPI_SCL_GPIO_PORT GPIOB
#define TOUCH_SPI_SCL_GPIO_PIN GPIO_Pin_13
#define TOUCH_SPI_SCL_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_SPI_MOSI_GPIO_PORT GPIOB
#define TOUCH_SPI_MOSI_GPIO_PIN GPIO_Pin_15
#define TOUCH_SPI_MOSI_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_SPI_MISO_GPIO_PORT GPIOB
#define TOUCH_SPI_MISO_GPIO_PIN GPIO_Pin_14
#define TOUCH_SPI_MISO_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_SPI_CS_GPIO_PORT GPIOB
#define TOUCH_SPI_CS_GPIO_PIN GPIO_Pin_12
#define TOUCH_SPI_CS_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_SPI_IRQ_GPIO_PORT GPIOB
#define TOUCH_SPI_IRQ_GPIO_PIN GPIO_Pin_10
#define TOUCH_SPI_IRQ_GPIO_CLK RCC_AHB1Periph_GPIOB

#define TOUCH_SPI_DELAY_US (1U)

TOUCH_BUS_OPS *mcu_touch_spi_get_ops(void);
uint8_t mcu_touch_spi_init(void);
uint8_t mcu_touch_spi_transfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len);
uint8_t mcu_touch_spi_read_int(void);
void mcu_touch_spi_delay_ms(uint32_t ms);

#endif
