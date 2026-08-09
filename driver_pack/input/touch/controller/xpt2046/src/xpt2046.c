#include "xpt2046.h"
#include <stddef.h>

static XPT2046_CONTEXT xpt2046_ctx = {
    .raw_min_x = XPT2046_DEFAULT_RAW_MIN_X,
    .raw_max_x = XPT2046_DEFAULT_RAW_MAX_X,
    .raw_min_y = XPT2046_DEFAULT_RAW_MIN_Y,
    .raw_max_y = XPT2046_DEFAULT_RAW_MAX_Y,
    .press_threshold = XPT2046_DEFAULT_PRESS_THRESHOLD,
};
static TOUCH_DEVICE xpt2046_dev;

/**
 * @function:   xpt2046_read_adc
 * @breif:      Read one 12-bit ADC channel from XPT2046.
 * @param dev:  Touch device object.
 * @param cmd:  XPT2046 command byte.
 * @param data: Output ADC value.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t xpt2046_read_adc(TOUCH_DEVICE *dev, uint8_t cmd, uint16_t *data) {
    uint8_t tx[3] = {cmd, 0x00U, 0x00U};
    uint8_t rx[3] = {0};

    if ((dev == NULL) || (dev->bus == NULL) || (dev->bus->transfer == NULL) || (data == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    if (dev->bus->transfer(tx, rx, sizeof(tx)) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

    *data = (uint16_t)((((uint16_t)rx[1] << 8U) | rx[2]) >> 3U);
    return TOUCH_OK;
}

/**
 * @function:    xpt2046_is_pressed
 * @breif:       Check whether XPT2046 is currently pressed.
 * @param dev:   Touch device object.
 * @param state: Output state, 1 pressed and 0 released.
 * @retval:      TOUCH_STATUS.
 */
static uint8_t xpt2046_is_pressed(TOUCH_DEVICE *dev, uint8_t *state) {
    uint16_t z1 = 0U;
    XPT2046_CONTEXT *ctx;

    if ((dev == NULL) || (dev->priv == NULL) || (state == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ctx = (XPT2046_CONTEXT *)dev->priv;
    if (dev->bus && dev->bus->read_int) {
        *state = (dev->bus->read_int() == 0U) ? 1U : 0U;
        return TOUCH_OK;
    }

    if (xpt2046_read_adc(dev, XPT2046_CMD_READ_Z1, &z1) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }
    *state = (z1 >= ctx->press_threshold) ? 1U : 0U;
    return TOUCH_OK;
}

/**
 * @function:   xpt2046_sort_samples
 * @breif:      Sort sample values from small to large.
 * @param data: Sample buffer.
 * @param len:  Sample count.
 * @retval:     NULL
 */
static void xpt2046_sort_samples(uint16_t *data, uint8_t len) {
    for (uint8_t i = 0U; i < len; i++) {
        for (uint8_t j = (uint8_t)(i + 1U); j < len; j++) {
            if (data[j] < data[i]) {
                uint16_t temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}

/**
 * @function:     xpt2046_filtered_read
 * @breif:        Read one filtered coordinate sample from XPT2046.
 * @param dev:    Touch device object.
 * @param raw_x:  Output raw X value.
 * @param raw_y:  Output raw Y value.
 * @retval:       TOUCH_STATUS.
 */
static uint8_t xpt2046_filtered_read(TOUCH_DEVICE *dev, uint16_t *raw_x, uint16_t *raw_y) {
    uint16_t xs[XPT2046_SAMPLE_COUNT];
    uint16_t ys[XPT2046_SAMPLE_COUNT];
    uint32_t sum_x = 0U;
    uint32_t sum_y = 0U;

    if ((raw_x == NULL) || (raw_y == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    for (uint8_t i = 0U; i < XPT2046_SAMPLE_COUNT; i++) {
        if (xpt2046_read_adc(dev, XPT2046_CMD_READ_X, &xs[i]) != TOUCH_OK) {
            return TOUCH_ERROR_BUS;
        }
        if (xpt2046_read_adc(dev, XPT2046_CMD_READ_Y, &ys[i]) != TOUCH_OK) {
            return TOUCH_ERROR_BUS;
        }
    }

    xpt2046_sort_samples(xs, XPT2046_SAMPLE_COUNT);
    xpt2046_sort_samples(ys, XPT2046_SAMPLE_COUNT);
    for (uint8_t i = 1U; i < (XPT2046_SAMPLE_COUNT - 1U); i++) {
        sum_x += xs[i];
        sum_y += ys[i];
    }

    *raw_x = (uint16_t)(sum_x / (XPT2046_SAMPLE_COUNT - 2U));
    *raw_y = (uint16_t)(sum_y / (XPT2046_SAMPLE_COUNT - 2U));
    return TOUCH_OK;
}

/**
 * @function:      xpt2046_map_axis
 * @breif:         Map raw ADC value to screen axis.
 * @param raw:     Raw ADC value.
 * @param raw_min: Raw minimum value.
 * @param raw_max: Raw maximum value.
 * @param size:    Screen axis size.
 * @param mirror:  Non-zero to mirror coordinate.
 * @retval:        Screen coordinate.
 */
static uint16_t xpt2046_map_axis(uint16_t raw, uint16_t raw_min, uint16_t raw_max, uint16_t size,
                                 uint8_t mirror) {
    uint32_t value;

    if ((size == 0U) || (raw_max <= raw_min)) {
        return 0U;
    }
    if (raw <= raw_min) {
        value = 0U;
    } else if (raw >= raw_max) {
        value = size - 1U;
    } else {
        value = ((uint32_t)(raw - raw_min) * (uint32_t)(size - 1U)) /
                (uint32_t)(raw_max - raw_min);
    }

    if (mirror) {
        value = (uint32_t)(size - 1U) - value;
    }
    return (uint16_t)value;
}

/**
 * @function:     xpt2046_raw_to_point
 * @breif:        Convert raw XPT2046 coordinates to screen point.
 * @param dev:    Touch device object.
 * @param raw_x:  Raw X value.
 * @param raw_y:  Raw Y value.
 * @param point:  Output touch point.
 * @retval:       NULL
 */
static void xpt2046_raw_to_point(TOUCH_DEVICE *dev, uint16_t raw_x, uint16_t raw_y,
                                 TOUCH_POINT *point) {
    XPT2046_CONTEXT *ctx = (XPT2046_CONTEXT *)dev->priv;
    uint16_t map_x;
    uint16_t map_y;

    if (ctx->swap_xy) {
        uint16_t temp = raw_x;
        raw_x = raw_y;
        raw_y = temp;
    }

    map_x = xpt2046_map_axis(raw_x, ctx->raw_min_x, ctx->raw_max_x, dev->width, ctx->mirror_x);
    map_y = xpt2046_map_axis(raw_y, ctx->raw_min_y, ctx->raw_max_y, dev->height, ctx->mirror_y);

    point->x = map_x;
    point->y = map_y;
}

/**
 * @function:   xpt2046_init
 * @breif:      Initialize XPT2046 bus.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t xpt2046_init(TOUCH_DEVICE *dev) {
    if ((dev == NULL) || (dev->bus == NULL) || (dev->priv == NULL) || (dev->bus->transfer == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    if (dev->bus->init && (dev->bus->init() != TOUCH_OK)) {
        return TOUCH_ERROR_BUS;
    }

    ((XPT2046_CONTEXT *)dev->priv)->last_pressed = 0U;
    return TOUCH_OK;
}

/**
 * @function:    xpt2046_read_point
 * @breif:       Read XPT2046 touch point.
 * @param dev:   Touch device object.
 * @param point: Output touch point.
 * @retval:      TOUCH_STATUS.
 */
static uint8_t xpt2046_read_point(TOUCH_DEVICE *dev, TOUCH_POINT *point) {
    XPT2046_CONTEXT *ctx;
    uint16_t raw_x;
    uint16_t raw_y;
    uint8_t pressed;

    if ((dev == NULL) || (point == NULL) || (dev->bus == NULL) || (dev->priv == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ctx = (XPT2046_CONTEXT *)dev->priv;
    point->pressed = 0U;
    point->point_num = 0U;
    point->event = TOUCH_EVENT_NONE;

    if (xpt2046_is_pressed(dev, &pressed) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }
    if (!pressed) {
        if (ctx->last_pressed) {
            ctx->last_pressed = 0U;
            point->event = TOUCH_EVENT_RELEASE;
        }
        return TOUCH_OK;
    }

    if (xpt2046_filtered_read(dev, &raw_x, &raw_y) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

    point->pressed = 1U;
    point->point_num = 1U;
    point->event = ctx->last_pressed ? TOUCH_EVENT_CONTACT : TOUCH_EVENT_PRESS;
    xpt2046_raw_to_point(dev, raw_x, raw_y, point);
    ctx->last_pressed = 1U;

    return TOUCH_OK;
}

/**
 * @function:   xpt2046_sleep
 * @breif:      Put XPT2046 into low power idle state.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t xpt2046_sleep(TOUCH_DEVICE *dev) {
    if ((dev == NULL) || (dev->priv == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ((XPT2046_CONTEXT *)dev->priv)->last_pressed = 0U;
    return TOUCH_OK;
}

/**
 * @function:   xpt2046_wakeup
 * @breif:      Wake XPT2046 from idle state.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t xpt2046_wakeup(TOUCH_DEVICE *dev) {
    if ((dev == NULL) || (dev->priv == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ((XPT2046_CONTEXT *)dev->priv)->last_pressed = 0U;
    return TOUCH_OK;
}

static const TOUCH_CONTROLLER xpt2046_controller = {
    .init = xpt2046_init,
    .read_point = xpt2046_read_point,
    .sleep = xpt2046_sleep,
    .wakeup = xpt2046_wakeup,
};

/**
 * @function:     xpt2046_register
 * @breif:        Register XPT2046 as current touch controller.
 * @param dev:    Optional user device object, NULL to use internal object.
 * @param bus:    Board touch bus operation table.
 * @param width:  Touch panel width.
 * @param height: Touch panel height.
 * @retval:       TOUCH_STATUS.
 */
uint8_t xpt2046_register(TOUCH_DEVICE *dev, TOUCH_BUS_OPS *bus, uint16_t width, uint16_t height) {
    TOUCH_DEVICE *target = dev ? dev : &xpt2046_dev;

    if ((bus == NULL) || (width == 0U) || (height == 0U)) {
        return TOUCH_ERROR_PARAM;
    }

    target->address = 0U;
    target->width = width;
    target->height = height;
    target->bus = bus;
    target->priv = &xpt2046_ctx;

    return touch_register(target, &xpt2046_controller);
}

/**
 * @function:          xpt2046_set_calibration
 * @breif:             Set XPT2046 raw coordinate calibration.
 * @param raw_min_x:   Raw X minimum.
 * @param raw_max_x:   Raw X maximum.
 * @param raw_min_y:   Raw Y minimum.
 * @param raw_max_y:   Raw Y maximum.
 * @param swap_xy:     Non-zero to swap X/Y.
 * @param mirror_x:    Non-zero to mirror X.
 * @param mirror_y:    Non-zero to mirror Y.
 * @retval:            NULL
 */
void xpt2046_set_calibration(uint16_t raw_min_x, uint16_t raw_max_x, uint16_t raw_min_y,
                             uint16_t raw_max_y, uint8_t swap_xy, uint8_t mirror_x,
                             uint8_t mirror_y) {
    xpt2046_ctx.raw_min_x = raw_min_x;
    xpt2046_ctx.raw_max_x = raw_max_x;
    xpt2046_ctx.raw_min_y = raw_min_y;
    xpt2046_ctx.raw_max_y = raw_max_y;
    xpt2046_ctx.swap_xy = swap_xy;
    xpt2046_ctx.mirror_x = mirror_x;
    xpt2046_ctx.mirror_y = mirror_y;
}

/**
 * @function:   xpt2046_get_controller
 * @breif:      Get XPT2046 controller operation table.
 * @param:      NULL
 * @retval:     TOUCH_CONTROLLER pointer.
 */
const TOUCH_CONTROLLER *xpt2046_get_controller(void) {
    return &xpt2046_controller;
}

/**
 * @function:   xpt2046_get_context
 * @breif:      Get XPT2046 runtime context.
 * @param:      NULL
 * @retval:     XPT2046_CONTEXT pointer.
 */
XPT2046_CONTEXT *xpt2046_get_context(void) {
    return &xpt2046_ctx;
}
