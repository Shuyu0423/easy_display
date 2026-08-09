#include "easy_display/input.h"

#include <string.h>

static bool easy_pointer_info_is_valid(const easy_pointer_info_t *info) {
    return (info != NULL) && (info->width > 0U) && (info->height > 0U);
}

easy_device_status_t easy_pointer_configure(
    easy_pointer_t *pointer,
    const easy_pointer_info_t *info,
    const easy_pointer_ops_t *ops,
    void *context) {
    if ((pointer == NULL) || !easy_pointer_info_is_valid(info) ||
        (ops == NULL) || (ops->read == NULL)) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    memset(pointer, 0, sizeof(*pointer));
    pointer->ops = ops;
    pointer->context = context;
    pointer->info = *info;
    pointer->configured = true;
    return EASY_DEVICE_OK;
}

easy_device_status_t easy_pointer_init(easy_pointer_t *pointer) {
    easy_device_status_t status = EASY_DEVICE_OK;

    if ((pointer == NULL) || !pointer->configured) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if (pointer->initialized) {
        return EASY_DEVICE_ERROR_STATE;
    }

    if (pointer->ops->init != NULL) {
        status = pointer->ops->init(pointer->context);
    }
    if (status == EASY_DEVICE_OK) {
        pointer->initialized = true;
    }
    return status;
}

easy_device_status_t easy_pointer_deinit(easy_pointer_t *pointer) {
    easy_device_status_t status = EASY_DEVICE_OK;

    if ((pointer == NULL) || !pointer->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }

    if (pointer->ops->deinit != NULL) {
        status = pointer->ops->deinit(pointer->context);
    }
    if (status == EASY_DEVICE_OK) {
        pointer->initialized = false;
    }
    return status;
}

easy_device_status_t easy_pointer_read(
    easy_pointer_t *pointer,
    easy_pointer_data_t *data) {
    easy_device_status_t status;

    if ((pointer == NULL) || !pointer->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if (data == NULL) {
        return EASY_DEVICE_ERROR_PARAM;
    }

    memset(data, 0, sizeof(*data));
    status = pointer->ops->read(pointer->context, data);
    if (status != EASY_DEVICE_OK) {
        memset(data, 0, sizeof(*data));
        return status;
    }

    if ((data->x >= pointer->info.width) ||
        (data->y >= pointer->info.height)) {
        memset(data, 0, sizeof(*data));
        return EASY_DEVICE_ERROR_IO;
    }

    return EASY_DEVICE_OK;
}

easy_device_status_t easy_pointer_sleep(easy_pointer_t *pointer) {
    if ((pointer == NULL) || !pointer->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if ((pointer->ops->sleep == NULL) ||
        ((pointer->info.capabilities & EASY_POINTER_CAP_SLEEP) == 0U)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    return pointer->ops->sleep(pointer->context);
}

easy_device_status_t easy_pointer_wakeup(easy_pointer_t *pointer) {
    if ((pointer == NULL) || !pointer->initialized) {
        return EASY_DEVICE_ERROR_NOT_READY;
    }
    if ((pointer->ops->wakeup == NULL) ||
        ((pointer->info.capabilities & EASY_POINTER_CAP_SLEEP) == 0U)) {
        return EASY_DEVICE_ERROR_NOT_SUPPORTED;
    }
    return pointer->ops->wakeup(pointer->context);
}

const easy_pointer_info_t *easy_pointer_get_info(
    const easy_pointer_t *pointer) {
    if ((pointer == NULL) || !pointer->configured) {
        return NULL;
    }
    return &pointer->info;
}
