#include "ili9341.h"
#include <stddef.h>

static BASE_SPI ili9341;

/**
 * @function:   ili9341_base_init
 * @breif:      Register board SPI callbacks for ILI9341.
 * @param:      NULL
 * @retval:     NULL
 */
static void ili9341_base_init(void) {
    lcd_fn_register(&ili9341);
}

/**
 * @function:   ili9341_write_u16
 * @breif:      Write a 16-bit value to ILI9341 data port.
 * @param data: 16-bit data value.
 * @retval:     NULL
 */
static void ili9341_write_u16(uint16_t data) {
    ili9341.write_data((uint8_t)(data >> 8U));
    ili9341.write_data((uint8_t)(data & 0xFFU));
}

/**
 * @function:   ili9341_set_window
 * @breif:      Set ILI9341 drawing window.
 * @param x1:   Start X coordinate.
 * @param y1:   Start Y coordinate.
 * @param x2:   End X coordinate.
 * @param y2:   End Y coordinate.
 * @retval:     NULL
 */
void ili9341_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    ili9341.write_cmd(ILI9341_CMD_COLUMN_ADDR);
    ili9341_write_u16(x1);
    ili9341_write_u16(x2);

    ili9341.write_cmd(ILI9341_CMD_PAGE_ADDR);
    ili9341_write_u16(y1);
    ili9341_write_u16(y2);

    ili9341.write_cmd(ILI9341_CMD_MEMORY_WRITE);
}

/**
 * @function:   ili9341_driver_init
 * @breif:      Initialize ILI9341 controller registers.
 * @param:      NULL
 * @retval:     NULL
 */
static void ili9341_driver_init(void) {
    ili9341.set_reset(0);
    DELAY_MS(20);
    ili9341.set_reset(1);
    DELAY_MS(120);

#if NEED_SPIPORT_BL
    SPI_SET_BLE(1);
#endif

    ili9341.write_cmd(0xEF);
    ili9341.write_data(0x03);
    ili9341.write_data(0x80);
    ili9341.write_data(0x02);

    ili9341.write_cmd(0xCF);
    ili9341.write_data(0x00);
    ili9341.write_data(0xC1);
    ili9341.write_data(0x30);

    ili9341.write_cmd(0xED);
    ili9341.write_data(0x64);
    ili9341.write_data(0x03);
    ili9341.write_data(0x12);
    ili9341.write_data(0x81);

    ili9341.write_cmd(0xE8);
    ili9341.write_data(0x85);
    ili9341.write_data(0x00);
    ili9341.write_data(0x78);

    ili9341.write_cmd(0xCB);
    ili9341.write_data(0x39);
    ili9341.write_data(0x2C);
    ili9341.write_data(0x00);
    ili9341.write_data(0x34);
    ili9341.write_data(0x02);

    ili9341.write_cmd(0xF7);
    ili9341.write_data(0x20);

    ili9341.write_cmd(0xEA);
    ili9341.write_data(0x00);
    ili9341.write_data(0x00);

    ili9341.write_cmd(0xC0);
    ili9341.write_data(0x23);

    ili9341.write_cmd(0xC1);
    ili9341.write_data(0x10);

    ili9341.write_cmd(0xC5);
    ili9341.write_data(0x3E);
    ili9341.write_data(0x28);

    ili9341.write_cmd(0xC7);
    ili9341.write_data(0x86);

    ili9341.write_cmd(ILI9341_CMD_MEMORY_ACCESS);
    switch (SET_SCREEN_ORIENTATION) {
        case 0:
            ili9341.write_data(0x48);
            break;
        case 1:
            ili9341.write_data(0x88);
            break;
        case 2:
            ili9341.write_data(0x28);
            break;
        case 3:
            ili9341.write_data(0xE8);
            break;
        default:
            ili9341.write_data(0x48);
            break;
    }

    ili9341.write_cmd(ILI9341_CMD_PIXEL_FORMAT);
    ili9341.write_data(0x55);

    ili9341.write_cmd(0xB1);
    ili9341.write_data(0x00);
    ili9341.write_data(0x18);

    ili9341.write_cmd(0xB6);
    ili9341.write_data(0x08);
    ili9341.write_data(0x82);
    ili9341.write_data(0x27);

    ili9341.write_cmd(0xF2);
    ili9341.write_data(0x00);

    ili9341.write_cmd(0x26);
    ili9341.write_data(0x01);

    ili9341.write_cmd(0xE0);
    ili9341.write_data(0x0F);
    ili9341.write_data(0x31);
    ili9341.write_data(0x2B);
    ili9341.write_data(0x0C);
    ili9341.write_data(0x0E);
    ili9341.write_data(0x08);
    ili9341.write_data(0x4E);
    ili9341.write_data(0xF1);
    ili9341.write_data(0x37);
    ili9341.write_data(0x07);
    ili9341.write_data(0x10);
    ili9341.write_data(0x03);
    ili9341.write_data(0x0E);
    ili9341.write_data(0x09);
    ili9341.write_data(0x00);

    ili9341.write_cmd(0xE1);
    ili9341.write_data(0x00);
    ili9341.write_data(0x0E);
    ili9341.write_data(0x14);
    ili9341.write_data(0x03);
    ili9341.write_data(0x11);
    ili9341.write_data(0x07);
    ili9341.write_data(0x31);
    ili9341.write_data(0xC1);
    ili9341.write_data(0x48);
    ili9341.write_data(0x08);
    ili9341.write_data(0x0F);
    ili9341.write_data(0x0C);
    ili9341.write_data(0x31);
    ili9341.write_data(0x36);
    ili9341.write_data(0x0F);

    ili9341.write_cmd(ILI9341_CMD_SLEEP_OUT);
    DELAY_MS(120);
    ili9341.write_cmd(ILI9341_CMD_DISPLAY_ON);
}

/**
 * @function:   ili9341_init
 * @breif:      Initialize ILI9341 device and board SPI.
 * @param:      NULL
 * @retval:     NULL
 */
static void ili9341_init(void) {
    ili9341_base_init();
    ili9341.dev->spi_init();
    ili9341_driver_init();
}

/**
 * @function:    ili9341_set_pixel
 * @breif:       Draw one RGB565 pixel on ILI9341.
 * @param x:     X coordinate.
 * @param y:     Y coordinate.
 * @param color: RGB565 color.
 * @retval:      NULL
 */
static void ili9341_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    ili9341_set_window(x, y, x, y);
    ili9341_write_u16(color);
}

/**
 * @function:          ili9341_write_pixels
 * @breif:             Write continuous RGB565 pixels to current ILI9341 window.
 * @param pixels:      RGB565 pixel data pointer, high byte first.
 * @param pixel_count: Pixel count.
 * @retval:            NULL
 */
static void ili9341_write_pixels(const uint8_t *pixels, uint32_t pixel_count) {
    if (ili9341.write_data_buf) {
        ili9341.write_data_buf(pixels, pixel_count * 2U);
    }
}

/**
 * @function:    ili9341_clear
 * @breif:       Fill whole ILI9341 screen with one RGB565 color.
 * @param color: RGB565 color.
 * @retval:      NULL
 */
static void ili9341_clear(uint16_t color) {
    uint32_t total_pixels = (uint32_t)LCD_WIDTH * (uint32_t)LCD_HEIGHT;
    uint8_t color_h = (uint8_t)(color >> 8U);
    uint8_t color_l = (uint8_t)(color & 0xFFU);

    ili9341_set_window(0U, 0U, LCD_WIDTH - 1U, LCD_HEIGHT - 1U);
    for (uint32_t i = 0U; i < total_pixels; i++) {
        ili9341.write_data(color_h);
        ili9341.write_data(color_l);
    }
}

/**
 * @function:   ili9341_display
 * @breif:      Display one full RGB565 frame buffer.
 * @param buf:  RGB565 frame buffer pointer, high byte first.
 * @retval:     NULL
 */
static void ili9341_display(const uint8_t *buf) {
    if (buf == NULL) {
        return;
    }

    ili9341_set_window(0U, 0U, LCD_WIDTH - 1U, LCD_HEIGHT - 1U);
    ili9341_write_pixels(buf, (uint32_t)LCD_WIDTH * (uint32_t)LCD_HEIGHT);
}

/**
 * @function:   ili9341_sleep
 * @breif:      Put ILI9341 into sleep mode.
 * @param:      NULL
 * @retval:     NULL
 */
static void ili9341_sleep(void) {
    ili9341.write_cmd(0x10);
    DELAY_MS(5);
}

/**
 * @function:          ili9341_driver_init_callback
 * @breif:             Fill LCD driver callbacks for ILI9341.
 * @param lcd_driver:  LCD driver object.
 * @retval:            NULL
 */
void ili9341_driver_init_callback(LCD_DRIVER *lcd_driver) {
    if (lcd_driver == NULL) {
        return;
    }

    lcd_driver->ui_init = ili9341_init;
    lcd_driver->disp = ili9341_display;
    lcd_driver->update = NULL;
    lcd_driver->clear = ili9341_clear;
    lcd_driver->sleep = ili9341_sleep;
    lcd_driver->set_pixel = ili9341_set_pixel;
    lcd_driver->set_window = ili9341_set_window;
    lcd_driver->write_pixels = ili9341_write_pixels;
    lcd_driver->dev_type = DEV_LCD;
}
