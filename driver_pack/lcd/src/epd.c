/*
 * @file         : epd.c
 * @Author       : shuyu
 * @LastEditTime : 2025-08-17 13:46
 * @Description  : 
 */

#include "epd.h"

static BASE_SPI epd;

/**
 * @function:   epdfn_init
 * @brief:      函数注册
 * @param{NULL}:  NULL
 * @retval:     NULL
 */
static void epdfn_init(void) {
    lcd_fn_register(&epd);
}

/**
 * @funciton: read_epd_busy
 * @brief:      读取是否处于忙等状态
 * @param{NULL}:    NULL
 * @retval:     NULL
 */
static void read_epd_busy(void) {
    while (epd.read_busy());
}

/**
 * @fuunciton:  epd_reset
 * @brief:      硬件复位
 * @retval:     NULL
 */
static void epd_reset(void) {
    DELAY_MS(100);
    epd.set_reset(0);
    DELAY_MS(10);
    epd.set_reset(1);
    DELAY_MS(10);
    read_epd_busy();
}

#if !FAST_ENABLE
/**
 * @funciton:       epd_init
 * @brief:          epd的初始化函数
 * @param{NULL}:      NULL
 * @retval:         NULL
 */
static void epd_init(void) {
    epdfn_init();
    epd.dev->spi_init();
    epd_reset();
    read_epd_busy();
    epd.write_cmd(RESET_CMD);
    read_epd_busy();
}
#endif

/**
 * @fuction:    epd_display
 * @brief:      将需要显示的数据搬运到LCD内部的RAM中
 * @param{NULL}:  const uint8_t* disp_buf
 * @retval:     NULL
 */
static void epd_display(const uint8_t *disp_buf) {
    uint16_t i, j, height, width;

    width = (LCD_WIDTH % 8 == 0) ? (LCD_WIDTH / 8) : (LCD_WIDTH / 8 + 1);
    height = LCD_HEIGHT;

    epd.write_cmd(WRITE_RAM);
    for (j = 0; j < height; j++) {
        for (i = 0; i < width; i++) { epd.write_data(disp_buf[i + j * width]); }
    }
}

#if (!FAST_ENABLE && !PART_ENABLE)
/**
 * @function:       epd_update
 * @brief:          更新
 * @param{NULL}:    NULL
 * @retval:         NULL 
 */
static void epd_update(void) {
    epd.write_cmd(UP_MODE_2);
    epd.write_data(0xF4);
    epd.write_cmd(MASTER_Activation);
    read_epd_busy();
}
#endif

#if (PART_ENABLE && !FAST_ENABLE)
/**
 * @funciton:       epd_part_update
 * @brief:          局部更新
 * @param{NULL}:    NULL
 * @retval:         NULL
 */
static void epd_part_update(void) {
    epd.write_cmd(UP_MODE_2);
    epd.write_data(0xDC);
    epd.write_cmd(MASTER_Activation);
    read_epd_busy();
}
#endif

#if FAST_ENABLE
/**
 * @funciton:       epd_fast_update
 * @brief:          快速更新
 * @param{NULL}:    NULL
 * @retval:         NULL
 */
static void epd_fast_update(void) {
    epd.write_cmd(UP_MODE_2);
    epd.write_data(0xC7);
    epd.write_cmd(MASTER_Activation);
    read_epd_busy();
}
#endif

/**
 * @funciton:       epd_sleep
 * @breif:          睡眠模式
 * @param{NULL}:    NULL
 * @retval:         NULL
 */
static void epd_sleep(void) {
    epd.write_cmd(SLEEP_CMD);
    epd.write_data(0xC7);
    DELAY_MS(200);
}

#if (FAST_ENABLE && defined(MODE1))
/**
 * @function:   epd_fastmode1_init
 * @breif:      快速刷新模式1初始化
 * @param{NULL}: NULL
 * @retval:     NULL
 */
static void epd_fastmode1_init(void) {
    epdfn_init();
    epd.dev->spi_init();

    epd_reset();
    read_epd_busy();
    epd.write_cmd(0x12);
    read_epd_busy();
    epd.write_cmd(0x18);
    epd.write_data(0x80);
    epd.write_cmd(0x22);
    epd.write_data(0xB1);
    epd.write_cmd(0x20);
    read_epd_busy();
    epd.write_cmd(0x1A);
    epd.write_data(0x64);
    epd.write_data(0x00);
    epd.write_cmd(0x22);
    epd.write_data(0x91);
    epd.write_cmd(0x20);
    read_epd_busy();
}
#endif

#if (FAST_ENABLE && defined(MODE2))
/**
 * @funciton:   epd_fastmode2_init
 * @breif:      快速刷新模式2初始化
 * @param{NULL}: NULL
 * @retval:     NULL    
 */
static void epd_fastmode2_init(void) {
    epdfn_init();
    epd.dev->spi_init();

    epd_reset();
    read_epd_busy();
    epd.write_cmd(0x12);
    read_epd_busy();
    epd.write_cmd(0x18);
    epd.write_data(0x80);
    epd.write_cmd(0x22);
    epd.write_data(0xB1);
    epd.write_cmd(0x20);
    read_epd_busy();
    epd.write_cmd(0x1A);
    epd.write_data(0x5A);
    epd.write_data(0x00);
    epd.write_cmd(0x22);
    epd.write_data(0x91);
    epd.write_cmd(0x20);
    read_epd_busy();
}
#endif

#if !PART_ENABLE
/**
 * @function:       epd_disp_clear
 * @brief:          全局擦屏函数
 * @param{NULL}:    NULL
 * @retval:         NULL
 */
static void epd_disp_clear(void) {
    uint16_t i;
    epd.write_cmd(0x3C);
    epd.write_data(0x05);
    epd.write_cmd(0x24);
    for (i = 0; i < 2888; i++) { epd.write_data(0xff); }

    read_epd_busy();
    epd.write_cmd(0x26);

    for (i = 0; i < 2888; i++) { epd.write_data(0x00); }
}
#endif

#if PART_ENABLE
/**
 * @function:       epd_disp_clear
 * @brief:          局部擦屏函数
 * @param{NULL}:    NULL
 * @retval:         需要在局部刷新模式下使用
 */
static void epd_disp_part_clear(uint16_t color) {
    uint16_t i;
    read_epd_busy();
    epd.write_cmd(0x26);
    for (i = 0; i < 2888; i++) { epd.write_data(0xFF); }
}
#endif

/**
 * @function:       ssd1680_driver_init_callback
 * @brief:          提供外部回调初始化
 */

void ssd1680_driver_init_callback(LCD_DRIVER *lcd_driver) {
    lcd_driver->disp = epd_display;
#if FAST_ENABLE
    lcd_driver->update = epd_fast_update;
#ifdef MODE1
    lcd_driver->ui_init = epd_fastmode1_init;
#endif
#ifdef MODE2
    lcd_driver->ui_init = epd_fastmode2_init;
#endif
#else
    lcd_driver->ui_init = epd_init;
#endif

#if (PART_ENABLE && !FAST_ENABLE)
    lcd_driver->update = epd_part_update;
#elif (!FAST_ENABLE)
    lcd_driver->update = epd_update;
#endif

#if PART_ENABLE
    lcd_driver->clear = epd_disp_part_clear;
#else
    lcd_driver->clear = epd_disp_clear;
#endif

    lcd_driver->sleep = epd_sleep;

    lcd_driver->dev_type = DEV_EPD;
}
