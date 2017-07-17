#include "utilities.h"
#include "constant.h"
#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "Module_MemMap.h"

#define Vadc          3.3
#define V25           1774.0
#define AVG_SLOPE     0.0043

static uint8_t latch_temp_high_alarm = 0;

void EmptyBuffer(uint8_t* a)
{
	int i = 0;
	for(i =0; i<= PAGE_SIZE+1;i++)
	{
		a[i] = 0;
  }
}

uint16_t GetTemperature (ADC_HandleTypeDef *hadc)
{
	uint32_t TemBuffer = 0x00000000;
	HAL_ADC_Start(hadc);
	HAL_Delay(500);
	TemBuffer = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);
return ((uint16_t)(TemBuffer & 0x0000FFFF));
	//double Measured_Temperature = ((V25 - (double)TemBuffer)*(Vadc/4095.0))/(AVG_SLOPE)-25.0;
	//uint16_t temp = ((uint16_t)(Measured_Temperature*256.0));
	//return temp;
}

void Tx_Pin_Disable(void)
{
   HAL_GPIO_WritePin(Tx_DSBL_GPIO_Port, Tx_DSBL_Pin, GPIO_PIN_SET);
}

void Tx_Pin_Enable(void)
{
   HAL_GPIO_WritePin(Tx_DSBL_GPIO_Port, Tx_DSBL_Pin, GPIO_PIN_RESET);
}

uint8_t GetTx_Pin_DIS(I2C_HandleTypeDef *hi2c)
{
  return HAL_GPIO_ReadPin(Tx_DSBL_GPIO_Port, Tx_DSBL_Pin);
}

void Assert_IntL(void)
{
	HAL_GPIO_WritePin(IntL_GPIO_Port, IntL_Pin, GPIO_PIN_RESET);
	uint8_t u = Internal_Read_MemMap(2);
	u = u & 0xFD;
	Internal_Write_MemMap(2,u);
}

void Deassert_IntL(void)
{
	HAL_GPIO_WritePin(IntL_GPIO_Port, IntL_Pin, GPIO_PIN_SET);
	uint8_t u = Internal_Read_MemMap(2);
	u = u | 0x02;
	Internal_Write_MemMap(2,u);
}

uint8_t IsModSelL(void)                //1不接受数据，0接收数据
{
	return HAL_GPIO_ReadPin(ModSelL_GPIO_Port, ModSelL_Pin);
}

void SetLatchTempHighAlarm(ADC_HandleTypeDef *hadc)
{
	/*if(GetTemperature(hadc) > TEMP_ALARM_VALUE )
	{
	   latch_temp_high_alarm = 0x80;
	}*/
	return;
}

uint8_t GetLatchTempHighAlarm(void)
{
  return latch_temp_high_alarm;
}

void    ClearLatchTempHighAlarm(void)
{
	latch_temp_high_alarm = 0;
}

void Data_Ready(void)
{
	uint8_t u = Internal_Read_MemMap(2);
	u = u & 0xFE;
	Internal_Write_MemMap(2,u);
}

void Data_Not_Ready(void)
{
	uint8_t u = Internal_Read_MemMap(2);
	u = u | 0x01;
	Internal_Write_MemMap(2,u);
}

void GetIntL(void)
{
	HAL_GPIO_ReadPin(IntL_GPIO_Port, IntL_Pin);
}
