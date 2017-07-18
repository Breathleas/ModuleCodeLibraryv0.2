
//���ͷ�ļ������ʵ���� Tx/Rx CDR оƬͨ�ŵĺ���������

#ifndef MODULE_MASTER_CDR
#define MODULE_MASTER_CDR

#include "stdint.h"
#include "stm32f1xx_hal.h"

//��·���
#define Tx0   0
#define Tx1   1
#define Tx2   2
#define Tx3   3
#define Rx0   4
#define Rx1   5
#define Rx2   6
#define Rx3   7
#define ChAll 8

//ʹ�ܣ�ʧ��
#define Value_Reset 0
#define Value_Set   1

//���Կ���
#define Precursor  0
#define Postcurosr 1

//оƬ����
#define GN2104_TX_I2C_ADDRESS                 0x24                                  //Tx CDR ��ַ
#define GN2104_RX_I2C_ADDRESS                 0x2C                                  //Rx CDR ��ַ
#define GN2104_ADDRESS_SIZE                   I2C_MEMADD_SIZE_16BIT                 //���ݴ�С


void SetLatchTxfault(I2C_HandleTypeDef *hi2c);                             //���� Tx Fault

uint8_t GetLatchTxFault(I2C_HandleTypeDef *hi2c);                          //��ȡ Tx Fault

void ClearLatchTxfault(void);                                              //��� Tx Fault

//���º�����δʵ�֣������ڵ��Ե�ʱ��v0.3�����д���

void CDRPowerUp(I2C_HandleTypeDef *hi2c, uint8_t channel);                    //��CDR

void CDRPowerDown(I2C_HandleTypeDef *hi2c, uint8_t channel);                  //�ر�CDR

void SetLosThres(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t value);

void SetLosHyst(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t value);

void CDRBypass(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t state);      //����CDR

void SetEqulizer(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t value);

void SetDriverMute(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t state); 

void SetDriverDeemp(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t value);

void SetDeempPrePostCursor(I2C_HandleTypeDef *hi2c, uint8_t channel, uint8_t PreorPost);

#endif
