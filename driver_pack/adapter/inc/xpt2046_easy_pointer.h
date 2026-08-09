#ifndef XPT2046_EASY_POINTER_H
#define XPT2046_EASY_POINTER_H

#include "easy_display/input.h"
#include "touch_driver.h"

typedef struct {
    TOUCH_DEVICE legacy_device;
    TOUCH_BUS_OPS *bus;
    uint16_t width;
    uint16_t height;
    uint16_t raw_min_x;
    uint16_t raw_max_x;
    uint16_t raw_min_y;
    uint16_t raw_max_y;
    uint8_t swap_xy;
    uint8_t mirror_x;
    uint8_t mirror_y;
} xpt2046_easy_pointer_context_t;

easy_device_status_t xpt2046_easy_pointer_create(
    easy_pointer_t *pointer,
    xpt2046_easy_pointer_context_t *context,
    TOUCH_BUS_OPS *bus,
    uint16_t width,
    uint16_t height);

#endif

