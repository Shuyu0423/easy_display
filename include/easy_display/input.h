#ifndef EASY_DISPLAY_INPUT_H
#define EASY_DISPLAY_INPUT_H

#include <stdbool.h>
#include <stdint.h>

#include "easy_display/status.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EASY_POINTER_CAP_INTERRUPT = (1UL << 0),
    EASY_POINTER_CAP_GESTURE = (1UL << 1),
    EASY_POINTER_CAP_SLEEP = (1UL << 2),
};

typedef enum {
    EASY_POINTER_EVENT_NONE = 0,
    EASY_POINTER_EVENT_PRESS,
    EASY_POINTER_EVENT_RELEASE,
    EASY_POINTER_EVENT_CONTACT,
    EASY_POINTER_EVENT_CLICK,
    EASY_POINTER_EVENT_DOUBLE_CLICK,
    EASY_POINTER_EVENT_LONG_PRESS,
    EASY_POINTER_EVENT_SWIPE_UP,
    EASY_POINTER_EVENT_SWIPE_DOWN,
    EASY_POINTER_EVENT_SWIPE_LEFT,
    EASY_POINTER_EVENT_SWIPE_RIGHT,
} easy_pointer_event_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t capabilities;
} easy_pointer_info_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    bool pressed;
    easy_pointer_event_t event;
} easy_pointer_data_t;

typedef struct {
    easy_device_status_t (*init)(void *context);
    easy_device_status_t (*deinit)(void *context);
    easy_device_status_t (*read)(
        void *context,
        easy_pointer_data_t *data);
    easy_device_status_t (*sleep)(void *context);
    easy_device_status_t (*wakeup)(void *context);
} easy_pointer_ops_t;

typedef struct {
    /* Internal runtime fields. Configure through easy_pointer_configure(). */
    const easy_pointer_ops_t *ops;
    void *context;
    easy_pointer_info_t info;
    bool configured;
    bool initialized;
} easy_pointer_t;

easy_device_status_t easy_pointer_configure(
    easy_pointer_t *pointer,
    const easy_pointer_info_t *info,
    const easy_pointer_ops_t *ops,
    void *context);

easy_device_status_t easy_pointer_init(easy_pointer_t *pointer);
easy_device_status_t easy_pointer_deinit(easy_pointer_t *pointer);
easy_device_status_t easy_pointer_read(
    easy_pointer_t *pointer,
    easy_pointer_data_t *data);
easy_device_status_t easy_pointer_sleep(easy_pointer_t *pointer);
easy_device_status_t easy_pointer_wakeup(easy_pointer_t *pointer);

const easy_pointer_info_t *easy_pointer_get_info(
    const easy_pointer_t *pointer);

#ifdef __cplusplus
}
#endif

#endif
