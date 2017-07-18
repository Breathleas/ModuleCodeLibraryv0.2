
//���ͷ�ļ�����˹�������λ����ͨ��I2C�ĳ�ʼ���жϣ��ر��жϺ��жϺ����Ķ���

#ifndef MODULE_SLAVE_I2C
#define MODULE_SLAVE_I2C

#include "stm32f1xx_hal.h"

void I2C_Slave_Transreceiver_IT_Initialize(I2C_HandleTypeDef *hi2c);            //��ʼ���ж�

void I2C_Slave_Transreceiver_IT_Deinitialize(I2C_HandleTypeDef *hi2c);          //�ر��ж�

void User_Slave_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);                     //�����¼��ж�

void User_Slave_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);                     //��������ж�


#endif
