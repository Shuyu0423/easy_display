#ifndef ILI9341_EASY_DISPLAY_H
#define ILI9341_EASY_DISPLAY_H

#include "easy_display/display.h"
#include "lcd_driver.h"

typedef struct {
    LCD_DRIVER legacy_driver;
} ili9341_easy_display_context_t;

easy_device_status_t ili9341_easy_display_create(
    easy_display_t *display,
    ili9341_easy_display_context_t *context);

#endif

