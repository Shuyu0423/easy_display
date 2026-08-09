#ifndef XPT2046_H__
#define XPT2046_H__

#include "touch_driver.h"

#define XPT2046_CMD_READ_X (0xD0U)
#define XPT2046_CMD_READ_Y (0x90U)
#define XPT2046_CMD_READ_Z1 (0xB0U)
#define XPT2046_CMD_READ_Z2 (0xC0U)

#define XPT2046_DEFAULT_RAW_MIN_X (200U)
#define XPT2046_DEFAULT_RAW_MAX_X (3900U)
#define XPT2046_DEFAULT_RAW_MIN_Y (200U)
#define XPT2046_DEFAULT_RAW_MAX_Y (3900U)
#define XPT2046_DEFAULT_PRESS_THRESHOLD (100U)
#define XPT2046_SAMPLE_COUNT (5U)

typedef struct {
    uint16_t raw_min_x;
    uint16_t raw_max_x;
    uint16_t raw_min_y;
    uint16_t raw_max_y;
    uint16_t press_threshold;
    uint8_t swap_xy;
    uint8_t mirror_x;
    uint8_t mirror_y;
    uint8_t last_pressed;
} XPT2046_CONTEXT;

uint8_t xpt2046_register(TOUCH_DEVICE *dev, TOUCH_BUS_OPS *bus, uint16_t width, uint16_t height);
void xpt2046_set_calibration(uint16_t raw_min_x, uint16_t raw_max_x, uint16_t raw_min_y,
                             uint16_t raw_max_y, uint8_t swap_xy, uint8_t mirror_x,
                             uint8_t mirror_y);
const TOUCH_CONTROLLER *xpt2046_get_controller(void);
XPT2046_CONTEXT *xpt2046_get_context(void);

#endif
