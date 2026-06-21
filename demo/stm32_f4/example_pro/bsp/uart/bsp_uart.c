
 
#include "bsp_uart.h" 
#include "sc_log.h"
#include "stdio.h"

extern UART_HANDLER usart_handler;

static void dma_init(void);

void uart1_init(uint32_t __Baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;	

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin           = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin           = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode          = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed         = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType         = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd          = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_DeInit(USART1);
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate              = __Baud;
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;
	USART_InitStructure.USART_Parity                = USART_Parity_No ;
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None; 
	USART_Init(USART1,&USART_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel                    = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority  = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority         = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd                 = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// DMA中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn; //嵌套通道为DMA2_Stream5_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //抢占优先级为 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //响应优先级为 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //通道中断使能
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);          //开启串口空闲中断

	dma_init();

	USART_Cmd(USART1, ENABLE);
		
}

uint8_t dma_buffer[RING_BUFFER_SIZE];

static void dma_init(void) {
	DMA_InitTypeDef DMA_InitStructure;
    
    // 使能DMA时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    
    // 配置DMA流
    DMA_DeInit(USART_RX_DMA_STREAM);
    
    DMA_InitStructure.DMA_Channel = USART_RX_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = RING_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(USART_RX_DMA_STREAM, &DMA_InitStructure);
    
    // 使能DMA传输完成中断和错误中断
    DMA_ITConfig(USART_RX_DMA_STREAM, DMA_IT_TC | DMA_IT_TE | DMA_IT_FE, ENABLE);
    
    // 使能USART1的DMA接收
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
    // 启动DMA传输
    DMA_Cmd(USART_RX_DMA_STREAM, ENABLE);
}

#if !defined(__MICROLIB)
#if (__ARMCLIB_VERSION <= 6000000)

struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

void _sys_exit(int x)
{
	x = x;
}
#endif

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (uint8_t)ch);
	
	while( RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE) ){}
	
    return ch;
}


void uart1DmaClear(void)
{
    DMA_Cmd(USART_RX_DMA_STREAM, DISABLE);    // 关闭 DMA1_Channel5 通道
    DMA_SetCurrDataCounter(USART_RX_DMA_STREAM, RING_BUFFER_SIZE);   // 重新写入要传输的数据数量
    DMA_Cmd(USART_RX_DMA_STREAM, ENABLE);     // 使能 DMA1_Channel5 通道
}


static uint32_t last_dma_counter = RING_BUFFER_SIZE; // 全局静态变量，初始化为缓冲区大小

void USART1_IRQHandler(void) {
	 if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) {
        USART_ReceiveData(USART1);  // 清除空闲中断标志

        uint32_t current_counter = DMA_GetCurrDataCounter(USART_RX_DMA_STREAM);
        uint32_t received;
        uint32_t start_index;

        // 计算接收到的字节数
        if (current_counter <= last_dma_counter) {
            received = last_dma_counter - current_counter;
        } else {
            received = (last_dma_counter + RING_BUFFER_SIZE) - current_counter;
        }

        // 计算数据在dma_buffer中的起始位置
        start_index = (RING_BUFFER_SIZE - last_dma_counter) % RING_BUFFER_SIZE;
        last_dma_counter = current_counter;

        // 处理数据拷贝（考虑环形缓冲区回绕）
        if (start_index + received <= RING_BUFFER_SIZE) {
            // 数据未跨越缓冲区边界
            usart_handler.push(&dma_buffer[start_index], received);
        } else {
            // 数据跨越缓冲区边界，分两段拷贝
            uint16_t first_segment = RING_BUFFER_SIZE - start_index;
            usart_handler.push(&dma_buffer[start_index], first_segment);
            usart_handler.push(dma_buffer, received - first_segment);
        }
    }
}


// DMA2 Stream5中断服务函数
void DMA2_Stream5_IRQHandler(void) {
    // 处理传输完成中断
    if(DMA_GetITStatus(USART_RX_DMA_STREAM, DMA_IT_TCIF5)) {
        DMA_ClearITPendingBit(USART_RX_DMA_STREAM, DMA_IT_TCIF5);
        // TODO:处理传输完成中断
    }
    
    // 处理传输错误中断
    if(DMA_GetITStatus(USART_RX_DMA_STREAM, DMA_IT_TEIF5)) {
        DMA_ClearITPendingBit(USART_RX_DMA_STREAM, DMA_IT_TEIF5);
        // TODO:错误处理逻辑
    }
    
    // 处理FIFO错误中断
    if(DMA_GetITStatus(USART_RX_DMA_STREAM, DMA_IT_FEIF5)) {
        DMA_ClearITPendingBit(USART_RX_DMA_STREAM, DMA_IT_FEIF5);
        // TODO:错误处理逻辑
    }
}

/**
 * @function:   void send_char(uint8_t data);
 * @brief:		发送一个字节的函数
 */
void send_char(uint8_t data) {
	while (!(USART1->SR & USART_SR_TXE));
    USART1->DR = data;
}




