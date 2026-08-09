#include "easy_display/display.h"
#include "easy_display/input.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    easy_display_flush_done_cb_t pending_done;
    void *pending_context;
    bool asynchronous;
    size_t received_bytes;
} display_backend_t;

static easy_device_status_t backend_display_init(void *context) {
    return (context != NULL) ? EASY_DEVICE_OK : EASY_DEVICE_ERROR_PARAM;
}

static easy_device_status_t backend_display_flush(
    void *context,
    const easy_display_area_t *area,
    const void *pixels,
    size_t byte_count,
    easy_display_flush_done_cb_t done,
    void *done_context) {
    display_backend_t *backend = (display_backend_t *)context;

    assert(area != NULL);
    assert(pixels != NULL);
    backend->received_bytes = byte_count;

    if (backend->asynchronous) {
        backend->pending_done = done;
        backend->pending_context = done_context;
    } else {
        done(done_context, EASY_DEVICE_OK);
    }
    return EASY_DEVICE_OK;
}

static easy_device_status_t backend_pointer_read(
    void *context,
    easy_pointer_data_t *data) {
    (void)context;
    data->x = 12U;
    data->y = 34U;
    data->pressed = true;
    data->event = EASY_POINTER_EVENT_PRESS;
    return EASY_DEVICE_OK;
}

static void user_flush_done(void *context, easy_device_status_t status) {
    uint32_t *count = (uint32_t *)context;
    assert(status == EASY_DEVICE_OK);
    (*count)++;
}

int main(void) {
    static const easy_display_ops_t display_ops = {
        .init = backend_display_init,
        .flush = backend_display_flush,
    };
    static const easy_pointer_ops_t pointer_ops = {
        .read = backend_pointer_read,
    };
    const easy_display_info_t display_info = {
        .width = 240U,
        .height = 320U,
        .pixel_format = EASY_DISPLAY_PIXEL_FORMAT_RGB565,
        .bytes_per_pixel = 2U,
        .capabilities = EASY_DISPLAY_CAP_ASYNC_FLUSH,
    };
    const easy_pointer_info_t pointer_info = {
        .width = 240U,
        .height = 320U,
    };
    const easy_display_area_t area = {0U, 0U, 9U, 4U};
    easy_display_t display;
    easy_pointer_t pointer;
    easy_pointer_data_t pointer_data;
    display_backend_t backend;
    uint8_t pixels[10U * 5U * 2U];
    uint32_t done_count = 0U;

    memset(&backend, 0, sizeof(backend));
    assert(easy_display_configure(&display, &display_info, &display_ops,
                                  &backend) == EASY_DEVICE_OK);
    assert(easy_display_init(&display) == EASY_DEVICE_OK);

    assert(easy_display_flush(&display, &area, pixels, sizeof(pixels),
                              user_flush_done, &done_count) == EASY_DEVICE_OK);
    assert(done_count == 1U);
    assert(!easy_display_is_busy(&display));
    assert(backend.received_bytes == sizeof(pixels));

    backend.asynchronous = true;
    assert(easy_display_flush(&display, &area, pixels, sizeof(pixels),
                              user_flush_done, &done_count) == EASY_DEVICE_OK);
    assert(easy_display_is_busy(&display));
    assert(easy_display_flush(&display, &area, pixels, sizeof(pixels),
                              user_flush_done, &done_count) == EASY_DEVICE_ERROR_BUSY);
    backend.pending_done(backend.pending_context, EASY_DEVICE_OK);
    assert(done_count == 2U);
    assert(!easy_display_is_busy(&display));

    assert(easy_display_flush(&display, &area, pixels, sizeof(pixels) - 1U,
                              NULL, NULL) == EASY_DEVICE_ERROR_PARAM);
    assert(easy_display_deinit(&display) == EASY_DEVICE_OK);

    assert(easy_pointer_configure(&pointer, &pointer_info, &pointer_ops,
                                  NULL) == EASY_DEVICE_OK);
    assert(easy_pointer_init(&pointer) == EASY_DEVICE_OK);
    assert(easy_pointer_read(&pointer, &pointer_data) == EASY_DEVICE_OK);
    assert(pointer_data.x == 12U);
    assert(pointer_data.y == 34U);
    assert(pointer_data.pressed);
    assert(pointer_data.event == EASY_POINTER_EVENT_PRESS);
    assert(easy_pointer_deinit(&pointer) == EASY_DEVICE_OK);

    puts("easy_display device API tests passed");
    return 0;
}
