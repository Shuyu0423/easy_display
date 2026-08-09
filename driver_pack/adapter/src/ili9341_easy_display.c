#include "ili9341_easy_display.h"

#include "ili9341.h"

#include <stddef.h>

static easy_device_status_t ili9341_backend_init(void *context) {
    ili9341_easy_display_context_t *adapter =
        (ili9341_easy_display_context_t *)context;

    if (adapter == NULL) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    ili9341_driver_init_callback(&adapter->legacy_driver);
    if ((adapter->legacy_driver.ui_init == NULL) ||
        (adapter->legacy_driver.set_window == NULL) ||
        (adapter->legacy_driver.write_pixels == NULL)) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }

    adapter->legacy_driver.ui_init();
    return EASY_DEVICE_OK;
}

static easy_device_status_t ili9341_backend_flush(
    void *context,
    const easy_display_area_t *area,
    const void *pixels,
    size_t byte_count,
    easy_display_flush_done_cb_t done,
    void *done_context) {
    ili9341_easy_display_context_t *adapter =
        (ili9341_easy_display_context_t *)context;

    if ((adapter == NULL) || (area == NULL) || (pixels == NULL) ||
        ((byte_count & 1U) != 0U)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    adapter->legacy_driver.set_window(area->x1, area->y1,
                                      area->x2, area->y2);
    adapter->legacy_driver.write_pixels((const uint8_t *)pixels,
                                        (uint32_t)(byte_count / 2U));

    if (done != NULL) {
        done(done_context, EASY_DEVICE_OK);
    }
    return EASY_DEVICE_OK;
}

static const easy_display_ops_t ili9341_backend_ops = {
    .init = ili9341_backend_init,
    .flush = ili9341_backend_flush,
};

easy_device_status_t ili9341_easy_display_create(
    easy_display_t *display,
    ili9341_easy_display_context_t *context) {
    const easy_display_info_t info = {
        .width = LCD_WIDTH,
        .height = LCD_HEIGHT,
        .pixel_format = EASY_DISPLAY_PIXEL_FORMAT_RGB565,
        .bytes_per_pixel = 2U,
        .capabilities = 0U,
    };

    if ((display == NULL) || (context == NULL)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    return easy_display_configure(display, &info, &ili9341_backend_ops,
                                  context);
}

