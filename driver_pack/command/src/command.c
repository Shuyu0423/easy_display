/*
 * @file         : command.c
 * @Author       : shuyu
 * @LastEditTime : 2025-08-09 20:57
 * @Description  : 操作命令，需要添加新的指令在此添加声明
 */

#include "command.h"

/***************************command funciton***********************/
REGISTER_COMMAND_FUNCTION(version) {
    LOG(INFO,RED,"version:%s",get_sc_log_version());
}

REGISTER_COMMAND_FUNCTION(hello) {
    LOG(INFO,RED,"hello world");
}

/*
REGISTER_COMMAND_FUNCTION(checkimg) {
    static uint8_t pic;
    SPI_SET_RESET(1);
    SPI_SET_RESET(0);
	ui_init();
    ui_buf_clear(WHITE);        
    ui_clear(0xFF);
    pic++;
    if(pic & 1) {
        ui_show_picture(0,0,152,152,gImage_1,BLACK);
    }else{
        ui_show_picture(0,0,152,152,gImage_2,BLACK);
    }
    
    
}
*/

/***************************command define***********************/
REGISTER_COMMAND(SC_VERSION,version);
REGISTER_COMMAND(SC_HELLO,hello);
//REGISTER_COMMAND(SC_CHECKIMG,checkimg);

extern COMMAND_ENTRY command_section$$Base[];
extern COMMAND_ENTRY command_section$$Limit[];

/**TOP_API*/
void process_command(const char* input) {
	// 直接使用符号地址
    const COMMAND_ENTRY* start = command_section$$Base;
    const COMMAND_ENTRY* end = command_section$$Limit;
	const COMMAND_ENTRY* cmd;
    for (cmd = start; cmd < end; cmd++){
        // FIXME: @sy
        if (strcmp(input, cmd->command_name) == 0) {
            cmd->command_fn(); // 执行命令
            return;
        }
    }
 	return ;
}

