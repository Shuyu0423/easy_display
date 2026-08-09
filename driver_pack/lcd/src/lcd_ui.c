#include "lcd_ui.h"
#include "font.h"
#include <stddef.h>

static LCD_DRIVER ui_driver;
static PAINT paint;

#if USE_EPD
static uint8_t img_bw[LCD_WIDTH * LCD_HEIGHT];
#endif

#if USE_EPD
/**
 * @function:      ui_new_img
 * @breif:         Create a monochrome canvas for EPD drawing.
 * @param image:   Canvas buffer.
 * @param width:   Canvas width.
 * @param height:  Canvas height.
 * @param rotate:  Canvas rotation angle.
 * @param color:   Default canvas color.
 * @retval:        NULL
 */
static void ui_new_img(uint8_t *image, uint16_t width, uint16_t height, uint16_t rotate,
                       uint16_t color) {
    paint.dis_buffer = image;
    paint.color = color;
    paint.widthMemory = width;
    paint.heightMemory = height;
    paint.widthByte = (width % 8U == 0U) ? (width / 8U) : (width / 8U + 1U);
    paint.heightByte = height;
    paint.rotate = rotate;

    if ((rotate == 0U) || (rotate == 180U)) {
        paint.width = height;
        paint.height = width;
    } else {
        paint.width = width;
        paint.height = height;
    }

#if USE_EPD
    ui_buf_clear(WHITE);
#endif
}
#endif

/**
 * @function:       ui_set_pixel
 * @breif:          Draw one pixel through the active display driver.
 * @param xpoint:   X coordinate.
 * @param ypoint:   Y coordinate.
 * @param color:    Pixel color.
 * @retval:         NULL
 */
static void ui_set_pixel(uint16_t xpoint, uint16_t ypoint, uint16_t color) {
    if (ui_driver.dev_type == DEV_EPD) {
        uint16_t x, y;
        uint32_t addr;
        uint8_t r_data;

        if ((paint.dis_buffer == NULL) || (xpoint >= paint.width) || (ypoint >= paint.height)) {
            return;
        }

        switch (paint.rotate) {
            case 0:
                x = xpoint;
                y = paint.heightMemory - ypoint - 1U;
                break;
            case 90:
                x = paint.widthMemory - ypoint - 1U;
                y = paint.heightMemory - xpoint - 1U;
                break;
            case 180:
                x = paint.widthMemory - xpoint - 1U;
                y = ypoint;
                break;
            case 270:
                x = ypoint;
                y = xpoint;
                break;
            default:
                return;
        }

        addr = (x / 8U) + (y * paint.widthByte);
        r_data = paint.dis_buffer[addr];
        if (color == BLACK) {
            paint.dis_buffer[addr] = r_data & ~(0x80U >> (x % 8U));
        } else {
            paint.dis_buffer[addr] = r_data | (0x80U >> (x % 8U));
        }
    } else if (ui_driver.set_pixel) {
        ui_driver.set_pixel(xpoint, ypoint, color);
    }
}

/**
 * @function:   ui_init
 * @breif:      Initialize the selected display driver and UI state.
 * @param:      NULL
 * @retval:     NULL
 */
void ui_init(void) {
#if USE_EPD
    ssd1680_driver_init_callback(&ui_driver);
#endif
#if USE_LCD
#if USE_ILI9341
    ili9341_driver_init_callback(&ui_driver);
#elif USE_ST7789
    st7789_driver_init_callback(&ui_driver);
#endif
#endif

    if (ui_driver.ui_init) {
        ui_driver.ui_init();
    }

    if (ui_driver.dev_type == DEV_EPD) {
#if USE_EPD
        ui_new_img(img_bw, LCD_WIDTH, LCD_HEIGHT, 0, WHITE);
#endif
    } else if (ui_driver.clear) {
        ui_driver.clear(WHITE);
    }
}

/**
 * @function:   ui_disp
 * @breif:      Send the framebuffer to the active display.
 * @param buf:  Framebuffer pointer.
 * @retval:     NULL
 */
void ui_disp(const uint8_t *buf) {
    if (ui_driver.disp) {
        ui_driver.disp(buf);
    }
}

/**
 * @function:   ui_updata
 * @breif:      Trigger display refresh for displays that need explicit update.
 * @param:      NULL
 * @retval:     NULL
 */
void ui_updata(void) {
    if (ui_driver.update) {
        ui_driver.update();
    }
}

/**
 * @function:    ui_clear
 * @breif:       Clear the active display.
 * @param color: Clear color.
 * @retval:      NULL
 */
void ui_clear(uint16_t color) {
    if (ui_driver.clear) {
        ui_driver.clear(color);
    }
}

/**
 * @function:    ui_buf_clear
 * @breif:       Clear the local EPD canvas buffer.
 * @param color: Fill color.
 * @retval:      NULL
 */
void ui_buf_clear(uint8_t color) {
    if (paint.dis_buffer == NULL) {
        return;
    }

    for (uint16_t y = 0; y < paint.heightByte; y++) {
        for (uint16_t x = 0; x < paint.widthByte; x++) {
            paint.dis_buffer[x + (y * paint.widthByte)] = color;
        }
    }
}

/**
 * @function:    ui_show_char
 * @breif:       Draw one ASCII character.
 * @param x:     Start X coordinate.
 * @param y:     Start Y coordinate.
 * @param chr:   Character code.
 * @param size1: Font size.
 * @param color: Character color.
 * @retval:      NULL
 */
void ui_show_char(uint16_t x, uint16_t y, uint16_t chr, uint16_t size1, uint16_t color) {
    uint16_t size2;
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint16_t chr1 = chr - ' ';

    if (ui_driver.dev_type == DEV_EPD) {
        if (size1 == 8U) {
            size2 = 6U;
        } else {
            size2 = (size1 / 8U + ((size1 % 8U) ? 1U : 0U)) * (size1 / 2U);
        }
    } else {
        size2 = (size1 == 8U) ? 8U : size1;
    }

    for (uint16_t i = 0; i < size2; i++) {
        uint16_t temp;

        if (size1 == 8U) {
            temp = asc2_0806[chr1][i];
        } else if (size1 == 12U) {
            temp = asc2_1206[chr1][i];
        } else if (size1 == 16U) {
            temp = asc2_1608[chr1][i];
        } else if (size1 == 24U) {
            temp = asc2_2412[chr1][i];
        } else if (size1 == 48U) {
            temp = asc2_4824[chr1][i];
        } else {
            return;
        }

        for (uint16_t m = 0; m < 8U; m++) {
            ui_set_pixel(x, y, (temp & 0x01U) ? color : WHITE);
            temp >>= 1U;
            y++;
        }

        x++;
        if ((size1 != 8U) && ((x - x0) == size1 / 2U)) {
            x = x0;
            y0 += 8U;
        }
        y = y0;
    }
}

/**
 * @function:    ui_show_string
 * @breif:       Draw a string.
 * @param x:     Start X coordinate.
 * @param y:     Start Y coordinate.
 * @param chr:   String pointer.
 * @param size1: Font size.
 * @param color: String color.
 * @retval:      NULL
 */
void ui_show_string(uint16_t x, uint16_t y, uint8_t *chr, uint16_t size1, uint16_t color) {
    while (*chr != '\0') {
        ui_show_char(x, y, *chr, size1, color);
        chr++;
        x += size1 / 2U;
    }

    if (ui_driver.dev_type == DEV_EPD) {
#if USE_EPD
        ui_disp(img_bw);
        ui_updata();
#endif
    }
}

/**
 * @function:     ui_show_picture
 * @breif:        Draw a picture on the active display.
 * @param x:      Start X coordinate.
 * @param y:      Start Y coordinate.
 * @param width:  Picture width.
 * @param height: Picture height.
 * @param image:  Picture data pointer.
 * @param color:  Foreground color for monochrome EPD pictures.
 * @retval:       NULL
 */
void ui_show_picture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image,
                     uint16_t color) {
    if (image == NULL) {
        return;
    }

    if (ui_driver.dev_type == DEV_EPD) {
        uint16_t y0 = y;
        uint16_t bg_color = !color;
        uint16_t byte_count = width * (height / 8U + ((height % 8U) ? 1U : 0U));

        for (uint16_t i = 0; i < byte_count; i++) {
            uint8_t temp = image[i];
            for (uint8_t bit = 0; bit < 8U; bit++) {
                ui_set_pixel(x, y, (temp & 0x80U) ? color : bg_color);
                y++;
                temp <<= 1U;
            }

            if ((y - y0) == height) {
                y = y0;
                x++;
            }
        }

#if USE_EPD
        ui_disp(img_bw);
        ui_updata();
#endif
    } else if (ui_driver.set_window && ui_driver.write_pixels) {
        ui_driver.set_window(x, y, x + width - 1U, y + height - 1U);
        ui_driver.write_pixels(image, (uint32_t)width * height);
    }
}

/**
 * @function:     ui_show_picture_rgb565
 * @breif:        Draw an RGB565 picture on LCD displays.
 * @param x:      Start X coordinate.
 * @param y:      Start Y coordinate.
 * @param width:  Picture width.
 * @param height: Picture height.
 * @param image:  RGB565 picture data pointer.
 * @retval:       NULL
 */
void ui_show_picture_rgb565(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                            const uint8_t *image) {
    if ((image == NULL) || (ui_driver.dev_type != DEV_LCD)) {
        return;
    }

    if (ui_driver.set_window && ui_driver.write_pixels) {
        ui_driver.set_window(x, y, x + width - 1U, y + height - 1U);
        ui_driver.write_pixels(image, (uint32_t)width * height);
    }
}

/**
 * @function:   ui_sleep
 * @breif:      Put the active display into sleep mode.
 * @param:      NULL
 * @retval:     NULL
 */
void ui_sleep(void) {
    if (ui_driver.sleep) {
        ui_driver.sleep();
    }
}

/**
 * @function:    LCD_ShowChar
 * @breif:       Compatibility wrapper for drawing one character.
 * @param x:     Start X coordinate.
 * @param y:     Start Y coordinate.
 * @param num:   Character code.
 * @param fc:    Foreground color.
 * @param bc:    Background color, reserved.
 * @param sizey: Font height.
 * @param mode:  Draw mode, reserved.
 * @retval:      NULL
 */
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey,
                  uint8_t mode) {
    (void)bc;
    (void)mode;
    ui_show_char(x, y, num, sizey, fc);
}

/**
 * @function:    LCD_ShowString
 * @breif:       Compatibility wrapper for drawing a string.
 * @param x:     Start X coordinate.
 * @param y:     Start Y coordinate.
 * @param p:     String pointer.
 * @param fc:    Foreground color.
 * @param bc:    Background color, reserved.
 * @param sizey: Font height.
 * @param mode:  Draw mode, reserved.
 * @retval:      NULL
 */
void LCD_ShowString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc,
                    uint8_t sizey, uint8_t mode) {
    (void)bc;
    (void)mode;

    while (*p != '\0') {
        ui_show_char(x, y, *p, sizey, fc);
        x += sizey / 2U;
        p++;
    }
}

/**
 * @function:    LCD_FillRect_FastStatic
 * @breif:       Fill a rectangular area with one color.
 * @param x0:    Start X coordinate.
 * @param y0:    Start Y coordinate.
 * @param x1:    End X coordinate.
 * @param y1:    End Y coordinate.
 * @param color: Fill color.
 * @retval:      NULL
 */
void LCD_FillRect_FastStatic(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    for (uint16_t y = y0; y <= y1; y++) {
        for (uint16_t x = x0; x <= x1; x++) { ui_set_pixel(x, y, color); }
    }
}

/**
 * @function:    TFT_full_color
 * @breif:       Fill the full LCD with one color.
 * @param color: Fill color.
 * @retval:      NULL
 */
void TFT_full_color(unsigned int color) {
    ui_clear((uint16_t)color);
}

/**
 * @function:       LCD_DrawBitmap_Mono
 * @breif:          Draw a 1-bit monochrome bitmap.
 * @param x:        Start X coordinate.
 * @param y:        Start Y coordinate.
 * @param width:    Bitmap width.
 * @param height:   Bitmap height.
 * @param bitmap:   Bitmap data pointer.
 * @param fg_color: Foreground color.
 * @param bg_color: Background color.
 * @retval:         NULL
 */
void LCD_DrawBitmap_Mono(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                         const uint8_t *bitmap, uint16_t fg_color, uint16_t bg_color) {
    if (bitmap == NULL) {
        return;
    }

    for (uint16_t row = 0; row < height; row++) {
        for (uint16_t col = 0; col < width; col++) {
            uint16_t byte_index = row * ((width + 7U) / 8U) + (col / 8U);
            uint8_t bit_index = 7U - (col % 8U);
            uint16_t color = (bitmap[byte_index] & (1U << bit_index)) ? fg_color : bg_color;
            ui_set_pixel(x + col, y + row, color);
        }
    }
}

/**
 * @function:     LCD_ShowImage_RGB565
 * @breif:        Compatibility wrapper for drawing an RGB565 image.
 * @param x:      Start X coordinate.
 * @param y:      Start Y coordinate.
 * @param width:  Image width.
 * @param height: Image height.
 * @param image:  RGB565 image data pointer.
 * @retval:       NULL
 */
void LCD_ShowImage_RGB565(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                          const uint8_t *image) {
    ui_show_picture_rgb565(x, y, width, height, image);
}
