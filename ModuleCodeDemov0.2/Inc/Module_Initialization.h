
//这个头文件存放了关于模块初始化的一些函数

#ifndef MODULE_INITIALIZATION
#define MODULE_INITIALIZATION

#include "stm32f1xx_hal.h"

void Module_Init_Hardware(void);     //初始化板载硬件

void Module_Init_Register(void);     //初始化模块

#endif
