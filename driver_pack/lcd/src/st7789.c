/*
 * @file         : 
 * @Author       : shuyu
 * @LastEditTime : 2025-08-18 00:22
 * @Description  : 
 */

#include "st7789.h"
#include <stddef.h>

static BASE_SPI st7789;

/**
 * @funciton:       st7789_base_init
 * @breif:          st7789的spi初始化注册
 * @param{NULL}     NULL
 * @retval:         NULL
 */
static void st7789_base_init(void) {
    lcd_fn_register(&st7789);
}

/**
 * @funciton:       st7789_set_window
 * @breif:          设置窗口
 * @note：         
 * @retval:         NULL
 */
// TODO：暂时只支持屏幕正常显示，后续需要调整旋转
void st7789_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    st7789.write_cmd(0x2A); // 列地址设置
    st7789.write_data(x1 >> 8);
    st7789.write_data(x1 & 0xFF);
    st7789.write_data(x2 >> 8);
    st7789.write_data(x2 & 0xFF);

    st7789.write_cmd(0x2B); // 行地址设置
    st7789.write_data(y1 >> 8);
    st7789.write_data(y1 & 0xFF);
    st7789.write_data(y2 >> 8);
    st7789.write_data(y2 & 0xFF);

    st7789.write_cmd(0x2C); // 写入RAM
}

/**
 * @funciton:       st7789_driver_init
 * @breif:          st7789驱动初始化，发送初始化指令
 * 
 */
static void st7789_driver_init(void) {

    // 复位屏幕
    st7789.set_reset(0);
    DELAY_MS(120); // 通常120ms就足够了
    st7789.set_reset(1);
    DELAY_MS(120);

    /** 点亮背光 */

#if NEED_SPIPORT_BL
    SPI_SET_BLE(1);
#endif

    st7789.write_cmd(0x11); //Sleep out
    DELAY_MS(20);

    /* 控制颜色格式*/
    st7789.write_cmd(0x3A);
    st7789.write_data(0x05);

    //************* Start Initial Sequence **********//

    /** 控制方向 */
    st7789.write_cmd(0x36);
    switch (SET_SCREEN_ORIENTATION) {
        case 0:
            st7789.write_data(0x00);
            break;
        case 1:
            st7789.write_data(0xC0);
            break;
        case 2:
            st7789.write_data(0x70);
            break;
        case 3:
            st7789.write_data(0xA0);
            break;
        default:
            break;
    }

    st7789.write_cmd(0x2A); // 列地址设置
    st7789.write_data(0 >> 8);
    st7789.write_data(0 & 0xFF);
    st7789.write_data(LCD_WIDTH >> 8); // 使用配置的宽度
    st7789.write_data(LCD_WIDTH & 0xFF);

    st7789.write_cmd(0x2B); // 行地址设置
    st7789.write_data(0 >> 8);
    st7789.write_data(0 & 0xFF);
    st7789.write_data(LCD_HEIGHT >> 8); // 使用配置的高度
    st7789.write_data(LCD_HEIGHT & 0xFF);

    /* 帧率控制 */
    st7789.write_cmd(0xB2);
    st7789.write_data(0x05);
    st7789.write_data(0x05);
    st7789.write_data(0x00);
    st7789.write_data(0x33);
    st7789.write_data(0x33);

    /* 电源控制*/
    st7789.write_cmd(0xB7);
    st7789.write_data(0x05);

    /* 电量控制*/
    st7789.write_cmd(0xBB);
    st7789.write_data(0x3F);

    /* 电源控制 */
    st7789.write_cmd(0xC0);
    st7789.write_data(0x2C);

    /* VDV VRH*/
    st7789.write_cmd(0xC2);
    st7789.write_data(0x01);

    /* VRH */
    st7789.write_cmd(0xC3);
    st7789.write_data(0x0F);

    /* VDV*/
    st7789.write_cmd(0xC4);
    st7789.write_data(0x20);

    /* 帧率控制*/
    st7789.write_cmd(0xC6);
    st7789.write_data(0x01);

    /* 电源控制*/
    st7789.write_cmd(0xD0);
    st7789.write_data(0xA4);
    st7789.write_data(0xA1);

    st7789.write_cmd(0xE8); //Power Control 1
    st7789.write_data(0x03);

    st7789.write_cmd(0xE9); //Equalize time control
    st7789.write_data(0x09);
    st7789.write_data(0x09);
    st7789.write_data(0x08);

    st7789.write_cmd(0xE0); //Set Gamma
    st7789.write_data(0xD0);
    st7789.write_data(0x05);
    st7789.write_data(0x09);
    st7789.write_data(0x09);
    st7789.write_data(0x08);
    st7789.write_data(0x14);
    st7789.write_data(0x28);
    st7789.write_data(0x33);
    st7789.write_data(0x3F);
    st7789.write_data(0x07);
    st7789.write_data(0x13);
    st7789.write_data(0x14);
    st7789.write_data(0x28);
    st7789.write_data(0x30);

    st7789.write_cmd(0XE1); //Set Gamma
    st7789.write_data(0xD0);
    st7789.write_data(0x05);
    st7789.write_data(0x09);
    st7789.write_data(0x09);
    st7789.write_data(0x08);
    st7789.write_data(0x03);
    st7789.write_data(0x24);
    st7789.write_data(0x32);
    st7789.write_data(0x32);
    st7789.write_data(0x3B);
    st7789.write_data(0x14);
    st7789.write_data(0x13);
    st7789.write_data(0x28);
    st7789.write_data(0x2F);

    st7789.write_cmd(0x21); //反显

    st7789.write_cmd(0x29); //开启显示
}

/**
 * @funciton:       st7789_init
 * @breif:          st7789初始化
 * @param{NULL}:    NULL
 * @retval:         NULL
 */

static void st7789_init(void) {
    st7789_base_init();
    st7789.dev->spi_init();
    // 初始化驱动
    st7789_driver_init();
}

/**
 * @funciton：  st7789_set_pixel
 */
/**
 * @function:    st7789_set_pixel
 * @breif:       Draw one RGB565 pixel on ST7789.
 * @param x:     X coordinate.
 * @param y:     Y coordinate.
 * @param color: RGB565 color.
 * @retval:      NULL
 */
static void st7789_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    st7789_set_window(x, y, x, y);
    st7789.write_data(color >> 8);
    st7789.write_data(color);
}

/**
 * @function:          st7789_write_pixels
 * @breif:             Write continuous RGB565 pixels to the current ST7789 window.
 * @param pixels:      RGB565 pixel data pointer, high byte first.
 * @param pixel_count: Pixel count.
 * @retval:            NULL
 */
static void st7789_write_pixels(const uint8_t *pixels, uint32_t pixel_count) {
    if (st7789.write_data_buf) {
        st7789.write_data_buf(pixels, pixel_count * 2U);
    }
}

/**
 * @funiton:            st7789_clear
 * @breif:              清屏
 */
/**
 * @funiton:            st7789_clear
 * @breif:              清屏
 */
static void st7789_clear(uint16_t color) {
    uint32_t total_pixels = LCD_WIDTH * LCD_HEIGHT;

    // 设置窗口（使用配置的尺寸）
    st7789_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    // 拆分颜色为高8位和低8位
    uint8_t color_h = color >> 8;
    uint8_t color_l = color & 0xFF;

    // 发送颜色数据（注意：write_data已经包含了CS和DC控制）
    for (uint32_t i = 0; i < total_pixels; i++) {
        st7789.write_data(color_h); // 高8位
        st7789.write_data(color_l); // 低8位
    }
}
static void st7789_display(const uint8_t *buf) {
    //TODO：使用DMA进行传输
}

// LCD睡眠函数
static void st7789_sleep(void) {
    st7789.write_cmd(0x10); // 进入睡眠模式
    DELAY_MS(5);
}

/**
  * @funciton:      st7789_driver_init_callback
  * @breif:         提供外部回调初始化
  */
void st7789_driver_init_callback(LCD_DRIVER *lcd_driver) {
    lcd_driver->ui_init = st7789_init;
    lcd_driver->disp = st7789_display;
    lcd_driver->update = NULL; // LCD不需要更新操作
    lcd_driver->clear = st7789_clear;
    lcd_driver->sleep = st7789_sleep;
    lcd_driver->set_pixel = st7789_set_pixel;
    lcd_driver->set_window = st7789_set_window;
    lcd_driver->write_pixels = st7789_write_pixels;
    lcd_driver->dev_type = DEV_LCD;
}
