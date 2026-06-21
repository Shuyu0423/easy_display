/*
 * @Author       : SuperYu 824229900@qq.com
 * @Date         : 2025-07-06 15:51
 * @LastEditors  : Please set LastEditors
 * @LastEditTime : 2025-08-17 21:58
 * @Description  : 
 */
#include "board.h"
#include <stdio.h>

#include "user_conf.h"

#include "sc_log.h"
#include "lcd_ui.h"
#include "pic.h"

UART_HANDLER usart_handler;

int main(void) {
    board_init();
    uart1_init(115200U);
    tick_timer_init();
    buffer_init(&usart_handler);
    ui_init();

    //ui_show_string(30, 50, "hello world", 16, BLACK);
    ui_show_picture(0, 0, 150, 150, gImage_2, NULL);
#if 1
    uint8_t temp_buf[RING_BUFFER_SIZE];
    uint8_t cmp_buf[RING_BUFFER_SIZE];
#endif

    while (1) {

/*用于测试自定义段数据是否能够正常处理*/
#if 1
        uint16_t num = usart_handler.get_data(temp_buf, sizeof(temp_buf));
        memset(cmp_buf, 0, RING_BUFFER_SIZE);
        c_memcpy(cmp_buf, temp_buf, num);
        if (num > 0) {
            process_command(cmp_buf);
        }
#endif
    }
}
