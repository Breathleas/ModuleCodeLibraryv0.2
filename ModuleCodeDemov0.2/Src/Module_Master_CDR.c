# include "Module_Master_CDR.h"
#include "stm32f1xx_hal.h"


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
}

void ClearLatchTxfault(void)
{
  Latch_Tx1_Fault = 0;
  Latch_Tx2_Fault = 0;
  Latch_Tx3_Fault = 0;
  Latch_Tx4_Fault = 0;
}
