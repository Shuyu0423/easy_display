#ifndef LCD_DRIVER_H__
#define LCD_DRIVER_H__

#include "lcd_conf.h"

typedef enum { DEV_EPD, DEV_LCD } DEVICE_TYPE;

typedef struct {
    void (*spi_init)(void);
    uint8_t (*spi_transbyte)(uint8_t);
    void (*set_cs)(uint8_t);
    void (*set_dc)(uint8_t);
} SPI_DEV;

typedef struct {
    void (*ui_init)(void);
    void (*disp)(const uint8_t *);
    void (*update)(void);
    void (*clear)(uint16_t);
    void (*sleep)(void);
    DEVICE_TYPE dev_type;
    void (*set_pixel)(uint16_t, uint16_t, uint16_t);
    void (*set_window)(uint16_t, uint16_t, uint16_t, uint16_t);
    void (*write_pixels)(const uint8_t *, uint32_t);
} LCD_DRIVER;

typedef struct {
    SPI_DEV *dev;
    void (*write_cmd)(uint8_t);
    void (*write_data)(uint8_t);
    void (*write_data_buf)(const uint8_t *, uint32_t);
    uint8_t (*read_busy)(void);
    void (*set_reset)(uint8_t);
} BASE_SPI;

void lcd_fn_register(BASE_SPI *lcd_dev);

#endif
