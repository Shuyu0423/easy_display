#include "cst816.h"
#include <stddef.h>

static CST816_CONTEXT cst816_ctx;
static TOUCH_DEVICE cst816_dev;

static TOUCH_EVENT cst816_parse_event(uint8_t gesture_id) {
    switch (gesture_id) {
        case 0x01:
            return TOUCH_EVENT_SWIPE_UP;
        case 0x02:
            return TOUCH_EVENT_SWIPE_DOWN;
        case 0x03:
            return TOUCH_EVENT_SWIPE_LEFT;
        case 0x04:
            return TOUCH_EVENT_SWIPE_RIGHT;
        case 0x0C:
            return TOUCH_EVENT_LONG_PRESS;
        default:
            return TOUCH_EVENT_CONTACT;
    }
}

/**
 * @function:   cst816_reset
 * @breif:      Reset CST816 controller through board reset pin.
 * @param dev:  Touch device object.
 * @retval:     NULL
 */
static void cst816_reset(TOUCH_DEVICE *dev) {
    if ((dev == NULL) || (dev->bus == NULL) || (dev->bus->set_reset == NULL)) {
        return;
    }

    dev->bus->set_reset(0);
    if (dev->bus->delay_ms) {
        dev->bus->delay_ms(5);
    }
    dev->bus->set_reset(1);
    if (dev->bus->delay_ms) {
        dev->bus->delay_ms(50);
    }
}

/**
 * @function:   cst816_init
 * @breif:      Initialize CST816 controller.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t cst816_init(TOUCH_DEVICE *dev) {
    uint8_t chip_id = 0;

    if ((dev == NULL) || (dev->bus == NULL) || (dev->bus->read_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    if (dev->bus->init && (dev->bus->init() != TOUCH_OK)) {
        return TOUCH_ERROR_BUS;
    }

    cst816_reset(dev);

    if (dev->bus->read_reg(dev->address, CST816_REG_CHIP_ID, &chip_id, 1U) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

    ((CST816_CONTEXT *)dev->priv)->chip_id = chip_id;
    return TOUCH_OK;
}

/**
 * @function:    cst816_read_point
 * @breif:       Read CST816 touch coordinate and gesture information.
 * @param dev:   Touch device object.
 * @param point: Output touch point.
 * @retval:      TOUCH_STATUS.
 */
static uint8_t cst816_read_point(TOUCH_DEVICE *dev, TOUCH_POINT *point) {
    uint8_t buf[6];

    if ((dev == NULL) || (point == NULL) || (dev->bus == NULL) || (dev->bus->read_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    point->pressed = 0U;
    point->point_num = 0U;
    point->event = TOUCH_EVENT_NONE;

    if (dev->bus->read_int && (dev->bus->read_int() != 0U)) {
        return TOUCH_OK;
    }

    if (dev->bus->read_reg(dev->address, CST816_REG_GESTURE_ID, buf, sizeof(buf)) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

    point->point_num = buf[1] & 0x0FU;
    if (point->point_num == 0U) {
        return TOUCH_OK;
    }

    point->pressed = 1U;
    point->event = cst816_parse_event(buf[0]);
    point->x = (uint16_t)(((buf[2] & 0x0FU) << 8U) | buf[3]);
    point->y = (uint16_t)(((buf[4] & 0x0FU) << 8U) | buf[5]);

    if (point->x >= dev->width) {
        point->x = dev->width - 1U;
    }
    if (point->y >= dev->height) {
        point->y = dev->height - 1U;
    }

    return TOUCH_OK;
}

/**
 * @function:   cst816_sleep
 * @breif:      Put CST816 into sleep mode.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t cst816_sleep(TOUCH_DEVICE *dev) {
    uint8_t sleep_cmd = 0x03U;

    if ((dev == NULL) || (dev->bus == NULL) || (dev->bus->write_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    return dev->bus->write_reg(dev->address, CST816_REG_SLEEP_MODE, &sleep_cmd, 1U);
}

/**
 * @function:   cst816_wakeup
 * @breif:      Wake CST816 by reset sequence.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t cst816_wakeup(TOUCH_DEVICE *dev) {
    if (dev == NULL) {
        return TOUCH_ERROR_PARAM;
    }

    cst816_reset(dev);
    return TOUCH_OK;
}

static const TOUCH_CONTROLLER cst816_controller = {
    .init = cst816_init,
    .read_point = cst816_read_point,
    .sleep = cst816_sleep,
    .wakeup = cst816_wakeup,
};

/**
 * @function:     cst816_register
 * @breif:        Register CST816 as current touch controller.
 * @param dev:    Optional user device object, NULL to use internal object.
 * @param bus:    Board touch bus operation table.
 * @param width:  Touch panel width.
 * @param height: Touch panel height.
 * @retval:       TOUCH_STATUS.
 */
uint8_t cst816_register(TOUCH_DEVICE *dev, TOUCH_BUS_OPS *bus, uint16_t width, uint16_t height) {
    TOUCH_DEVICE *target = dev ? dev : &cst816_dev;

    if ((bus == NULL) || (width == 0U) || (height == 0U)) {
        return TOUCH_ERROR_PARAM;
    }

    target->address = CST816_DEFAULT_ADDR;
    target->width = width;
    target->height = height;
    target->bus = bus;
    target->priv = &cst816_ctx;

    return touch_register(target, &cst816_controller);
}

/**
 * @function:   cst816_get_controller
 * @breif:      Get CST816 controller operation table.
 * @param:      NULL
 * @retval:     TOUCH_CONTROLLER pointer.
 */
const TOUCH_CONTROLLER *cst816_get_controller(void) {
    return &cst816_controller;
}
