#include "user_conf.h"
#include "ili9341_xpt2046_test.h"

#if ILI9341_XPT2046_TEST_ENABLE

#include "board.h"
#include "ili9341.h"
#include "log_backend_rtt.h"
#include "mcu_touch_spi.h"
#include "touch_driver.h"
#include "xpt2046.h"

#define ILI9341_XPT2046_RTT_TERMINAL_ID (0U)
#define ILI9341_XPT2046_RTT_CHANNEL_INDEX (0U)
#define ILI9341_XPT2046_PRINT(...) \
    log_backend_rtt_printf(ILI9341_XPT2046_RTT_TERMINAL_ID, ILI9341_XPT2046_RTT_CHANNEL_INDEX, __VA_ARGS__)

#define RGB565_RED (0xF800U)
#define RGB565_GREEN (0x07E0U)
#define RGB565_BLUE (0x001FU)
#define RGB565_YELLOW (0xFFE0U)
#define RGB565_CYAN (0x07FFU)
#define RGB565_MAGENTA (0xF81FU)
#define RGB565_BLACK (0x0000U)
#define RGB565_WHITE (0xFFFFU)

static LCD_DRIVER test_lcd;

static const char *touch_event_name(TOUCH_EVENT event) {
    switch (event) {
        case TOUCH_EVENT_NONE:
            return "NONE";
        case TOUCH_EVENT_PRESS:
            return "PRESS";
        case TOUCH_EVENT_RELEASE:
            return "RELEASE";
        case TOUCH_EVENT_CONTACT:
            return "CONTACT";
        default:
            return "OTHER";
    }
}

static void test_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    if (test_lcd.set_pixel == 0) {
        return;
    }

    for (uint16_t y = y0; y <= y1; y++) {
        for (uint16_t x = x0; x <= x1; x++) {
            test_lcd.set_pixel(x, y, color);
        }
    }
}

static void test_draw_cross(uint16_t x, uint16_t y, uint16_t color) {
    if (test_lcd.set_pixel == 0) {
        return;
    }

    for (uint16_t i = 0U; i < 9U; i++) {
        if ((x + i) >= 4U && (x + i - 4U) < LCD_WIDTH) {
            test_lcd.set_pixel((uint16_t)(x + i - 4U), y, color);
        }
        if ((y + i) >= 4U && (y + i - 4U) < LCD_HEIGHT) {
            test_lcd.set_pixel(x, (uint16_t)(y + i - 4U), color);
        }
    }
}

static void test_draw_pattern(void) {
    ILI9341_XPT2046_PRINT("LCD color test: red\r\n");
    test_lcd.clear(RGB565_RED);
    delay_ms(500U);

    ILI9341_XPT2046_PRINT("LCD color test: green\r\n");
    test_lcd.clear(RGB565_GREEN);
    delay_ms(500U);

    ILI9341_XPT2046_PRINT("LCD color test: blue\r\n");
    test_lcd.clear(RGB565_BLUE);
    delay_ms(500U);

    ILI9341_XPT2046_PRINT("LCD pattern test\r\n");
    test_fill_rect(0U, 0U, (LCD_WIDTH / 2U) - 1U, (LCD_HEIGHT / 2U) - 1U, RGB565_RED);
    test_fill_rect(LCD_WIDTH / 2U, 0U, LCD_WIDTH - 1U, (LCD_HEIGHT / 2U) - 1U, RGB565_GREEN);
    test_fill_rect(0U, LCD_HEIGHT / 2U, (LCD_WIDTH / 2U) - 1U, LCD_HEIGHT - 1U, RGB565_BLUE);
    test_fill_rect(LCD_WIDTH / 2U, LCD_HEIGHT / 2U, LCD_WIDTH - 1U, LCD_HEIGHT - 1U, RGB565_YELLOW);

    for (uint16_t x = 0U; x < LCD_WIDTH; x++) {
        test_lcd.set_pixel(x, 0U, RGB565_WHITE);
        test_lcd.set_pixel(x, LCD_HEIGHT - 1U, RGB565_WHITE);
    }
    for (uint16_t y = 0U; y < LCD_HEIGHT; y++) {
        test_lcd.set_pixel(0U, y, RGB565_WHITE);
        test_lcd.set_pixel(LCD_WIDTH - 1U, y, RGB565_WHITE);
    }
}

static void test_touch_loop(void) {
    TOUCH_POINT point;
    uint8_t ret;

    ret = xpt2046_register(0, mcu_touch_spi_get_ops(), LCD_WIDTH, LCD_HEIGHT);
    if (ret != TOUCH_OK) {
        ILI9341_XPT2046_PRINT("xpt2046_register failed: %u\r\n", ret);
        while (1) {
        }
    }

    xpt2046_set_calibration(200U, 3900U, 200U, 3900U, 0U, 0U, 0U);

    ret = touch_init();
    if (ret != TOUCH_OK) {
        ILI9341_XPT2046_PRINT("touch_init failed: %u\r\n", ret);
        while (1) {
        }
    }

    ILI9341_XPT2046_PRINT("XPT2046 ready. Touch screen to print point and draw cross.\r\n");
    while (1) {
        ret = touch_read_point(&point);
        if (ret == TOUCH_OK) {
            if (point.pressed) {
                ILI9341_XPT2046_PRINT("touch: x=%u y=%u event=%s\r\n",
                                      point.x, point.y, touch_event_name(point.event));
                test_draw_cross(point.x, point.y, RGB565_MAGENTA);
            } else if (point.event == TOUCH_EVENT_RELEASE) {
                ILI9341_XPT2046_PRINT("touch release\r\n");
            }
        } else {
            ILI9341_XPT2046_PRINT("touch_read_point failed: %u\r\n", ret);
        }

        delay_ms(ILI9341_XPT2046_TEST_POLL_MS);
    }
}

void ili9341_xpt2046_test(void) {
    ILI9341_XPT2046_PRINT("\r\n=== ILI9341 + XPT2046 test ===\r\n");
    ILI9341_XPT2046_PRINT("LCD=%ux%u RTT terminal=%u channel=%u\r\n",
                          (uint16_t)LCD_WIDTH,
                          (uint16_t)LCD_HEIGHT,
                          (uint16_t)ILI9341_XPT2046_RTT_TERMINAL_ID,
                          (uint16_t)ILI9341_XPT2046_RTT_CHANNEL_INDEX);

    ili9341_driver_init_callback(&test_lcd);
    test_lcd.ui_init();
    test_draw_pattern();
    test_touch_loop();
}

#endif
