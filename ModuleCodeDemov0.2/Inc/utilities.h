#ifndef UTILITIES
#define UTILITIES

#include "stdint.h"
#include "stm32f1xx_hal.h"

void EmptyBuffer(uint8_t* a);
uint16_t GetTemperature (ADC_HandleTypeDef *hadc);

#endif
