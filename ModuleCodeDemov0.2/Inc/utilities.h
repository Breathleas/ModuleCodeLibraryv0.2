#ifndef UTILITIES
#define UTILITIES

#include "stdint.h"
#include "stm32f1xx_hal.h"

#define TEMP_ALARM_VALUE 100
void EmptyBuffer(uint8_t* a);
uint16_t GetTemperature (ADC_HandleTypeDef *hadc);
void Tx_Pin_Disable(void);
void Tx_Pin_Enable(void);
uint8_t GetTx_DIS(I2C_HandleTypeDef *hi2c);
void Assert_IntL(void);
void Deassert_IntL(void);
uint8_t IsModSelL(void);
void    SetLatchTempHighAlarm(ADC_HandleTypeDef *hadc);
uint8_t GetLatchTempHighAlarm(void);
void    ClearLatchTempHighAlarm(void);
void Data_Ready(void);
void Data_Not_Ready(void);
void GetIntL(void);
#endif
