#include "Module_MemMap.h"
#include "main.h"
#include "constant.h"
#include "stdlib.h"
#include "Module_Master_CDR.h"
#include "utilities.h"
#include "Module_Master_Driver_I2C.h"

//1.0 版 只支持 SFF8636中 定义的最简功能，除去模块信息，Memory Map共有256个字节
#define RO_START_ADDRESS_1    0
#define RO_STOP_ADDRESS_1    85
#define RO_START_ADDRESS_2  108
#define RO_STOP_ADDRESS_2   111
#define RO_START_ADDRESS_3  128
#define RO_STOP_ADDRESS_3   223
//如增加只读区域的话需要修添加


static uint8_t MemMap_Array[MEMORY_MAP_SIZE] = {0};                  //内部内存地址
extern I2C_HandleTypeDef hi2c2;
//static int Address_Redirection(int Memory_Address);                实现多页的MemMap（未完成）

uint8_t* Get_Memmap(void)                                                //获取MemMap地址
{
	return MemMap_Array;
}

uint8_t Read_MemMap(int Memory_Address)                            //读MemMap
{
	if( Memory_Address >= MEMORY_MAP_SIZE)
	{
		return READ_ERR;
	}
	
	if(Memory_Address == STATUS+1)
	{
		if((*(MemMap_Array + Memory_Address) & 0x01) == 0)
		{
			Deassert_IntL();
		}
	}
	else if (Memory_Address == (INTERRUPT_FLAG + 1))
	{
		*(MemMap_Array + Memory_Address) = GetLatchTxFault(&hi2c2);
		 ClearLatchTxfault();
	}
	else if(Memory_Address == (INTERRUPT_FLAG + 3))
	{
	   *(MemMap_Array + Memory_Address) = GetLatchTempHighAlarm();
		 ClearLatchTempHighAlarm();
	}
	
	//*(MemMap_Array + Memory_Address) = MASTERI2C->GetFromChip(Memory_Address)
  return *(MemMap_Array + Memory_Address);
}

uint8_t Write_MemMap(int Memory_Address, uint8_t value)             //写MemMap
{
	if(( Memory_Address >= MEMORY_MAP_SIZE)                                                ||
		 ((Memory_Address >= RO_START_ADDRESS_1) && (Memory_Address <= RO_STOP_ADDRESS_1)) ||
		 ((Memory_Address >= RO_START_ADDRESS_2) && (Memory_Address <= RO_STOP_ADDRESS_2)) ||
	   ((Memory_Address >= RO_START_ADDRESS_3) && (Memory_Address <= RO_STOP_ADDRESS_3)))        
	   //如增加只读区域的话需要修添加
	{
		return WRITE_ERR;
	}
	else
	{
    *(MemMap_Array + Memory_Address) = value;
		
		/*if(Memory_Address == CONTROL)
		{
		  if((value &  0x08) != 0)
			{
			  TxDisable(&hi2c2,Channel_3);
			}
			else
			{
				TxEnable(&hi2c2,Channel_3);
			}
		  if((value &  0x04) != 0)
			{
			  TxDisable(&hi2c2,Channel_2);			  
			}
			else
			{
				TxEnable(&hi2c2,Channel_2);
			}
		  if((value &  0x02) != 0)
			{
			  TxDisable(&hi2c2,Channel_1);			  
			}
			else
			{
				TxEnable(&hi2c2,Channel_1);
			}
      if((value &  0x01) != 0)
			{
			  TxDisable(&hi2c2,Channel_0);			  
		  }
			else
			{
				TxEnable(&hi2c2,Channel_0);
			}
		}*/
		//MASTERI2C->PushToChip(Memory_Address,value)
	}
	return WRITE_NORMAL;
}

uint8_t Internal_Write_MemMap(int Memory_Address, uint8_t value)     //内部写MemMap
{
	if(Memory_Address >= MEMORY_MAP_SIZE)
	{
		return WRITE_ERR;
	}
  *(MemMap_Array + Memory_Address) = value;
	return WRITE_NORMAL;
}

uint8_t Internal_Read_MemMap(int Memory_Address)                            //读MemMap
{
	if( Memory_Address >= MEMORY_MAP_SIZE)
	{
		return READ_ERR;
	}
	//*(MemMap_Array + Memory_Address) = MASTERI2C->GetFromChip(Memory_Address)
  return *(MemMap_Array + Memory_Address);
}
//86 Tx 失能
//87 选择 Rx 的速率 （可选）
//88 选择 Tx 的速率 （可选）
//89 - 92 Software Application 选择 （可选）
