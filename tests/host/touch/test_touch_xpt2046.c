#include "xpt2046.h"
#include <stdio.h>
#include <string.h>

static uint8_t mock_int_level = 1U;
static uint16_t mock_raw_x = 2000U;
static uint16_t mock_raw_y = 2500U;
static uint8_t mock_bus_init_count;
static uint8_t mock_transfer_count;

static int expect_true(int condition, const char *message) {
    if (!condition) {
        printf("FAIL: %s\n", message);
        return 1;
    }
    return 0;
}

static uint8_t mock_bus_init(void) {
    mock_bus_init_count++;
    return TOUCH_OK;
}

static uint8_t mock_transfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len) {
    uint16_t value = 0U;

    if ((tx_data == NULL) || (rx_data == NULL) || (len != 3U)) {
        return TOUCH_ERROR_PARAM;
    }

    if (tx_data[0] == XPT2046_CMD_READ_X) {
        value = mock_raw_x;
    } else if (tx_data[0] == XPT2046_CMD_READ_Y) {
        value = mock_raw_y;
    }

    memset(rx_data, 0, len);
    rx_data[1] = (uint8_t)((value << 3U) >> 8U);
    rx_data[2] = (uint8_t)((value << 3U) & 0xFFU);
    mock_transfer_count++;
    return TOUCH_OK;
}

static uint8_t mock_read_int(void) {
    return mock_int_level;
}

int main(void) {
    TOUCH_BUS_OPS bus = {
        .init = mock_bus_init,
        .transfer = mock_transfer,
        .read_int = mock_read_int,
    };
    TOUCH_POINT point = {0};
    XPT2046_CONTEXT *ctx;
    int failed = 0;

    failed |= expect_true(xpt2046_register(NULL, &bus, 240U, 320U) == TOUCH_OK,
                          "xpt2046_register succeeds");
    failed |= expect_true(touch_init() == TOUCH_OK, "touch_init succeeds");
    failed |= expect_true(mock_bus_init_count == 1U, "bus init called once");

    ctx = xpt2046_get_context();
    failed |= expect_true(ctx->raw_min_x == XPT2046_DEFAULT_RAW_MIN_X,
                          "default raw min x configured");

    failed |= expect_true(touch_read_point(&point) == TOUCH_OK, "inactive read succeeds");
    failed |= expect_true(point.pressed == 0U, "inactive read reports no press");

    xpt2046_set_calibration(1000U, 3000U, 1000U, 3000U, 0U, 0U, 0U);
    mock_int_level = 0U;
    mock_raw_x = 2000U;
    mock_raw_y = 2500U;
    memset(&point, 0, sizeof(point));

    failed |= expect_true(touch_read_point(&point) == TOUCH_OK, "pressed read succeeds");
    failed |= expect_true(point.pressed == 1U, "pressed read reports pressed");
    failed |= expect_true(point.point_num == 1U, "pressed read reports one point");
    failed |= expect_true(point.event == TOUCH_EVENT_PRESS, "first point reports press");
    failed |= expect_true(point.x == 119U, "x coordinate mapped");
    failed |= expect_true(point.y == 239U, "y coordinate mapped");
    failed |= expect_true(mock_transfer_count >= (XPT2046_SAMPLE_COUNT * 2U),
                          "filtered read uses repeated samples");

    memset(&point, 0, sizeof(point));
    failed |= expect_true(touch_read_point(&point) == TOUCH_OK, "contact read succeeds");
    failed |= expect_true(point.event == TOUCH_EVENT_CONTACT, "second point reports contact");

    mock_int_level = 1U;
    memset(&point, 0, sizeof(point));
    failed |= expect_true(touch_read_point(&point) == TOUCH_OK, "release read succeeds");
    failed |= expect_true(point.event == TOUCH_EVENT_RELEASE, "release event reported");

    if (failed) {
        return 1;
    }

    printf("touch XPT2046 smoke test passed\n");
    return 0;
}
