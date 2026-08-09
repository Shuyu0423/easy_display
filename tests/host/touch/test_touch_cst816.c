#include "cst816.h"
#include <stdio.h>
#include <string.h>

static uint8_t mock_regs[256];
static uint8_t mock_int_level;
static uint8_t mock_last_write_reg;
static uint8_t mock_last_write_value;
static uint8_t mock_bus_init_count;
static uint8_t mock_reset_count;
static uint8_t mock_delay_count;

static int expect_true(int condition, const char *message)
{
    if (!condition) {
        printf("FAIL: %s\n", message);
        return 1;
    }
    return 0;
}

static uint8_t mock_bus_init(void)
{
    mock_bus_init_count++;
    return TOUCH_OK;
}

static uint8_t mock_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    (void)dev_addr;
    memcpy(data, &mock_regs[reg], len);
    return TOUCH_OK;
}

static uint8_t mock_write_reg(uint8_t dev_addr, uint8_t reg, const uint8_t *data, uint16_t len)
{
    (void)dev_addr;
    mock_last_write_reg = reg;
    mock_last_write_value = (len > 0U) ? data[0] : 0U;
    return TOUCH_OK;
}

static void mock_set_reset(uint8_t level)
{
    (void)level;
    mock_reset_count++;
}

static uint8_t mock_read_int(void)
{
    return mock_int_level;
}

static void mock_delay_ms(uint32_t ms)
{
    (void)ms;
    mock_delay_count++;
}

int main(void)
{
    TOUCH_BUS_OPS bus = {
        .init = mock_bus_init,
        .read_reg = mock_read_reg,
        .write_reg = mock_write_reg,
        .set_reset = mock_set_reset,
        .read_int = mock_read_int,
        .delay_ms = mock_delay_ms,
    };
    TOUCH_POINT point = {0};
    TOUCH_DEVICE *dev;
    CST816_CONTEXT *ctx;
    int failed = 0;

    failed |= expect_true(touch_init() == TOUCH_ERROR_NOT_READY, "touch_init rejects missing registration");

    mock_regs[CST816_REG_CHIP_ID] = 0xB5U;
    failed |= expect_true(cst816_register(NULL, &bus, 240U, 280U) == TOUCH_OK, "cst816_register succeeds");
    failed |= expect_true(touch_init() == TOUCH_OK, "touch_init succeeds after registration");
    failed |= expect_true(mock_bus_init_count == 1U, "bus init called once");
    failed |= expect_true(mock_reset_count == 2U, "reset pin toggled during init");
    failed |= expect_true(mock_delay_count == 2U, "reset delays called during init");

    dev = touch_get_device();
    ctx = (CST816_CONTEXT *)dev->priv;
    failed |= expect_true(dev != NULL, "registered device is available");
    failed |= expect_true(ctx->chip_id == 0xB5U, "chip id stored in driver context");

    mock_int_level = 1U;
    failed |= expect_true(touch_read_point(&point) == TOUCH_OK, "read point returns OK when INT inactive");
    failed |= expect_true(point.pressed == 0U, "inactive INT reports no press");
    failed |= expect_true(point.event == TOUCH_EVENT_NONE, "inactive INT reports no event");

    mock_int_level = 0U;
    mock_regs[CST816_REG_GESTURE_ID] = 0x03U;
    mock_regs[CST816_REG_FINGER_NUM] = 0x01U;
    mock_regs[CST816_REG_XPOS_H] = 0x01U;
    mock_regs[CST816_REG_XPOS_L] = 0x2CU;
    mock_regs[CST816_REG_YPOS_H] = 0x00U;
    mock_regs[CST816_REG_YPOS_L] = 0x64U;
    memset(&point, 0, sizeof(point));

    failed |= expect_true(touch_read_point(&point) == TOUCH_OK, "read point succeeds when INT active");
    failed |= expect_true(point.pressed == 1U, "active point reports pressed");
    failed |= expect_true(point.point_num == 1U, "finger count parsed");
    failed |= expect_true(point.event == TOUCH_EVENT_SWIPE_LEFT, "gesture parsed");
    failed |= expect_true(point.x == 239U, "x coordinate clamps to panel width");
    failed |= expect_true(point.y == 100U, "y coordinate parsed");

    failed |= expect_true(touch_sleep() == TOUCH_OK, "sleep command succeeds");
    failed |= expect_true(mock_last_write_reg == CST816_REG_SLEEP_MODE, "sleep register selected");
    failed |= expect_true(mock_last_write_value == 0x03U, "sleep command value written");

    if (failed) {
        return 1;
    }

    printf("touch CST816 smoke test passed\n");
    return 0;
}
