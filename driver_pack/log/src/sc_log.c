/**
 * @file: sc_log.c
 * @breif:  日志库文件
 * @info: v1.0 2025-7-6 @sy
 *         fifo缓冲区、提供底层注册函数
 */

#include "sc_log.h"
#include "c_unit.h"
#include <stdio.h>

const char* sc_log_version = "01-250918";

const char* get_sc_log_version(void) {
/*
	char ret_version[9];
	uint8_t i = 0;
	for(i = 0; i< 9; i++) {
		ret_version[i] = sc_log_version[i];
	}
	return (char *)ret_version;
*/
	return sc_log_version;
}

#if RING_BUFFER_ENABLE
/************************************环形缓冲区*******************************/
RINGBUFFER_DECLARE(RING_BUFFER);

/**
 * @function:   void ring_buffer_init(void)
 * @breif:      环形缓冲区初始化
 */
static void ring_buffer_init(void) {
    RING_BUFFER.rec_data_len = 0;
    RING_BUFFER.tail = 0;
    RING_BUFFER.front = 0;

    memset(RING_BUFFER.ring_buffer,0,sizeof(RING_BUFFER.ring_buffer));
}


/**
 * @function: uint8_t is_empty(void)
 * @breif:    环形缓冲区是否为空
 */
static uint8_t is_empty(void) {
    return (RING_BUFFER.front == RING_BUFFER.tail) ? 1 :0;
 }

  /**
  * @funciton: uint8_t pop_buffer(uint8_t* data)
  * @breif:     出队
  */

static uint8_t pop_buffer(uint8_t* data) {
    if (RING_BUFFER.front == RING_BUFFER.tail) {
        return BUFFER_ERROR;
    }
    *data = RING_BUFFER.ring_buffer[RING_BUFFER.tail];
    RING_BUFFER.tail = (RING_BUFFER.tail + 1) % RING_BUFFER_SIZE;
    return BUFFER_SUCCESS;
}


#if PUSH_CHAR
 /**
  * @function: uint8_t push_buffer(uint8_t data)
  * @brief  :  入队
  */
static uint8_t push_buffer(uint8_t data) {
    if ((RING_BUFFER.front + 1) % RING_BUFFER_SIZE == RING_BUFFER.tail) {
        return BUFFER_ERROR;
    }
    RING_BUFFER.ring_buffer[RING_BUFFER.front] = data;
    RING_BUFFER.front = (RING_BUFFER.front + 1) % RING_BUFFER_SIZE;
    return BUFFER_SUCCESS;
 }



/**
 * @funciton:   uint8_t push_one_char(uint8_t data)
 * @breif:      输入一个字节的数据
 * @param[in]： uint8_t data
 * @retval:     0-error 1-success  
 * @notes:      默认支持单字节写入
 */
static uint8_t push_one_char(uint8_t data) {
    return push_buffer(data);
}
#endif

/**
 * @funciton:   uint8_t pop_one_char(uint8_t* data)
 * @breif:      输出一个字节的数据
 * @param[in]:  NULL
 * @retval:     0-error 1-success
 * @note:       调试功能默认不开启
 */
uint8_t pop_one_char(uint8_t* data) {
    if(is_empty()){
        return BUFFER_ERROR;
    }
    
    return pop_buffer(data);
}

/**
 * @fuction:    uint8_t push_string(uint8_t* data,uint16_t len)
 * @breif:      输入多字节数据
 * @param[in]:  uint8_t* data  输入的数组
 * @param[in]:  uint16_t len    输入的数组长度
 * @retval:     0-error 1-success
 * @note:       调试功能默认不开启
 */
uint8_t push_string(uint8_t* data, uint16_t len) {
    // 检查可用空间
    uint16_t free_space = (RING_BUFFER.tail - RING_BUFFER.front - 1 + RING_BUFFER_SIZE) % RING_BUFFER_SIZE;
    if (len > free_space) {
        return BUFFER_ERROR; // 空间不足
    }

    // 高效拷贝（考虑回绕）
    if (RING_BUFFER.front + len < RING_BUFFER_SIZE) {
        c_memcpy(&RING_BUFFER.ring_buffer[RING_BUFFER.front], data, len);
        RING_BUFFER.front = (RING_BUFFER.front + len) % RING_BUFFER_SIZE;
    } else {
        uint16_t first_segment = RING_BUFFER_SIZE - RING_BUFFER.front;
        c_memcpy(&RING_BUFFER.ring_buffer[RING_BUFFER.front], data, first_segment);
        c_memcpy(RING_BUFFER.ring_buffer, data + first_segment, len - first_segment);
        RING_BUFFER.front = len - first_segment;
    }
    return BUFFER_SUCCESS;
}

/**
 * @funciton:       get_data
 * @brief:          获取环形缓冲区的数据
 * @param{dest}:    接收的缓冲区
 * @param{max_len}: 需要读取的数据长度
 * @note：          真正读取的数据长度取决于两个指针之间的差值
 */
static uint16_t get_data(uint8_t* dest, uint16_t max_len) {
    uint16_t bytes_available = 0;
    uint16_t bytes_to_copy = 0;
    uint16_t read_valnum = 0;
    
    // 计算可用数据量
    if(RING_BUFFER.front >= RING_BUFFER.tail) {
        bytes_available = RING_BUFFER.front - RING_BUFFER.tail;
    } else {
        bytes_available = RING_BUFFER_SIZE - RING_BUFFER.tail + RING_BUFFER.front;
    }
    
    
    // 确定实际可读取的数据量
    bytes_to_copy = (bytes_available > max_len) ? max_len : bytes_available;

    while(bytes_to_copy--) {
        if(pop_one_char(dest) == BUFFER_SUCCESS) {
            dest++;
            read_valnum++;
        } else {
            break;  
        }
    }

    return read_valnum;
}


/**提供顶部的注册接口设计*/
/**回调函数内容 */
void buffer_init(UART_HANDLER* uart_handler) {
    uart_handler->init = ring_buffer_init;
#if PUSH_CHAR
    uart_handler->push = push_one_char;
#elif PUSH_STRING
    uart_handler->push = push_string;
#endif
	uart_handler->get_data = get_data;
}

#endif
/***********************日志函数实现************************/

#ifdef UART_DEBUG_ENABLE

/**
 * @function: void send_string(const char* str)
 * @brief:      发送字符串
 * @note:      NULL
 */
static void send_string(const char* str) {
    while(*str) {
        SEND_CHAR(*str++);
    }
}

/**
 * @function:   void itoa(int32_t value, char* str, int32_t base)
 * @breif:      将整数转换成字符串
 */
static void itoa(int32_t value, char* str, int32_t base) {
    char* ptr = str;
    char* start = str;
    char temp;
    int sign = 0;
    
    // 处理负数（仅十进制）
    if (base == 10 && value < 0) {
        sign = 1;
        value = -value;  // 转为正数
    }
    
    // 处理0的特殊情况
    if (value == 0) {
        *ptr++ = '0';
        *ptr = '\0';
        return;
    }
    
    // 转换数字
    uint32_t abs_value = (uint32_t)value;  // 安全转换
    do {
        uint32_t digit = abs_value % base;
        *ptr++ = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        abs_value /= base;
    } while (abs_value > 0);
    
    // 添加负号
    if (sign) {
        *ptr++ = '-';
    }
    
    *ptr-- = '\0';
    
    // 反转字符串
    while (start < ptr) {
        temp = *start;
        *start++ = *ptr;
        *ptr-- = temp;
    }
}

/**
 * @funticon:  void utoa_hex(uint32_t value, char* str, int32_t pad)
 * @brief:      将无符号整数转换为十六进制字符串
 */
static void utoa_hex(uint32_t value, char* str, int32_t pad) {
    char* ptr = str;
    char* start = str;
    char temp;
    int32_t digit_count = 0;
    
    do {
        int32_t digit = value & 0xF;
        *ptr++ = (digit < 10) ? '0' + digit : 'a' + digit - 10;
        value >>= 4;
        digit_count++;
    } while (value > 0);
    
    // 添加前导零以满足填充要求
    while (digit_count < pad) {
        *ptr++ = '0';
        digit_count++;
    }
    
    *ptr-- = '\0';
    
    // 反转字符串
    while (start < ptr) {
        temp = *start;
        *start++ = *ptr;
        *ptr-- = temp;
    }
}

/**
 * @funciton:   void log_printf(uint8_t level, const char* format, ...)
 * @breif:      日志打印
 * @param[in]:  LogLevel level  日志等级
 * @param[in]:  const char* format      打印内容
 * @retval:     NULL
 */
void log_printf(uint8_t level, const char* format, ...) {
    static char buffer[SUPPORT_DEBUG_LEN]; // 静态缓冲区
    char* ptr = buffer;
    char num_buffer[20];
    va_list args;
    
    // 添加日志级别前缀
    switch (level) {
        case INFO:  strcpy(ptr, "[INFO]  "); break;
        case WARN:  strcpy(ptr, "[WARN]  "); break;
        case ERR:   strcpy(ptr, "[ERROR] "); break;
        default:    strcpy(ptr, "[DEBUG] "); break;
    }
    
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++;
            int pad = 0;
            
            if (*format >= '0' && *format <= '9') {
                pad = *format - '0';
                format++;
            }
			
			size_t remaining = sizeof(buffer) - (ptr - buffer) - 1;
            
            switch (*format) {
                case 'd': {
                    int32_t num = va_arg(args, int32_t);
                    itoa(num, num_buffer, 10);
                    size_t len = strlen(num_buffer);
                    size_t copy_len = len < remaining ? len : remaining;
                    strncpy(ptr, num_buffer, copy_len);
                    ptr += copy_len;
                    break;
                }
                case 'u': {
                    uint32_t num = va_arg(args, uint32_t);
                    itoa(num, num_buffer, 10);
                    strcpy(ptr, num_buffer);
                    ptr += strlen(num_buffer);
                    break;
                }
                case 'x': {
                    uint32_t num = va_arg(args, uint32_t);
                    utoa_hex(num, num_buffer, pad);
                    strcpy(ptr, "0x");
                    ptr += 2;
                    strcpy(ptr, num_buffer);
                    ptr += strlen(num_buffer);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int32_t);
                    *ptr++ = c;
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    strcpy(ptr, str);
                    ptr += strlen(str);
                    break;
                }
                case '%': {
                    *ptr++ = '%';
                    break;
                }
                default: {
                     if (remaining > 0) {
                        *ptr++ = '?';
                    }
                    break;
                }
            }
        } else {
            *ptr++ = *format;
        }
        
        if (ptr - buffer >= sizeof(buffer) - 1) {
            // 防止缓冲区溢出
            break;
        }
        
        format++;
    }
    
    va_end(args);
    
    *ptr = '\0'; // 确保字符串以空字符结尾
    
    // 发送格式化后的字符串到串口
    send_string(buffer);
}

#endif

/***********************FLASH日志实现**********************/
#ifdef FLASH_DEBUG_ENABLE
//COMPONENT_INIT(CTRL,ctrl_info)                          // 初始化控制参数
COMPONENT_INIT(HEAD,log_head_ctrl)
static FLASH_MANEGER info_manager,err_manager,warn_manager;

static void component_init(void) {
	
    log_head_ctrl.ops.flash_read = FLASH_READ_FUNC;
    log_head_ctrl.ops.flash_write = FLASH_WRITE_FUNC;
    log_head_ctrl.ops.flash_earse = FLASH_ERASE_FUNC;
	log_head_ctrl.info_log_start_addr = FLASH_START_ADDR + sizeof(CTRL_INFO) + (0 * FLASH_SECTOR_SIZE);
	log_head_ctrl.err_log_start_addr = FLASH_START_ADDR + sizeof(CTRL_INFO) + (1 * FLASH_SECTOR_SIZE);
	log_head_ctrl.warn_log_start_addr = FLASH_START_ADDR + sizeof(CTRL_INFO) + (2 * FLASH_SECTOR_SIZE);
    
    info_manager.sector_size = FLASH_SECTOR_SIZE;
    info_manager.head_offset = 0;           
    info_manager.log_count   = 0;              
             
    err_manager.sector_size = FLASH_SECTOR_SIZE;
    err_manager.head_offset = 0;
    err_manager.log_count   = 0;
              
    warn_manager.sector_size = FLASH_SECTOR_SIZE;
    warn_manager.head_offset = 0;
    warn_manager.log_count   = 0;
    
}

/**
 * @funciton:   uint8_t flash_log_init(void)
 * @brief:      日志表初始化
 * @param{in}:  NULL
 * @retval:     状态
 */
uint8_t flash_log_init(void) {

    uint8_t ret = LOG_OK;
#if 0
	/*读取控制块查看是否有文件写入*/
    log_head_ctrl.ops.flash_read(FLASH_START_ADDR,(uint32_t*)&ctrl_info,sizeof(CTRL_INFO));
    /* 有数据写入则需要更新全局变量 */
    if(ctrl_info.log_info_flag == WRITE_FLAG ) {
		log_head_ctrl.ops.flash_read(log_head_ctrl.info_log_start_addr,(uint32_t*)&info_manager,sizeof(FLASH_MANEGER));
    }

    if(ctrl_info.log_err_flag == WRITE_FLAG ) {
		log_head_ctrl.ops.flash_read(log_head_ctrl.err_log_start_addr,(uint32_t*)&err_manager,sizeof(FLASH_MANEGER));
    }

    if(ctrl_info.log_warn_flag == WRITE_FLAG ) {
		log_head_ctrl.ops.flash_read(log_head_ctrl.warn_log_start_addr,(uint32_t*)&warn_manager,sizeof(FLASH_MANEGER));
    }
#else

    component_init();

#endif

	return ret;
}   


/**
 * @funciton:   uint8_t flash_log_write(uint8_t* p_data)
 * @breif:      日志写入的具体实现
 * @param{in}:  log_type    写入日志类型
 * @param{in}:  p_data      写入日志内容
 * @param{in}:  len         写入日志长度
 * @retval:     写入状态
 */
static uint8_t flash_log_write(uint8_t log_type,uint8_t* p_data,uint8_t len) {
    LOG_PACK log_pack;          // 写入的数据包
    uint32_t write_start_addr;  //  写入的地址
    static uint8_t info_count,err_count,warn_count;

    //uint8_t type_bit = 0;
    uint8_t ret = LOG_OK;

    log_pack.data_len = len;
    log_pack.timestamp = LOG_TICK_FUNC();
    c_memcpy(log_pack.data,p_data,len);

    switch (log_type)
    {
    case INFO :

#if 0
        /* 判断当前写入的地址在扇区范围大小之内 */
        if(info_manager.head_offset <= info_manager.sector_size) {

            type_bit = 1;       // 1

            info_manager.log_count++;
            log_pack.event_id = info_manager.log_count;
            write_start_addr = log_head_ctrl.info_log_start_addr  + sizeof(FLASH_MANEGER) + info_manager.head_offset;
            
			// FIXME: @sy   stm32写入是4字节写入，因此在写入算法的时候是默认一个字节的写入，因此里面会乘4，此时应该除以4
            log_head_ctrl.ops.flash_write(write_start_addr, (uint32_t*)&log_pack, sizeof(LOG_PACK) / 4);
            info_manager.head_offset += sizeof(LOG_PACK);
			info_manager.sector_size = FLASH_SECTOR_SIZE;
			/*更新mannager*/
			// FIXME: 没有意义，stm32写入需要擦除整个块			@sy
			/*
			log_head_ctrl.ops.flash_write(log_head_ctrl.info_log_start_addr, 
                                                       (uint32_t*)&info_manager, 
                                                       sizeof(FLASH_MANEGER) / 4);
			*/
        }else {
            ret = LOG_INFO_SIZE_FULL;
        }

        type_bit = 1;       // 1
#else 
        write_start_addr = log_head_ctrl.info_log_start_addr  + sizeof(FLASH_MANEGER) + (info_count++) * sizeof(LOG_PACK);
        log_pack.event_id = info_count;
        log_head_ctrl.ops.flash_write(write_start_addr, (uint32_t*)&log_pack, sizeof(LOG_PACK) / 4);


#endif
        
        break;

    case ERR:
#if 0
		if(err_manager.head_offset <= err_manager.sector_size) {
            
            type_bit = 2;       // 2
            
            err_manager.log_count++;
            log_pack.event_id = err_manager.log_count;
            write_start_addr = log_head_ctrl.err_log_start_addr  + sizeof(FLASH_MANEGER) + err_manager.head_offset;
            
            log_head_ctrl.ops.flash_write(write_start_addr, (uint32_t*)&log_pack, sizeof(LOG_PACK));
            err_manager.head_offset += sizeof(LOG_PACK);
			/*更新mannager*/
			log_head_ctrl.ops.flash_write(log_head_ctrl.err_log_start_addr, 
                                                       (uint32_t*)&err_manager, 
                                                       sizeof(FLASH_MANEGER));
        }else {
            ret = LOG_ERR_SIZE_FULL;
        }


        type_bit = 2;
#else
        write_start_addr = log_head_ctrl.err_log_start_addr  + sizeof(FLASH_MANEGER) + (err_count++) * sizeof(LOG_PACK);
        log_pack.event_id = err_count;
        log_head_ctrl.ops.flash_write(write_start_addr, (uint32_t*)&log_pack, sizeof(LOG_PACK) / 4);


#endif

        break;

    case WARN:
#if 0
        if(warn_manager.head_offset <= warn_manager.sector_size) {
            
            type_bit = 3;   // 4

            warn_manager.log_count++;
            log_pack.event_id = warn_manager.log_count;
            write_start_addr = log_head_ctrl.warn_log_start_addr  + sizeof(FLASH_MANEGER) + warn_manager.head_offset;
            
            log_head_ctrl.ops.flash_write(write_start_addr, (uint32_t*)&log_pack, sizeof(LOG_PACK));
            warn_manager.head_offset += sizeof(LOG_PACK);
			/*更新mannager*/
			log_head_ctrl.ops.flash_write(log_head_ctrl.warn_log_start_addr, 
                                                       (uint32_t*)&warn_manager, 
                                                       sizeof(FLASH_MANEGER));
        }else {
            ret = LOG_RAWN_SIZE_FULL;
        }


        type_bit = 3;
#else 
        write_start_addr = log_head_ctrl.warn_log_start_addr  + sizeof(FLASH_MANEGER) + (warn_count++) * sizeof(LOG_PACK);
        log_pack.event_id = warn_count;
        log_head_ctrl.ops.flash_write(write_start_addr, (uint32_t*)&log_pack, sizeof(LOG_PACK) / 4);
#endif

        break;
    default:
        
        break;
    }

#if 0
    /*更新头部表 */
    if(type_bit == 1) {
        /*info */
        if(ctrl_info.log_info_flag != WRITE_FLAG) {
            ctrl_info.log_info_flag = WRITE_FLAG;
            log_head_ctrl.ops.flash_write(FLASH_START_ADDR, (uint32_t*)&ctrl_info, (sizeof(CTRL_INFO) / 4));
        }
    }else if(type_bit == 2) {
        /*err*/
        if(ctrl_info.log_err_flag != WRITE_FLAG) {
            ctrl_info.log_err_flag = WRITE_FLAG;
            log_head_ctrl.ops.flash_write(FLASH_START_ADDR, (uint32_t*)&ctrl_info, sizeof(CTRL_INFO));
        }
    }else if(type_bit ==  3) {
        /*warn*/
        if(ctrl_info.log_warn_flag != WRITE_FLAG) {
            ctrl_info.log_info_flag = WRITE_FLAG;
            log_head_ctrl.ops.flash_write(FLASH_START_ADDR, (uint32_t*)&ctrl_info, sizeof(CTRL_INFO));
        }
    }
#endif

	return ret;

}



/**
 * @funciton:   f_log_write
 * @breif:      用户使用写入日志
 * @param{in}:  log_type    写入日志类型
 * @param{in}:  写入数据内容
 * @retval:     写入状态
 */
uint8_t f_log_write(uint8_t log_type,const char* fmt,...) {
    char w_buffer[SUPPORT_MAX_WRITE_SIZE] = {0};
    uint8_t ret = LOG_OK;
    uint16_t buf_size;
	
    va_list args;
    va_start(args, fmt);
    vsnprintf(w_buffer, sizeof(w_buffer), fmt, args);
    va_end(args);
    buf_size = c_strlen(w_buffer);
	
    ret = flash_log_write(log_type,(uint8_t*)w_buffer,buf_size);
	
    return ret;
}


/**
 * @funciton:       base_log_read
 * @breif:          日志读取
 * @param[in]:      读取的日志类型
 * @param[in]:      需要读取的索引
 * @retval：        读取的状态
 */
uint8_t f_log_read(uint8_t log_type,uint8_t indx,LOG_PACK* log_pack) {
    uint32_t primask = __get_PRIMASK();
	uint8_t ret = LOG_OK;
	uint32_t temp_addr;
    switch(log_type) {
        case INFO:
        if(indx > info_manager.log_count) {
            ret = LOG_INFO_INDX_LARGER;
            return  ret;
        }
		temp_addr = log_head_ctrl.info_log_start_addr +   MANAGER_SIZE + (indx * sizeof(LOG_PACK));
        log_head_ctrl.ops.flash_read(temp_addr,(uint32_t*)log_pack,sizeof(LOG_PACK));
        break;
        case ERR:
        if(indx > err_manager.log_count) {
            ret = LOG_ERR_INDX_LARGER;
            return  ret;
        }
        log_head_ctrl.ops.flash_read(log_head_ctrl.err_log_start_addr + (indx * MANAGER_SIZE),
                                                                        (uint32_t*)log_pack,
                                                                        sizeof(LOG_PACK));
        break;
        case WARN:
        if(indx > warn_manager.log_count) {
            ret = LOG_WARN_INDX_LARGER;
            return  ret;
        }
        log_head_ctrl.ops.flash_read(log_head_ctrl.warn_log_start_addr + (indx * MANAGER_SIZE),
                                                                        (uint32_t*)log_pack,
                                                                        sizeof(LOG_PACK));
        break;
        default:
        break;
    }
    __set_PRIMASK(primask);  // 恢复原始中断状态
	return ret;
}


#endif



