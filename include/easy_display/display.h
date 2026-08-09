#ifndef EASY_DISPLAY_DISPLAY_H
#define EASY_DISPLAY_DISPLAY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "easy_display/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    EASY_DISPLAY_PIXEL_FORMAT_RGB565 = 0,
    EASY_DISPLAY_PIXEL_FORMAT_RGB888,
    EASY_DISPLAY_PIXEL_FORMAT_ARGB8888,
} easy_display_pixel_format_t;

typedef enum {
    EASY_DISPLAY_ROTATION_0 = 0,
    EASY_DISPLAY_ROTATION_90,
    EASY_DISPLAY_ROTATION_180,
    EASY_DISPLAY_ROTATION_270,
} easy_display_rotation_t;

enum {
    EASY_DISPLAY_CAP_ASYNC_FLUSH = (1UL << 0),
    EASY_DISPLAY_CAP_ROTATION = (1UL << 1),
    EASY_DISPLAY_CAP_SLEEP = (1UL << 2),
};

typedef struct {
    uint16_t width;
    uint16_t height;
    easy_display_pixel_format_t pixel_format;
    uint8_t bytes_per_pixel;
    uint32_t capabilities;
} easy_display_info_t;

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} easy_display_area_t;

/*
 * A backend must call the completion callback exactly once after accepting a
 * flush. Blocking backends may call it before returning; DMA backends call it
 * when the transfer completes. If flush() returns an error, it must not call
 * the completion callback.
 */
typedef void (*easy_display_flush_done_cb_t)(
    void *context,
    easy_device_status_t status);

typedef struct {
    easy_device_status_t (*init)(void *context);
    easy_device_status_t (*deinit)(void *context);
    easy_device_status_t (*flush)(
        void *context,
        const easy_display_area_t *area,
        const void *pixels,
        size_t byte_count,
        easy_display_flush_done_cb_t done,
        void *done_context);
    easy_device_status_t (*set_rotation)(
        void *context,
        easy_display_rotation_t rotation);
    easy_device_status_t (*sleep)(void *context);
    easy_device_status_t (*wakeup)(void *context);
} easy_display_ops_t;

typedef struct {
    /* Internal runtime fields. Configure through easy_display_configure(). */
    const easy_display_ops_t *ops;
    void *context;
    easy_display_info_t info;
    easy_display_flush_done_cb_t pending_done;
    void *pending_done_context;
    bool configured;
    bool initialized;
    bool flush_pending;
} easy_display_t;

easy_device_status_t easy_display_configure(
    easy_display_t *display,
    const easy_display_info_t *info,
    const easy_display_ops_t *ops,
    void *context);

easy_device_status_t easy_display_init(easy_display_t *display);
easy_device_status_t easy_display_deinit(easy_display_t *display);

easy_device_status_t easy_display_flush(
    easy_display_t *display,
    const easy_display_area_t *area,
    const void *pixels,
    size_t byte_count,
    easy_display_flush_done_cb_t done,
    void *done_context);

easy_device_status_t easy_display_set_rotation(
    easy_display_t *display,
    easy_display_rotation_t rotation);

easy_device_status_t easy_display_sleep(easy_display_t *display);
easy_device_status_t easy_display_wakeup(easy_display_t *display);

const easy_display_info_t *easy_display_get_info(
    const easy_display_t *display);

bool easy_display_is_busy(const easy_display_t *display);

#ifdef __cplusplus
}
#endif

#endif
