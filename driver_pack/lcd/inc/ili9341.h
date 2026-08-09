#ifndef ILI9341_H__
#define ILI9341_H__

#include "lcd_driver.h"

#define ILI9341_CMD_SLEEP_OUT (0x11U)
#define ILI9341_CMD_DISPLAY_ON (0x29U)
#define ILI9341_CMD_COLUMN_ADDR (0x2AU)
#define ILI9341_CMD_PAGE_ADDR (0x2BU)
#define ILI9341_CMD_MEMORY_WRITE (0x2CU)
#define ILI9341_CMD_MEMORY_ACCESS (0x36U)
#define ILI9341_CMD_PIXEL_FORMAT (0x3AU)

void ili9341_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void ili9341_driver_init_callback(LCD_DRIVER *lcd_driver);

#endif
