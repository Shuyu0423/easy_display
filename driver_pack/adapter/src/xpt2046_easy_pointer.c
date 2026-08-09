#include "xpt2046_easy_pointer.h"

#include "xpt2046.h"

#include <stddef.h>

static easy_device_status_t touch_status_to_easy(uint8_t status) {
    switch (status) {
        case TOUCH_OK:
            return EASY_DEVICE_OK;
        case TOUCH_ERROR_PARAM:
            return EASY_DEVICE_ERROR_PARAM;
        case TOUCH_ERROR_NOT_READY:
            return EASY_DEVICE_ERROR_NOT_READY;
        case TOUCH_ERROR_BUS:
            return EASY_DEVICE_ERROR_IO;
        default:
            return EASY_DEVICE_ERROR_IO;
    }
}

static easy_pointer_event_t touch_event_to_easy(TOUCH_EVENT event) {
    switch (event) {
        case TOUCH_EVENT_PRESS:
            return EASY_POINTER_EVENT_PRESS;
        case TOUCH_EVENT_RELEASE:
            return EASY_POINTER_EVENT_RELEASE;
        case TOUCH_EVENT_CONTACT:
            return EASY_POINTER_EVENT_CONTACT;
        case TOUCH_EVENT_CLICK:
            return EASY_POINTER_EVENT_CLICK;
        case TOUCH_EVENT_DOUBLE_CLICK:
            return EASY_POINTER_EVENT_DOUBLE_CLICK;
        case TOUCH_EVENT_LONG_PRESS:
            return EASY_POINTER_EVENT_LONG_PRESS;
        case TOUCH_EVENT_SWIPE_UP:
            return EASY_POINTER_EVENT_SWIPE_UP;
        case TOUCH_EVENT_SWIPE_DOWN:
            return EASY_POINTER_EVENT_SWIPE_DOWN;
        case TOUCH_EVENT_SWIPE_LEFT:
            return EASY_POINTER_EVENT_SWIPE_LEFT;
        case TOUCH_EVENT_SWIPE_RIGHT:
            return EASY_POINTER_EVENT_SWIPE_RIGHT;
        case TOUCH_EVENT_NONE:
        default:
            return EASY_POINTER_EVENT_NONE;
    }
}

static easy_device_status_t xpt2046_backend_init(void *context) {
    xpt2046_easy_pointer_context_t *adapter =
        (xpt2046_easy_pointer_context_t *)context;
    const TOUCH_CONTROLLER *controller;
    uint8_t status;

    if ((adapter == NULL) || (adapter->bus == NULL)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    status = xpt2046_register(&adapter->legacy_device, adapter->bus,
                              adapter->width, adapter->height);
    if (status != TOUCH_OK) {
        return touch_status_to_easy(status);
    }

    xpt2046_set_calibration(adapter->raw_min_x, adapter->raw_max_x,
                            adapter->raw_min_y, adapter->raw_max_y,
                            adapter->swap_xy, adapter->mirror_x,
                            adapter->mirror_y);

    controller = xpt2046_get_controller();
    if ((controller == NULL) || (controller->init == NULL)) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    return touch_status_to_easy(controller->init(&adapter->legacy_device));
}

static easy_device_status_t xpt2046_backend_read(
    void *context,
    easy_pointer_data_t *data) {
    xpt2046_easy_pointer_context_t *adapter =
        (xpt2046_easy_pointer_context_t *)context;
    const TOUCH_CONTROLLER *controller = xpt2046_get_controller();
    TOUCH_POINT point;
    uint8_t status;

    if ((adapter == NULL) || (data == NULL) || (controller == NULL) ||
        (controller->read_point == NULL)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    status = controller->read_point(&adapter->legacy_device, &point);
    if (status != TOUCH_OK) {
        return touch_status_to_easy(status);
    }

    data->x = point.x;
    data->y = point.y;
    data->pressed = point.pressed != 0U;
    data->event = touch_event_to_easy(point.event);
    return EASY_DEVICE_OK;
}

static easy_device_status_t xpt2046_backend_sleep(void *context) {
    xpt2046_easy_pointer_context_t *adapter =
        (xpt2046_easy_pointer_context_t *)context;
    const TOUCH_CONTROLLER *controller = xpt2046_get_controller();

    if ((adapter == NULL) || (controller == NULL) ||
        (controller->sleep == NULL)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    return touch_status_to_easy(controller->sleep(&adapter->legacy_device));
}

static easy_device_status_t xpt2046_backend_wakeup(void *context) {
    xpt2046_easy_pointer_context_t *adapter =
        (xpt2046_easy_pointer_context_t *)context;
    const TOUCH_CONTROLLER *controller = xpt2046_get_controller();

    if ((adapter == NULL) || (controller == NULL) ||
        (controller->wakeup == NULL)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    return touch_status_to_easy(controller->wakeup(&adapter->legacy_device));
}

static const easy_pointer_ops_t xpt2046_backend_ops = {
    .init = xpt2046_backend_init,
    .read = xpt2046_backend_read,
    .sleep = xpt2046_backend_sleep,
    .wakeup = xpt2046_backend_wakeup,
};

easy_device_status_t xpt2046_easy_pointer_create(
    easy_pointer_t *pointer,
    xpt2046_easy_pointer_context_t *context,
    TOUCH_BUS_OPS *bus,
    uint16_t width,
    uint16_t height) {
    easy_pointer_info_t info;

    if ((pointer == NULL) || (context == NULL) || (bus == NULL) ||
        (width == 0U) || (height == 0U)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    context->bus = bus;
    context->width = width;
    context->height = height;
    context->raw_min_x = XPT2046_DEFAULT_RAW_MIN_X;
    context->raw_max_x = XPT2046_DEFAULT_RAW_MAX_X;
    context->raw_min_y = XPT2046_DEFAULT_RAW_MIN_Y;
    context->raw_max_y = XPT2046_DEFAULT_RAW_MAX_Y;
    context->swap_xy = 0U;
    context->mirror_x = 0U;
    context->mirror_y = 0U;

    info.width = width;
    info.height = height;
    info.capabilities = EASY_POINTER_CAP_INTERRUPT | EASY_POINTER_CAP_SLEEP;

    return easy_pointer_configure(pointer, &info, &xpt2046_backend_ops,
                                  context);
}

