#ifndef CST816_H__
#define CST816_H__

#include "touch_driver.h"

#define CST816_DEFAULT_ADDR (0x15U)

#define CST816_REG_GESTURE_ID (0x01U)
#define CST816_REG_FINGER_NUM (0x02U)
#define CST816_REG_XPOS_H (0x03U)
#define CST816_REG_XPOS_L (0x04U)
#define CST816_REG_YPOS_H (0x05U)
#define CST816_REG_YPOS_L (0x06U)
#define CST816_REG_SLEEP_MODE (0xA5U)
#define CST816_REG_CHIP_ID (0xA7U)

typedef struct {
    uint8_t chip_id;
} CST816_CONTEXT;

uint8_t cst816_register(TOUCH_DEVICE *dev, TOUCH_BUS_OPS *bus, uint16_t width, uint16_t height);
const TOUCH_CONTROLLER *cst816_get_controller(void);

#endif
