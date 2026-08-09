#include "user_conf.h"
#include "cst816_calibration_test.h"

#if TOUCH_CST816_CALIBRATION_TEST_ENABLE

#include "board.h"
#include "cst816.h"
#include "log_backend_rtt.h"
#include "mcu_touch_iic.h"
#include "touch_driver.h"

#define TOUCH_CAL_RTT_TERMINAL_ID (0U)
#define TOUCH_CAL_RTT_CHANNEL_INDEX (0U)
#define TOUCH_CAL_PRINT(...) \
    log_backend_rtt_printf(TOUCH_CAL_RTT_TERMINAL_ID, TOUCH_CAL_RTT_CHANNEL_INDEX, __VA_ARGS__)

typedef struct {
    const char *name;
    uint32_t x_sum;
    uint32_t y_sum;
    uint16_t x_avg;
    uint16_t y_avg;
    uint16_t count;
} TOUCH_CAL_POINT;

typedef struct {
    uint8_t ready;
    uint8_t swap_xy;
    uint8_t mirror_x;
    uint8_t mirror_y;
    uint16_t x_min;
    uint16_t x_max;
    uint16_t y_min;
    uint16_t y_max;
} TOUCH_CAL_MAP;

static TOUCH_CAL_POINT cal_points[] = {
    {.name = "TOP_LEFT"},
    {.name = "TOP_RIGHT"},
    {.name = "BOTTOM_RIGHT"},
    {.name = "BOTTOM_LEFT"},
};

static TOUCH_CAL_MAP cal_map;

static const char *touch_event_name(TOUCH_EVENT event) {
    switch (event) {
        case TOUCH_EVENT_NONE:
            return "NONE";
        case TOUCH_EVENT_PRESS:
            return "PRESS";
        case TOUCH_EVENT_RELEASE:
            return "RELEASE";
        case TOUCH_EVENT_CONTACT:
            return "CONTACT";
        case TOUCH_EVENT_CLICK:
            return "CLICK";
        case TOUCH_EVENT_DOUBLE_CLICK:
            return "DOUBLE_CLICK";
        case TOUCH_EVENT_LONG_PRESS:
            return "LONG_PRESS";
        case TOUCH_EVENT_SWIPE_UP:
            return "SWIPE_UP";
        case TOUCH_EVENT_SWIPE_DOWN:
            return "SWIPE_DOWN";
        case TOUCH_EVENT_SWIPE_LEFT:
            return "SWIPE_LEFT";
        case TOUCH_EVENT_SWIPE_RIGHT:
            return "SWIPE_RIGHT";
        default:
            return "UNKNOWN";
    }
}

static void touch_cal_print_context(void) {
    CST816_CONTEXT *ctx = cst816_get_context();

    TOUCH_CAL_PRINT("CST816 chip_id=0x%02X project_id=0x%02X fw=0x%02X\r\n",
                    ctx->chip_id, ctx->project_id, ctx->fw_version);
}

static void touch_cal_wait_release(void) {
    TOUCH_POINT point;

    do {
        if (touch_read_point(&point) != TOUCH_OK) {
            TOUCH_CAL_PRINT("touch_read_point error while waiting release\r\n");
        }
        delay_ms(TOUCH_CAL_POLL_DELAY_MS);
    } while (point.pressed != 0U);
}

static void touch_cal_capture_point(TOUCH_CAL_POINT *cal_point) {
    TOUCH_POINT point;

    TOUCH_CAL_PRINT("\r\nPlace finger on %s, hold steady...\r\n", cal_point->name);
    cal_point->x_sum = 0U;
    cal_point->y_sum = 0U;
    cal_point->count = 0U;

    while (cal_point->count < TOUCH_CAL_STABLE_SAMPLES) {
        if (touch_read_point(&point) == TOUCH_OK) {
            if (point.pressed) {
                cal_point->x_sum += point.x;
                cal_point->y_sum += point.y;
                cal_point->count++;
                TOUCH_CAL_PRINT("%s sample %u/%u: x=%u y=%u event=%s\r\n",
                                cal_point->name,
                                cal_point->count,
                                (uint16_t)TOUCH_CAL_STABLE_SAMPLES,
                                point.x,
                                point.y,
                                touch_event_name(point.event));
            }
        } else {
            TOUCH_CAL_PRINT("%s read error\r\n", cal_point->name);
        }

        delay_ms(TOUCH_CAL_POLL_DELAY_MS);
    }

    cal_point->x_avg = (uint16_t)(cal_point->x_sum / cal_point->count);
    cal_point->y_avg = (uint16_t)(cal_point->y_sum / cal_point->count);

    TOUCH_CAL_PRINT("%s average: x=%u y=%u\r\n", cal_point->name, cal_point->x_avg, cal_point->y_avg);
    TOUCH_CAL_PRINT("Release finger...\r\n");
    touch_cal_wait_release();
}

static void touch_cal_print_result(void) {
    uint16_t min_x;
    uint16_t max_x;
    uint16_t min_y;
    uint16_t max_y;

    min_x = cal_map.x_min < cal_map.x_max ? cal_map.x_min : cal_map.x_max;
    max_x = cal_map.x_min < cal_map.x_max ? cal_map.x_max : cal_map.x_min;
    min_y = cal_map.y_min < cal_map.y_max ? cal_map.y_min : cal_map.y_max;
    max_y = cal_map.y_min < cal_map.y_max ? cal_map.y_max : cal_map.y_min;

    TOUCH_CAL_PRINT("\r\n=== CST816 calibration result ===\r\n");
    for (uint16_t i = 0U; i < (sizeof(cal_points) / sizeof(cal_points[0])); i++) {
        TOUCH_CAL_PRINT("%s: x=%u y=%u\r\n", cal_points[i].name, cal_points[i].x_avg, cal_points[i].y_avg);
    }
    TOUCH_CAL_PRINT("raw_range_x=%u..%u raw_range_y=%u..%u\r\n",
                    min_x, max_x, min_y, max_y);
    TOUCH_CAL_PRINT("panel_width=%u panel_height=%u\r\n",
                    (uint16_t)TOUCH_CAL_PANEL_WIDTH, (uint16_t)TOUCH_CAL_PANEL_HEIGHT);
    TOUCH_CAL_PRINT("swap_xy=%u mirror_x=%u mirror_y=%u\r\n",
                    cal_map.swap_xy, cal_map.mirror_x, cal_map.mirror_y);
    TOUCH_CAL_PRINT("map_x_min=%u map_x_max=%u map_y_min=%u map_y_max=%u\r\n",
                    cal_map.x_min, cal_map.x_max, cal_map.y_min, cal_map.y_max);
    TOUCH_CAL_PRINT("Live monitor will print raw and calibrated screen coordinates.\r\n");
}

static uint16_t touch_cal_abs_diff(uint16_t a, uint16_t b) {
    return (a > b) ? (uint16_t)(a - b) : (uint16_t)(b - a);
}

static uint16_t touch_cal_average2(uint16_t a, uint16_t b) {
    return (uint16_t)(((uint32_t)a + b) / 2U);
}

static uint16_t touch_cal_map_axis(uint16_t raw, uint16_t edge_a, uint16_t edge_b,
                                   uint16_t screen_size) {
    uint32_t range;
    uint32_t value;

    if (screen_size == 0U) {
        return 0U;
    }

    if (edge_a == edge_b) {
        return 0U;
    }

    if (edge_b > edge_a) {
        if (raw <= edge_a) {
            return 0U;
        }
        if (raw >= edge_b) {
            return (uint16_t)(screen_size - 1U);
        }
        range = (uint32_t)(edge_b - edge_a);
        value = ((uint32_t)(raw - edge_a) * (uint32_t)(screen_size - 1U)) / range;
    } else {
        if (raw >= edge_a) {
            return 0U;
        }
        if (raw <= edge_b) {
            return (uint16_t)(screen_size - 1U);
        }
        range = (uint32_t)(edge_a - edge_b);
        value = ((uint32_t)(edge_a - raw) * (uint32_t)(screen_size - 1U)) / range;
    }

    return (uint16_t)value;
}

static void touch_cal_build_map(void) {
    uint16_t horizontal_dx = touch_cal_abs_diff(cal_points[1].x_avg, cal_points[0].x_avg) +
                             touch_cal_abs_diff(cal_points[2].x_avg, cal_points[3].x_avg);
    uint16_t horizontal_dy = touch_cal_abs_diff(cal_points[1].y_avg, cal_points[0].y_avg) +
                             touch_cal_abs_diff(cal_points[2].y_avg, cal_points[3].y_avg);

    cal_map.swap_xy = (horizontal_dy > horizontal_dx) ? 1U : 0U;

    if (cal_map.swap_xy) {
        cal_map.x_min = touch_cal_average2(cal_points[0].y_avg, cal_points[3].y_avg);
        cal_map.x_max = touch_cal_average2(cal_points[1].y_avg, cal_points[2].y_avg);
        cal_map.y_min = touch_cal_average2(cal_points[0].x_avg, cal_points[1].x_avg);
        cal_map.y_max = touch_cal_average2(cal_points[3].x_avg, cal_points[2].x_avg);
    } else {
        cal_map.x_min = touch_cal_average2(cal_points[0].x_avg, cal_points[3].x_avg);
        cal_map.x_max = touch_cal_average2(cal_points[1].x_avg, cal_points[2].x_avg);
        cal_map.y_min = touch_cal_average2(cal_points[0].y_avg, cal_points[1].y_avg);
        cal_map.y_max = touch_cal_average2(cal_points[3].y_avg, cal_points[2].y_avg);
    }

    cal_map.mirror_x = (cal_map.x_min > cal_map.x_max) ? 1U : 0U;
    cal_map.mirror_y = (cal_map.y_min > cal_map.y_max) ? 1U : 0U;
    cal_map.ready = 1U;
}

static void touch_cal_map_point(const TOUCH_POINT *raw_point, uint16_t *screen_x,
                                uint16_t *screen_y) {
    uint16_t raw_x;
    uint16_t raw_y;

    if ((raw_point == 0) || (screen_x == 0) || (screen_y == 0) || !cal_map.ready) {
        return;
    }

    raw_x = cal_map.swap_xy ? raw_point->y : raw_point->x;
    raw_y = cal_map.swap_xy ? raw_point->x : raw_point->y;

    *screen_x = touch_cal_map_axis(raw_x, cal_map.x_min, cal_map.x_max, TOUCH_CAL_PANEL_WIDTH);
    *screen_y = touch_cal_map_axis(raw_y, cal_map.y_min, cal_map.y_max, TOUCH_CAL_PANEL_HEIGHT);
}

static void touch_cal_live_monitor(void) {
    TOUCH_POINT point;
    uint16_t screen_x = 0U;
    uint16_t screen_y = 0U;

    TOUCH_CAL_PRINT("\r\nEntering live monitor. Touch panel to print raw and calibrated point.\r\n");
    while (1) {
        if (touch_read_point(&point) == TOUCH_OK) {
            if ((point.event != TOUCH_EVENT_NONE) || point.pressed) {
                touch_cal_map_point(&point, &screen_x, &screen_y);
                TOUCH_CAL_PRINT("touch: pressed=%u points=%u raw=(%u,%u) screen=(%u,%u) event=%s\r\n",
                                point.pressed,
                                point.point_num,
                                point.x,
                                point.y,
                                screen_x,
                                screen_y,
                                touch_event_name(point.event));
            }
        } else {
            TOUCH_CAL_PRINT("touch_read_point error\r\n");
        }

        delay_ms(TOUCH_CAL_POLL_DELAY_MS);
    }
}

void cst816_calibration_test(void) {
    uint8_t ret;

    TOUCH_CAL_PRINT("\r\n=== CST816 calibration test ===\r\n");
    TOUCH_CAL_PRINT("RTT channel: terminal=%u channel=%u\r\n",
                    (uint16_t)TOUCH_CAL_RTT_TERMINAL_ID,
                    (uint16_t)TOUCH_CAL_RTT_CHANNEL_INDEX);

    ret = cst816_register(NULL, mcu_touch_iic_get_ops(),
                          TOUCH_CAL_RAW_WIDTH, TOUCH_CAL_RAW_HEIGHT);
    if (ret != TOUCH_OK) {
        TOUCH_CAL_PRINT("cst816_register failed: %u\r\n", ret);
        while (1) {
        }
    }

    ret = touch_init();
    if (ret != TOUCH_OK) {
        TOUCH_CAL_PRINT("touch_init failed: %u\r\n", ret);
        while (1) {
        }
    }

    touch_cal_print_context();

    for (uint16_t i = 0U; i < (sizeof(cal_points) / sizeof(cal_points[0])); i++) {
        touch_cal_capture_point(&cal_points[i]);
    }

    touch_cal_build_map();
    touch_cal_print_result();
    touch_cal_live_monitor();
}

#endif
