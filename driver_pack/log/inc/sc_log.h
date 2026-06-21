/**
 * @file: sc_log.h
 */

 #ifndef SC_LOG_H__
 #define SC_LOG_H__


#include "sc_log_conf.h"
#include <stdarg.h>
#include <string.h>

const char* get_sc_log_version(void);

#if RING_BUFFER_ENABLE
/**环形缓冲数组 */
#define RINGBUFFER_DECLARE(name)\
    struct ALIGN_4 name##_TYPE {   \
        uint16_t      rec_data_len;\
        uint8_t       ring_buffer[RING_BUFFER_SIZE];\
		uint16_t      front;\
        uint16_t      tail;\
    } name

enum {
    BUFFER_ERROR,
    BUFFER_SUCCESS
};  

typedef struct {
    void(*init)(void);              /**初始化*/
#if PUSH_CHAR
    uint8_t(*push)(uint8_t data);      /**单字节写入*/
#elif PUSH_STRING
    uint8_t(*push)(uint8_t* data,uint16_t len);    /**多字节写入*/
#endif
    uint16_t(*get_data)(uint8_t* dest, uint16_t max_len);   /*读取环形缓冲区的数据 */
}UART_HANDLER;


void buffer_init(UART_HANDLER* uart_handler);
#endif

/**日志*/
#if DEBUG_ENABLE

#ifdef RTT_DEBUG_ENABLE
#include "SEGGER_RTT.h"

/*是否开启具体的路径*/
#if LOG_PATH_ENABLE
#define LOG_I(ID,Index,sFormat,...) do{\
    SEGGER_RTT_SetTerminal(ID);\
    SEGGER_RTT_printf(Index,"[" #ID":%s->%s]:"sFormat,\
	__FILE__,__func__,\
	##__VA_ARGS__);\
}while(0);
#else
#define LOG_I(ID,Index,sFormat,...) do{\
    SEGGER_RTT_SetTerminal(ID);\
    SEGGER_RTT_printf(Index,"[" #ID":]:"sFormat,\
	##__VA_ARGS__);\
}while(0);

#endif

#else 
#define LOG_I(ID,Index,sFormat,...)  ((void)0)
#endif

#ifdef UART_DEBUG_ENABLE

/**需要终端支持ANSI颜色 */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void log_printf(uint8_t level, const char* format, ...);
#if LOG_PATH_ENABLE
#define LOG_PRINTF(name,color,format,...) do {\
    log_printf(\
            name,\
            ANSI_COLOR_##color "[" #name":%s->%s]:"format,\
			__FILE__,__func__,\
            ##__VA_ARGS__);\
}while(0);
#else
#define LOG_PRINTF(name,color,format,...) do {\
    log_printf(\
            name,\
            ANSI_COLOR_##color "[" #name":]"format,\
            ##__VA_ARGS__);\
}while(0);
#endif

#else 
#define LOG_PRINTF(name,color,format,...)			((void)0)
#endif


#ifdef FLASH_DEBUG_ENABLE




/* 便于功能的拓展 */
typedef struct {
    uint8_t (*flash_write)(uint32_t addr,uint32_t* write_buff,uint32_t wite_len);
    uint8_t (*flash_read)(uint32_t addr,uint32_t* read_buff,uint32_t read_len);
    uint8_t (*flash_earse)(uint32_t addr,uint32_t ears_len);
}FLASH_OPS;

/*写入的数据包格式*/
typedef struct __attribute__((aligned(4))) {
    uint32_t timestamp;
    uint32_t event_id;
    uint32_t data_len;
    uint8_t  data[SUPPORT_MAX_WRITE_SIZE];
} LOG_PACK;


/*文件头部块，进行信息的管理 */
typedef struct {
    FLASH_OPS   ops;                           // Flash操作函数
    uint32_t info_log_start_addr;               // 信息日志的起始地址
    uint32_t err_log_start_addr;                // 错误日志的起始地址
    uint32_t warn_log_start_addr;               // 原始数据的起始地址
}LOG_HEAD;


typedef struct {
    uint8_t log_info_flag;             	        // 日志是否需要擦除的标志位
    uint8_t log_err_flag;             			// 日志是否需要擦除的标志位
    uint8_t log_warn_flag;                  	// 日志是否需要擦除的标志位
    uint8_t log_all_flag;                   	// 日志是否需要擦除的标志位
}CTRL_INFO;


/*信息块的内容，对自己的信息进行管理*/

typedef struct ALIGN_4 {
    uint32_t    sector_size;                    // 扇区大小(字节)
    uint32_t    head_offset;                    // 扇区内偏移，应该为下一条数据写入的起始地址
    uint8_t    log_count;                       // 日志的数量
}FLASH_MANEGER;


#define X(name,x)     .name##_log_start_addr =  FLASH_START_ADDR + sizeof(CTRL_INFO) + (x * FLASH_SECTOR_SIZE) 

#define ADDR_LIST               \
        X(info, INFO),          \
        X(err,  ERR),          \
        X(warn, WARN),          \


/* 日志头部管理信息块初始化 */
#define CTRL_INIT(name)                         \
static CTRL_INFO name = {                       \
    .log_info_flag = WRITE_FLAG,              	\
    .log_err_flag = WRITE_FLAG,             	\
    .log_warn_flag = WRITE_FLAG,            	\
    .log_all_flag = WRITE_FLAG,             	\
};

#define HEAD_INIT(name)                         \
static LOG_HEAD name = {                        \
	.ops.flash_read = FLASH_READ_FUNC,\
	.ops.flash_write = FLASH_WRITE_FUNC,\
	.ops.flash_earse = FLASH_ERASE_FUNC,\
    ADDR_LIST                                   \
};


/* 信息块管理初始化 */
#define MANEGER_INIT(name)                  \
static FLASH_MANEGER name = {               \
    .sector_size = FLASH_SECTOR_SIZE,       \
    .head_offset = 0,                       \
    .log_count   = 0,                       \
};


/* 用户使用注册初始化接口 */
#define COMPONENT_INIT(TYPE,NAME)           \
        TYPE##_INIT(NAME)                   \

#define MANAGER_SIZE                    sizeof(FLASH_MANEGER)

uint8_t flash_log_init(void);
uint8_t f_log_write(uint8_t log_type,const char* fmt,...);   
uint8_t f_log_read(uint8_t log_type,uint8_t indx,LOG_PACK* log_pack);
#endif
#else
#define flash_log_init  ((void)0)
#define f_log_write     ((void)0)
#define f_log_read      ((void)0)
#endif


/**RTT、UART走同一个通道，但是flash走其他通道，因为需要支持终端打印*/
#define LOG(name,color,format,...)  do{\
    if(DEBUG_TARGETS & DEBUG_RTT_BIT) { \
        LOG_I(\
            name##_TERMINAL_ID, \
            name##_CHANNEL_INDEX, \
            RTT_CTRL_TEXT_BRIGHT_##color format, \
            ##__VA_ARGS__); \
    } \
    if(DEBUG_TARGETS & DEBUG_UART_BIT) { \
        LOG_PRINTF(\
            name,\
            color,\
            format,\
            ##__VA_ARGS__);\
    } \
}while(0);



#endif
