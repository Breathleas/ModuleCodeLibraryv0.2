
//���c�ļ�����˹�������λ����ͨ��I2C�ĳ�ʼ���жϣ��ر��жϺ��жϺ����ľ���ʵ�ֺ�һЩ�ڲ������Լ��ڲ����ݽṹ��

#include "Module_Slave_I2C.h"
#include "main.h"
#include "constant.h"
#include "Module_MemMap.h"
#include "utilities.h"

#define TXRXBUFFER_SIZE     RW_CHUNK_SIZE+1                            //��������С


//�ڲ����ݽṹ
static uint8_t TxRxBuffer[TXRXBUFFER_SIZE] = {0};                      //���ݷ��ͽ��ջ�����
static uint8_t Buffer_Internal_Address = 0;                            //��������ַָ�� 

//�ڲ�����
static void User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c);              //���յ���ַ�ж�
static void User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c);             //���ܵ�ֹͣ�ж�
static void User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c);       //���������ж�
static void User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c);       //���������ж�
static void User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c);       //���������ж�
static void User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c);        //���������ж�
static void User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c);                //δ��Ӧ�ж�

void I2C_Slave_Transreceiver_IT_Initialize(I2C_HandleTypeDef *hi2c)    //��ʼ���ж�
{
    //ʧ�� POS
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);
    
	  //���Ի�����æ״̬����I2C�ṹ
    hi2c->State = HAL_I2C_STATE_BUSY;
    hi2c->Mode = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    
	  //���������ͻ����С����I2C�ṹ
    hi2c->pBuffPtr = TxRxBuffer;
    hi2c->XferSize = TXRXBUFFER_SIZE;
    hi2c->XferCount = TXRXBUFFER_SIZE;

    //ʹ����Ӧ
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

    //ʹ���ж�
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
}

void I2C_Slave_Transreceiver_IT_Deinitialize(I2C_HandleTypeDef *hi2c)   //�ر��ж�
{
	 //��ջ�����
	  EmptyBuffer(TxRxBuffer);
	 
	  //ʧ���ж�
	  __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
	  
	  //ʧ����Ӧ
	  CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);
	
	  //״̬Ϊ����
	  hi2c->State = HAL_I2C_STATE_READY;
	  
	  //ģʽΪû��
    hi2c->Mode = HAL_I2C_MODE_NONE;
}

void User_Slave_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)      //�¼��жϴ���
{
    uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
    tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
    tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_EVT));
    tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF);
    tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
	
    /* ADDR ���� --------------------------------------------------------------*/
    if((tmp1 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_ADDR(hi2c);
    }
		
    /* STOPF ���� --------------------------------------------------------------*/
    else if((tmp3 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_STOPF(hi2c);
    }
		
    /* I2C �� ����״̬ -----------------------------------------------*/
    else if(tmp4 == SET)
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
			
      /* TXE ���� �� BTF δ���� -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveTransmit_TXE(hi2c);
      }
			
      /* BTF ���� -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveTransmit_BTF(hi2c);
      }
    }
		
    /* I2C �� ����״̬  --------------------------------------------------*/
    else
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
			
      /* RXNE ���� �� BTF δ���� ----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveReceive_RXNE(hi2c);
      }
			
      /* BTF ���� -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveReceive_BTF(hi2c);
      }
    }
}
static void User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)           //���յ���ַ�ж�
{
	uint32_t tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
	if((tmp4 == SET) && (hi2c->XferCount < hi2c->XferSize))          //�Ƿ�Ϊ�����˵�ַ�ķ���ģʽ
	{
		int i = 0;
		Buffer_Internal_Address = (*((hi2c->pBuffPtr)-1));             //���õ�ַ
	  for(i = 0; i < RW_CHUNK_SIZE; i++)                             //������д�뻺����
	  {
		  *((hi2c->pBuffPtr)+i) = Read_MemMap(Buffer_Internal_Address + i); 
	  }
  }
  else if ((tmp4 == SET) && (hi2c->XferCount == hi2c->XferSize))   //�Ƿ�Ϊδ���յ�ַ�ķ���ģʽ
	{
		int j = 0;
		for(j = 0; j < RW_CHUNK_SIZE; j++)                             //������д�뻺����
	  {
		  *((hi2c->pBuffPtr)+j) = Read_MemMap(Buffer_Internal_Address + j); 
	  }
	}
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);                                  //��� ADDR FLAG
	return;                                                          //����
}


static void User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)          //���ܵ�ֹͣ�ж�
{
   __HAL_I2C_CLEAR_STOPFLAG(hi2c);                                 //��� STOPF FLAG      
   int m = 0;
   int n = TXRXBUFFER_SIZE - hi2c->XferCount;        
	 if(n >0)                                                        //������ܵ�������
	 {
     Buffer_Internal_Address = TxRxBuffer[0];                      //���õ�ַ
	 }
	 while(m+1<n)                                                    //�����˵�ַ��������ʱ
	 {
		 Write_MemMap(Buffer_Internal_Address+m, TxRxBuffer[m+1]);     //������������д������Ĵ���
		 m++;
	 }
	 EmptyBuffer(TxRxBuffer);                                        //��ջ�����
	 hi2c->pBuffPtr = TxRxBuffer;                                    //���û�����ָ��
   hi2c->XferCount = TXRXBUFFER_SIZE;                              //���û�������С
   return;                                                         //����
}
static void User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c)         //���� TXE FLAG
{
  if(hi2c->XferCount != 0)
  {
    /* ������д��DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c)         //���� BTF FLAG ��Tx��
{
  if(hi2c->XferCount != 0)
  {
    /* ������д��DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c)         //���� RXNE FLAG
{
  if(hi2c->XferCount != 0)
  {
    /* ��DR������ */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c)           //���� BTF FLAG ��Rx��
{
  if(hi2c->XferCount != 0)
  {
    /* ��DR������ */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return;
}
void User_Slave_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)                 //�����жϴ���
{
  uint32_t tmp1 = 0;
	uint32_t tmp2 = 0;

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BERR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C ���ߴ����жϷ��� ----------------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;

    /* ��� BERR FLAG */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
    
    /* �ڴ�λ��stop֮���ܲ���start */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ARLO);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Arbitration Loss �����жϷ��� ---------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C δ��Ӧ�����жϷ��� ------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    tmp1 = hi2c->Mode;
    if(tmp1 == HAL_I2C_MODE_SLAVE)
    {
      User_I2C_Slave_AF(hi2c);                     //�Զ���δ��Ӧ�ж�
    }
    else
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
  
      /* ����ֹͣ */
      SET_BIT(hi2c->Instance->CR1,I2C_CR1_STOP);
  
      /* ��� AF FLAG */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
    }
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_OVR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Over-Run/Under-Run �жϷ��� -------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;
    /* ��� OVR FLAG */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
  }

  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    hi2c->State = HAL_I2C_STATE_READY;
    
    /* ���жδ�����ʱʧ�� Pos λ */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);
    
    HAL_I2C_ErrorCallback(hi2c);
  }
}
static void User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c)                       //δ��Ӧ�ж�
{
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);                                   //��� AF FLAG

  EmptyBuffer(TxRxBuffer);                                                   //��ջ�����
  hi2c->pBuffPtr = TxRxBuffer;                                               //���û�����ָ��
  hi2c->XferCount = TXRXBUFFER_SIZE;                                         //���û�������С
  return;
}
