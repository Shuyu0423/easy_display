#include "cst816.h"
#include <stddef.h>

static CST816_CONTEXT cst816_ctx;
static TOUCH_DEVICE cst816_dev;

static TOUCH_EVENT cst816_parse_event(uint8_t gesture_id) {
    switch (gesture_id) {
        case CST816_GESTURE_SLIDE_UP:
            return TOUCH_EVENT_SWIPE_UP;
        case CST816_GESTURE_SLIDE_DOWN:
            return TOUCH_EVENT_SWIPE_DOWN;
        case CST816_GESTURE_SLIDE_LEFT:
            return TOUCH_EVENT_SWIPE_LEFT;
        case CST816_GESTURE_SLIDE_RIGHT:
            return TOUCH_EVENT_SWIPE_RIGHT;
        case CST816_GESTURE_SINGLE_CLICK:
            return TOUCH_EVENT_CLICK;
        case CST816_GESTURE_DOUBLE_CLICK:
            return TOUCH_EVENT_DOUBLE_CLICK;
        case CST816_GESTURE_LONG_PRESS:
            return TOUCH_EVENT_LONG_PRESS;
        default:
            return TOUCH_EVENT_CONTACT;
    }
}

static TOUCH_EVENT cst816_parse_point_event(uint8_t event) {
    switch (event) {
        case CST816_POINT_EVENT_PRESS:
            return TOUCH_EVENT_PRESS;
        case CST816_POINT_EVENT_RELEASE:
            return TOUCH_EVENT_RELEASE;
        case CST816_POINT_EVENT_CONTACT:
            return TOUCH_EVENT_CONTACT;
        default:
            return TOUCH_EVENT_NONE;
    }
}

static uint8_t cst816_is_valid_chip(uint8_t chip_id) {
    return ((chip_id == CST816_CHIP_ID_1) || (chip_id == CST816_CHIP_ID_2) ||
            (chip_id == CST816_CHIP_ID_3));
}

static uint8_t cst816_read_byte(TOUCH_DEVICE *dev, uint8_t reg, uint8_t *data) {
    if ((dev == NULL) || (dev->bus == NULL) || (dev->bus->read_reg == NULL) || (data == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    return dev->bus->read_reg(dev->address, reg, data, 1U);
}

static uint8_t cst816_write_byte(TOUCH_DEVICE *dev, uint8_t reg, uint8_t data) {
    if ((dev == NULL) || (dev->bus == NULL) || (dev->bus->write_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    return dev->bus->write_reg(dev->address, reg, &data, 1U);
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
        dev->bus->delay_ms(10);
    }
    dev->bus->set_reset(1);
    if (dev->bus->delay_ms) {
        dev->bus->delay_ms(100);
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

    if ((dev == NULL) || (dev->bus == NULL) || (dev->priv == NULL) || (dev->bus->read_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    if (dev->bus->init && (dev->bus->init() != TOUCH_OK)) {
        return TOUCH_ERROR_BUS;
    }

    cst816_reset(dev);

    if (cst816_read_byte(dev, CST816_REG_CHIP_ID, &chip_id) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }
    if (!cst816_is_valid_chip(chip_id)) {
        return TOUCH_ERROR;
    }

    ((CST816_CONTEXT *)dev->priv)->chip_id = chip_id;
    ((CST816_CONTEXT *)dev->priv)->last_pressed = 0U;

    if (cst816_read_byte(dev, CST816_REG_PROJECT_ID,
                         &((CST816_CONTEXT *)dev->priv)->project_id) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }
    if (cst816_read_byte(dev, CST816_REG_FW_VERSION,
                         &((CST816_CONTEXT *)dev->priv)->fw_version) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

    if (cst816_write_byte(dev, CST816_REG_AUTOSLEEP_TIME, CST816_DEFAULT_AUTOSLEEP_TIME) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }
    if (cst816_write_byte(dev, CST816_REG_IRQ_CTL, CST816_DEFAULT_IRQ_CTL) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

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
    uint8_t point_event;
    CST816_CONTEXT *ctx;

    if ((dev == NULL) || (point == NULL) || (dev->bus == NULL) || (dev->priv == NULL) ||
        (dev->bus->read_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ctx = (CST816_CONTEXT *)dev->priv;
    point->pressed = 0U;
    point->point_num = 0U;
    point->event = TOUCH_EVENT_NONE;

    if (dev->bus->read_int && (dev->bus->read_int() != 0U)) {
        if (ctx->last_pressed) {
            ctx->last_pressed = 0U;
            point->event = TOUCH_EVENT_RELEASE;
        }
        return TOUCH_OK;
    }

    if (dev->bus->read_reg(dev->address, CST816_REG_GESTURE_ID, buf, sizeof(buf)) != TOUCH_OK) {
        return TOUCH_ERROR_BUS;
    }

    point->point_num = buf[1] & 0x0FU;
    if (point->point_num == 0U) {
        if (ctx->last_pressed) {
            ctx->last_pressed = 0U;
            point->event = TOUCH_EVENT_RELEASE;
        }
        return TOUCH_OK;
    }

    point->pressed = 1U;
    point->event = cst816_parse_event(buf[0]);
    if (buf[0] == CST816_GESTURE_NONE) {
        point_event = (uint8_t)(buf[2] >> CST816_POINT_EVENT_SHIFT);
        point->event = cst816_parse_point_event(point_event);
        if (point->event == TOUCH_EVENT_NONE) {
            point->event = ctx->last_pressed ? TOUCH_EVENT_CONTACT : TOUCH_EVENT_PRESS;
        }
    }
    ctx->last_pressed = 1U;

    point->x = (uint16_t)(((buf[2] & CST816_POINT_COORD_H_MASK) << 8U) | buf[3]);
    point->y = (uint16_t)(((buf[4] & CST816_POINT_COORD_H_MASK) << 8U) | buf[5]);

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
    uint8_t sleep_cmd = CST816_SLEEP_CMD;

    if ((dev == NULL) || (dev->bus == NULL) || (dev->priv == NULL) || (dev->bus->write_reg == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ((CST816_CONTEXT *)dev->priv)->last_pressed = 0U;
    return dev->bus->write_reg(dev->address, CST816_REG_SLEEP_MODE, &sleep_cmd, 1U);
}

/**
 * @function:   cst816_wakeup
 * @breif:      Wake CST816 by reset sequence.
 * @param dev:  Touch device object.
 * @retval:     TOUCH_STATUS.
 */
static uint8_t cst816_wakeup(TOUCH_DEVICE *dev) {
    if ((dev == NULL) || (dev->priv == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    ((CST816_CONTEXT *)dev->priv)->last_pressed = 0U;
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

/**
 * @function:   cst816_get_context
 * @breif:      Get CST816 runtime context.
 * @param:      NULL
 * @retval:     CST816_CONTEXT pointer.
 */
CST816_CONTEXT *cst816_get_context(void) {
    return &cst816_ctx;
}
