/*
 * @Author       : 824229900@qq.com
 * @Date         : 2025-07-18 21:32
 * @LastEditors  : SuperYu 824229900@qq.com
 * @LastEditTime : 2025-08-03 02:33
 * @Description  : 
 */
#include "mcu_flash.h"


/**
 * @funciton:   uint16_t mcu_flash_get_sector(uint32_t addr)
 * @brief:      返回地址所在的块
 * @param[in]:  读取的地址
 * @retval：    返回对应的块
 */
static uint16_t mcu_flash_get_sector(uint32_t addr) {
    if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}

/**
 * @function:   uint32_t mcu_flash_read_data(u32 faddr)
 * @brief:      读取地址内容
 * @retval:     具体的值
 */ 
#if 0
static uint32_t mcu_flash_read_data(u32 faddr) {
	return *(vu32*)faddr; 
}
#endif


/**
 * @function: uint8_t mcu_flash_erase(uint32_t addr,uint32_t ears_len)
 * @brief:      flash擦除
 * @param[in]:  uint32_t addr   擦除的首地址
 * @param[in]:  uint32_t ears_len   擦除的大小,取决于芯片支持的擦除大小
 * @retval:     擦除状态
 */

uint8_t mcu_flash_erase(uint32_t addr,uint32_t ears_len) {
    /**具体的内部实现*/
    return FLASH_EraseSector(mcu_flash_get_sector(addr),VoltageRange_3);

}

/**
 * @funciton: uint8_t mcu_flash_wirte(uint32_t addr,uint32_t* data,uint32_t size)
 * @breif:      flash写入
 * @param[in]:  uint32_t addr  写入的起始地址
 * @param[in]:  uint32_t* data  写入的数据内容
 * @param[in]:  uint32_t size   写入的大小
 * @retval:     写入状态
 */

uint8_t mcu_flash_wirte(uint32_t addr,uint32_t* data,uint32_t size) {
// FIXME: 日志写入写入进去不随便擦除，如果需要擦除，要使用对应的接口，否则会擦除重要的信息			@sy  2025-08-01
#if 0
    FLASH_Status status = FLASH_COMPLETE;
	uint32_t addrx = addr;
	uint32_t endaddr = 0;	
    if(addr < STM32_FLASH_BASE || addr % 4) {
        return CODE_ERROR;	
    }
	FLASH_Unlock();								
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 		
	endaddr = addr + size * 4;	        // 写入的结束地址
	
	if(addrx < 0X1FFF0000)			
	{

		while(addrx < endaddr)		
		{
			if(mcu_flash_read_data(addrx) != 0XFFFFFFFF)
			{   
				status = (FLASH_Status)mcu_flash_erase(addrx,size);
				if(status != FLASH_COMPLETE) {
                    break;	
                }
			} else {
                addrx+=4;
            } 
		} 

		
	}
	if(status == FLASH_COMPLETE)
	{
		while(addr < endaddr)//写数据
		{
			if(FLASH_ProgramWord(addr,*data) != FLASH_COMPLETE) { 
				break;	//写入异常
			}
			addr+=4;
			data++;
		} 
	}
    FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
    return CODE_SUCESS;

#else
	FLASH_Status status = FLASH_COMPLETE;

	uint32_t endaddr=0;	

	if(addr < STM32_FLASH_BASE || addr % 4) {
		return 0;	//非法地址
	} 
	FLASH_Unlock();									//解锁 
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR |FLASH_FLAG_PGAERR |FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR | FLASH_FLAG_BSY| FLASH_FLAG_RDERR);
	FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

	endaddr = addr + size * 4;	//写入的结束地址
	if(status == FLASH_COMPLETE)
	{
		while(addr < endaddr)//写数据
		{
			if(FLASH_ProgramWord(addr,*data) != FLASH_COMPLETE)			//写入数据
			{ 
				break;	//写入异常
			}
			addr+=4;
			data++;
		}
	}
	FLASH_DataCacheCmd(ENABLE);											//FLASH擦除结束,开启数据缓存
	FLASH_Lock();														//上锁
#endif
	return CODE_SUCESS;
}

/**
 * @function: uint8_t mcu_flash_read(uint32_t addr,uint32_t* buffer,uint32_t size)
 * @breif:      flash读取
 * @param[in]:  uint32_t addr  读取的起始地址
 * @param[in]:  uint32_t* data  读取的接受数据缓存
 * @param[in]:  uint32_t size   读取的大小
 * @retval:     读取状态
 */
// 修改函数原型和实现
uint8_t mcu_flash_read(uint32_t addr, uint32_t* buffer, uint32_t byte_size) {
    // 确保地址和缓冲区4字节对齐
    if((addr & 0x3) != 0 || ((uintptr_t)buffer & 0x3) != 0) {
        return CODE_ERROR;
    }
    
    uint32_t word_count = (byte_size + 3) / 4; // 向上取整
    uint32_t* buf_ptr = (uint32_t*)buffer;
    
    for(uint32_t i = 0; i < word_count; i++) {
        buf_ptr[i] = *(volatile uint32_t*)addr;
        addr += 4;
    }
    return CODE_SUCESS;
}


