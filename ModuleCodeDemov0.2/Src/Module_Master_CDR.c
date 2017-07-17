# include "Module_Master_CDR.h"
#include "stm32f1xx_hal.h"
#include "utilities.h"
#include "constant.h"
#include "Module_MemMap.h"

static uint8_t Latch_Tx1_Fault = 0;
static uint8_t Latch_Tx2_Fault = 0;
static uint8_t Latch_Tx3_Fault = 0;
static uint8_t Latch_Tx4_Fault = 0;


uint8_t GetLatchTxFault(I2C_HandleTypeDef *hi2c)     //返回一个符合sff-8636协议的八位字符
{
  return 0x00 | (Latch_Tx4_Fault << 3) | (Latch_Tx3_Fault << 2) | (Latch_Tx2_Fault << 1) | (Latch_Tx1_Fault);
}

void SetLatchTxfault(I2C_HandleTypeDef *hi2c)
{
	//检测到有问题就把有问题的针脚调一。
	if(0)              //Tx4 Fault 条件
	{
		Latch_Tx4_Fault = 1;
		if((Internal_Read_MemMap(MODULE_AND_CHANNEL_MASKES+1) & 0x08) == 0)
		{
		  Assert_IntL();
		}
	}
	if(0)              //Tx3 Fault 条件
	{
		Latch_Tx3_Fault = 1;
		if((Internal_Read_MemMap(MODULE_AND_CHANNEL_MASKES+1) & 0x04) == 0)
		{
		  Assert_IntL();
		}
	}
	if(0)              //Tx2 Fault 条件
	{
		Latch_Tx2_Fault = 1;
		if((Internal_Read_MemMap(MODULE_AND_CHANNEL_MASKES+1) & 0x02) == 0)
		{
		  Assert_IntL();
		}
	}
	if(0)              //Tx1 Fault 条件
	{
	  Latch_Tx1_Fault = 1;
		if((Internal_Read_MemMap(MODULE_AND_CHANNEL_MASKES+1) & 0x01) == 0)
		{
		  Assert_IntL();
		}
	}
}

void ClearLatchTxfault(void)
{
  Latch_Tx1_Fault = 0;
  Latch_Tx2_Fault = 0;
  Latch_Tx3_Fault = 0;
  Latch_Tx4_Fault = 0;
}
