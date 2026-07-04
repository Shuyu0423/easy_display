#include "lcd_driver.h"
#include <stddef.h>

static SPI_DEV spi_dev = {.spi_init = SPI_INIT,
                          .spi_transbyte = SPI_SWAP_BYTE,
                          .set_cs = SPI_SET_CS,
                          .set_dc = SPI_SET_DC};

/**
 * @function:   spi_write_cmd
 * @breif:      Write one command byte to the SPI display bus.
 * @param cmd:  Command byte.
 * @retval:     NULL
 */
static void spi_write_cmd(uint8_t cmd) {
    spi_dev.set_dc(0);
    spi_dev.set_cs(0);
    spi_dev.spi_transbyte(cmd);
    spi_dev.set_cs(1);
    spi_dev.set_dc(1);
}

/**
 * @function:   spi_write_data
 * @breif:      Write one data byte to the SPI display bus.
 * @param data: Data byte.
 * @retval:     NULL
 */
static void spi_write_data(uint8_t data) {
    spi_dev.set_dc(1);
    spi_dev.set_cs(0);
    spi_dev.spi_transbyte(data);
    spi_dev.set_cs(1);
}

/**
 * @function:   spi_write_data_buf
 * @breif:      Write a continuous data buffer in one SPI transaction.
 * @param data: Data buffer pointer.
 * @param len:  Data length in bytes.
 * @retval:     NULL
 */
static void spi_write_data_buf(const uint8_t *data, uint32_t len) {
    if ((data == NULL) || (len == 0U)) {
        return;
    }

    spi_dev.set_dc(1);
    spi_dev.set_cs(0);
    for (uint32_t i = 0; i < len; i++) {
        spi_dev.spi_transbyte(data[i]);
    }
    spi_dev.set_cs(1);
}

/**
 * @function:       lcd_fn_register
 * @breif:          Register SPI operation callbacks for a display device.
 * @param lcd_dev:  Display SPI operation object.
 * @retval:         NULL
 */
void lcd_fn_register(BASE_SPI *lcd_dev) {
    if (lcd_dev == NULL) {
        return;
    }

    lcd_dev->dev = &spi_dev;
    lcd_dev->write_cmd = spi_write_cmd;
    lcd_dev->write_data = spi_write_data;
    lcd_dev->write_data_buf = spi_write_data_buf;
    lcd_dev->read_busy = SPI_READ_BUSY;
    lcd_dev->set_reset = SPI_SET_RESET;
}
