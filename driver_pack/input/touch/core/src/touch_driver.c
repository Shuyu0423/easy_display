#include "touch_driver.h"
#include <stddef.h>

static TOUCH_DEVICE *touch_dev;
static const TOUCH_CONTROLLER *touch_controller;

/**
 * @function:          touch_register
 * @breif:             Register a touch device and its controller driver.
 * @param dev:         Touch device object.
 * @param controller:  Touch controller operation table.
 * @retval:            TOUCH_STATUS.
 */
uint8_t touch_register(TOUCH_DEVICE *dev, const TOUCH_CONTROLLER *controller) {
    if ((dev == NULL) || (controller == NULL)) {
        return TOUCH_ERROR_PARAM;
    }

    touch_dev = dev;
    touch_controller = controller;
    return TOUCH_OK;
}

/**
 * @function:   touch_init
 * @breif:      Initialize registered touch controller.
 * @param:      NULL
 * @retval:     TOUCH_STATUS.
 */
uint8_t touch_init(void) {
    if ((touch_dev == NULL) || (touch_controller == NULL) || (touch_controller->init == NULL)) {
        return TOUCH_ERROR_NOT_READY;
    }

    return touch_controller->init(touch_dev);
}

/**
 * @function:     touch_read_point
 * @breif:        Read current touch point from registered controller.
 * @param point:  Output touch point.
 * @retval:       TOUCH_STATUS.
 */
uint8_t touch_read_point(TOUCH_POINT *point) {
    if ((touch_dev == NULL) || (touch_controller == NULL) ||
        (touch_controller->read_point == NULL)) {
        return TOUCH_ERROR_NOT_READY;
    }

    return touch_controller->read_point(touch_dev, point);
}

/**
 * @function:   touch_sleep
 * @breif:      Put registered touch controller into sleep mode.
 * @param:      NULL
 * @retval:     TOUCH_STATUS.
 */
uint8_t touch_sleep(void) {
    if ((touch_dev == NULL) || (touch_controller == NULL) || (touch_controller->sleep == NULL)) {
        return TOUCH_ERROR_NOT_READY;
    }

    return touch_controller->sleep(touch_dev);
}

/**
 * @function:   touch_wakeup
 * @breif:      Wake up registered touch controller.
 * @param:      NULL
 * @retval:     TOUCH_STATUS.
 */
uint8_t touch_wakeup(void) {
    if ((touch_dev == NULL) || (touch_controller == NULL) || (touch_controller->wakeup == NULL)) {
        return TOUCH_ERROR_NOT_READY;
    }

    return touch_controller->wakeup(touch_dev);
}

/**
 * @function:   touch_get_device
 * @breif:      Get current registered touch device object.
 * @param:      NULL
 * @retval:     Touch device pointer.
 */
TOUCH_DEVICE *touch_get_device(void) {
    return touch_dev;
}
