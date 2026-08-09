#ifndef TOUCH_DRIVER_H__
#define TOUCH_DRIVER_H__

#include <stdint.h>

typedef enum {
    TOUCH_OK = 0,
    TOUCH_ERROR,
    TOUCH_ERROR_PARAM,
    TOUCH_ERROR_BUS,
    TOUCH_ERROR_NOT_READY,
} TOUCH_STATUS;

typedef enum {
    TOUCH_EVENT_NONE = 0,
    TOUCH_EVENT_PRESS,
    TOUCH_EVENT_RELEASE,
    TOUCH_EVENT_CONTACT,
    TOUCH_EVENT_CLICK,
    TOUCH_EVENT_DOUBLE_CLICK,
    TOUCH_EVENT_LONG_PRESS,
    TOUCH_EVENT_SWIPE_UP,
    TOUCH_EVENT_SWIPE_DOWN,
    TOUCH_EVENT_SWIPE_LEFT,
    TOUCH_EVENT_SWIPE_RIGHT,
} TOUCH_EVENT;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t pressed;
    uint8_t point_num;
    TOUCH_EVENT event;
} TOUCH_POINT;

typedef struct {
    uint8_t (*init)(void);
    uint8_t (*read_reg)(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len);
    uint8_t (*write_reg)(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len);
    uint8_t (*transfer)(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len);
    void (*set_reset)(uint8_t level);
    uint8_t (*read_int)(void);
    void (*delay_ms)(uint32_t ms);
} TOUCH_BUS_OPS;

typedef struct {
    uint8_t address;
    uint16_t width;
    uint16_t height;
    TOUCH_BUS_OPS *bus;
    void *priv;
} TOUCH_DEVICE;

typedef struct {
    uint8_t (*init)(TOUCH_DEVICE *dev);
    uint8_t (*read_point)(TOUCH_DEVICE *dev, TOUCH_POINT *point);
    uint8_t (*sleep)(TOUCH_DEVICE *dev);
    uint8_t (*wakeup)(TOUCH_DEVICE *dev);
} TOUCH_CONTROLLER;

uint8_t touch_register(TOUCH_DEVICE *dev, const TOUCH_CONTROLLER *controller);
uint8_t touch_init(void);
uint8_t touch_read_point(TOUCH_POINT *point);
uint8_t touch_sleep(void);
uint8_t touch_wakeup(void);
TOUCH_DEVICE *touch_get_device(void);

#endif
