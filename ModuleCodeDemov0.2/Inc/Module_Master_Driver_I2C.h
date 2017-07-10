#ifndef MODULE_MASTER_DRIVER_I2C
#define MODULE_MASTER_DRIVER_I2C

#include "stm32f1xx_hal.h"

#define GN1185_ADDRESS               0xA6
#define I_Mod                        0
#define I_Bias                       1
#define Mon_Disable                  2
#define Channel_0                    0
#define Channel_1                    1
#define Channel_2                    2
#define Channel_3                    3
#define Channel_All                  4
#define MAX_IBIAS_CUR                70.0
#define MAX_IMOD_CUR                 55.0


void SetTxModulationCurrent(double modulationCurrent, I2C_HandleTypeDef *hi2c, uint8_t channel);

void SetTxBiasSinkCurrent(double biasSinkCurrent, I2C_HandleTypeDef *hi2c, uint8_t channel);

void TxEnable(I2C_HandleTypeDef *hi2c, uint8_t channel);     //失能Tx_Dis

void TxDisable(I2C_HandleTypeDef *hi2c, uint8_t channel);    //使能Tx_Dis 如果输入是 Channel_All

uint8_t GetTxStatus(I2C_HandleTypeDef *hi2c, uint8_t channel);

void SetCurrentMonitor(I2C_HandleTypeDef *hi2c,uint8_t currentType, uint8_t channel);

void SetLDD_EYE_OPT(uint8_t value, I2C_HandleTypeDef *hi2c, uint8_t channel); 

void SetEqulizationMag(uint8_t value, I2C_HandleTypeDef *hi2c, uint8_t channel); 
	
void SetEqulizationPhase(uint8_t phase, I2C_HandleTypeDef *hi2c, uint8_t channel); 
	
void EnableEqulizer(I2C_HandleTypeDef *hi2c, uint8_t channel);
	
void DisableEqulizer(I2C_HandleTypeDef *hi2c, uint8_t channel);

void SetCrossingPointAdj(uint8_t value, I2C_HandleTypeDef *hi2c, uint8_t channel);  //35-65
	
void EnableCrossingPointAdj(I2C_HandleTypeDef *hi2c, uint8_t channel);
	
void DisableCrossingPointAdj(I2C_HandleTypeDef *hi2c, uint8_t channel);

#endif
