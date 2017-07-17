#include "Module_MemMap.h"
#include "main.h"
#include "constant.h"
#include "stdlib.h"
#include "Module_Master_CDR.h"
#include "utilities.h"
#include "Module_Master_Driver_I2C.h"

//1.0 �� ֻ֧�� SFF8636�� ���������ܣ���ȥģ����Ϣ��Memory Map����256���ֽ�
#define RO_START_ADDRESS_1    0
#define RO_STOP_ADDRESS_1    85
#define RO_START_ADDRESS_2  108
#define RO_STOP_ADDRESS_2   111
#define RO_START_ADDRESS_3  128
#define RO_STOP_ADDRESS_3   223
//������ֻ������Ļ���Ҫ�����


static uint8_t MemMap_Array[MEMORY_MAP_SIZE] = {0};                  //�ڲ��ڴ��ַ
extern I2C_HandleTypeDef hi2c2;
//static int Address_Redirection(int Memory_Address);                ʵ�ֶ�ҳ��MemMap��δ��ɣ�

uint8_t* Get_Memmap(void)                                                //��ȡMemMap��ַ
{
	return MemMap_Array;
}

uint8_t Read_MemMap(int Memory_Address)                            //��MemMap
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

uint8_t Write_MemMap(int Memory_Address, uint8_t value)             //дMemMap
{
	if(( Memory_Address >= MEMORY_MAP_SIZE)                                                ||
		 ((Memory_Address >= RO_START_ADDRESS_1) && (Memory_Address <= RO_STOP_ADDRESS_1)) ||
		 ((Memory_Address >= RO_START_ADDRESS_2) && (Memory_Address <= RO_STOP_ADDRESS_2)) ||
	   ((Memory_Address >= RO_START_ADDRESS_3) && (Memory_Address <= RO_STOP_ADDRESS_3)))        
	   //������ֻ������Ļ���Ҫ�����
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

uint8_t Internal_Write_MemMap(int Memory_Address, uint8_t value)     //�ڲ�дMemMap
{
	if(Memory_Address >= MEMORY_MAP_SIZE)
	{
		return WRITE_ERR;
	}
  *(MemMap_Array + Memory_Address) = value;
	return WRITE_NORMAL;
}

uint8_t Internal_Read_MemMap(int Memory_Address)                            //��MemMap
{
	if( Memory_Address >= MEMORY_MAP_SIZE)
	{
		return READ_ERR;
	}
	//*(MemMap_Array + Memory_Address) = MASTERI2C->GetFromChip(Memory_Address)
  return *(MemMap_Array + Memory_Address);
}
//86 Tx ʧ��
//87 ѡ�� Rx ������ ����ѡ��
//88 ѡ�� Tx ������ ����ѡ��
//89 - 92 Software Application ѡ�� ����ѡ��
