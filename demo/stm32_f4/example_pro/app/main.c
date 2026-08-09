#include "board.h"
#include <stdio.h>

#include "user_conf.h"

#include "sc_log.h"
#include "lcd_ui.h"
#include "pic.h"
#include "command.h"

#if TOUCH_CST816_CALIBRATION_TEST_ENABLE
#include "../../../../tests/target/touch/cst816_calibration_test.h"
#endif
#if ILI9341_XPT2046_TEST_ENABLE
#include "../../../../tests/target/touch/ili9341_xpt2046_test.h"
#endif

UART_HANDLER usart_handler;

int main(void) {
    board_init();
    uart1_init(115200U);
    tick_timer_init();

#if TOUCH_CST816_CALIBRATION_TEST_ENABLE
    cst816_calibration_test();
#endif
#if ILI9341_XPT2046_TEST_ENABLE
    ili9341_xpt2046_test();
#endif

    buffer_init(&usart_handler);
    ui_init();

    //ui_show_string(30, 50, "hello world", 16, BLACK);
    ui_show_picture(0, 0, 150, 150, gImage_2, BLACK);
#if 1
    uint8_t temp_buf[RING_BUFFER_SIZE];
    char cmp_buf[RING_BUFFER_SIZE];
#endif

    while (1) {

/*用于测试自定义段数据是否能够正常处理*/
#if 1
        uint16_t num = usart_handler.get_data(temp_buf, sizeof(temp_buf));
        c_memset(cmp_buf, 0, RING_BUFFER_SIZE);
        c_memcpy(cmp_buf, temp_buf, num);
        if (num > 0) {
            process_command(cmp_buf);
        }
#endif
    }
}
