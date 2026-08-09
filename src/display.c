#include "easy_display/display.h"

#include <string.h>

static bool easy_display_info_is_valid(const easy_display_info_t *info) {
    uint8_t expected_bytes_per_pixel;

    if ((info == NULL) || (info->width == 0U) || (info->height == 0U)) {
        return false;
    }

    switch (info->pixel_format) {
        case EASY_DISPLAY_PIXEL_FORMAT_RGB565:
            expected_bytes_per_pixel = 2U;
            break;
        case EASY_DISPLAY_PIXEL_FORMAT_RGB888:
            expected_bytes_per_pixel = 3U;
            break;
        case EASY_DISPLAY_PIXEL_FORMAT_ARGB8888:
            expected_bytes_per_pixel = 4U;
            break;
        default:
            return false;
    }

    return info->bytes_per_pixel == expected_bytes_per_pixel;
}

static bool easy_display_area_is_valid(const easy_display_t *display,
                                       const easy_display_area_t *area) {
    if ((display == NULL) || (area == NULL)) {
        return false;
    }

    return (area->x1 <= area->x2) && (area->y1 <= area->y2) &&
           (area->x2 < display->info.width) &&
           (area->y2 < display->info.height);
}

static size_t easy_display_area_byte_count(const easy_display_t *display,
                                           const easy_display_area_t *area) {
    size_t width = (size_t)area->x2 - area->x1 + 1U;
    size_t height = (size_t)area->y2 - area->y1 + 1U;
    return width * height * display->info.bytes_per_pixel;
}

static void easy_display_backend_flush_done(void *context,
                                            easy_device_status_t status) {
    easy_display_t *display = (easy_display_t *)context;
    easy_display_flush_done_cb_t done;
    void *done_context;

    if ((display == NULL) || !display->flush_pending) {
        return;
    }

    done = display->pending_done;
    done_context = display->pending_done_context;
    display->pending_done = NULL;
    display->pending_done_context = NULL;
    display->flush_pending = false;

    if (done != NULL) {
        done(done_context, status);
    }
}

easy_device_status_t easy_display_configure(
    easy_display_t *display,
    const easy_display_info_t *info,
    const easy_display_ops_t *ops,
    void *context) {
    if ((display == NULL) || !easy_display_info_is_valid(info) ||
        (ops == NULL) || (ops->flush == NULL)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    memset(display, 0, sizeof(*display));
    display->ops = ops;
    display->context = context;
    display->info = *info;
    display->configured = true;
    return EASY_DEVICE_OK;
}

easy_device_status_t easy_display_init(easy_display_t *display) {
    easy_device_status_t status = EASY_DEVICE_OK;

    if ((display == NULL) || !display->configured) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if (display->initialized) {
        return EASY_DEVICE_ERROR_STATE;
    }

    if (display->ops->init != NULL) {
        status = display->ops->init(display->context);
    }
    if (status == EASY_DEVICE_OK) {
        display->initialized = true;
    }
    return status;
}

easy_device_status_t easy_display_deinit(easy_display_t *display) {
    easy_device_status_t status = EASY_DEVICE_OK;

    if ((display == NULL) || !display->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if (display->flush_pending) {
        return EASY_DEVICE_ERROR_BUSY;
    }

    if (display->ops->deinit != NULL) {
        status = display->ops->deinit(display->context);
    }
    if (status == EASY_DEVICE_OK) {
        display->initialized = false;
    }
    return status;
}

easy_device_status_t easy_display_flush(
    easy_display_t *display,
    const easy_display_area_t *area,
    const void *pixels,
    size_t byte_count,
    easy_display_flush_done_cb_t done,
    void *done_context) {
    easy_device_status_t status;
    size_t required_byte_count;

    if ((display == NULL) || !display->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if ((pixels == NULL) || !easy_display_area_is_valid(display, area)) {
        return EASY_DEVICE_ERROR_PARAM;
    }
    if (display->flush_pending) {
        return EASY_DEVICE_ERROR_BUSY;
    }

    required_byte_count = easy_display_area_byte_count(display, area);
    if (byte_count < required_byte_count) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    display->pending_done = done;
    display->pending_done_context = done_context;
    display->flush_pending = true;

    status = display->ops->flush(display->context,
                                 area,
                                 pixels,
                                 required_byte_count,
                                 easy_display_backend_flush_done,
                                 display);
    if ((status != EASY_DEVICE_OK) && display->flush_pending) {
        display->pending_done = NULL;
        display->pending_done_context = NULL;
        display->flush_pending = false;
    }

    return status;
}

easy_device_status_t easy_display_set_rotation(
    easy_display_t *display,
    easy_display_rotation_t rotation) {
    if ((display == NULL) || !display->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if ((display->ops->set_rotation == NULL) ||
        ((display->info.capabilities & EASY_DISPLAY_CAP_ROTATION) == 0U)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    if (display->flush_pending) {
        return EASY_DEVICE_ERROR_BUSY;
    }

    return display->ops->set_rotation(display->context, rotation);
}

easy_device_status_t easy_display_sleep(easy_display_t *display) {
    if ((display == NULL) || !display->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if ((display->ops->sleep == NULL) ||
        ((display->info.capabilities & EASY_DISPLAY_CAP_SLEEP) == 0U)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    if (display->flush_pending) {
        return EASY_DEVICE_ERROR_BUSY;
    }
    return display->ops->sleep(display->context);
}

easy_device_status_t easy_display_wakeup(easy_display_t *display) {
    if ((display == NULL) || !display->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if ((display->ops->wakeup == NULL) ||
        ((display->info.capabilities & EASY_DISPLAY_CAP_SLEEP) == 0U)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    if (display->flush_pending) {
        return EASY_DEVICE_ERROR_BUSY;
    }
    return display->ops->wakeup(display->context);
}

const easy_display_info_t *easy_display_get_info(
    const easy_display_t *display) {
    if ((display == NULL) || !display->configured) {
        return NULL;
    }
    return &display->info;
}

bool easy_display_is_busy(const easy_display_t *display) {
    return (display != NULL) && display->flush_pending;
}
