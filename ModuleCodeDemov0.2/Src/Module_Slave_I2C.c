
//这个c文件存放了关于与上位机沟通的I2C的初始化中断，关闭中断和中断函数的具体实现和一些内部函数以及内部数据结构。

#include "Module_Slave_I2C.h"
#include "main.h"
#include "constant.h"
#include "Module_MemMap.h"
#include "utilities.h"

#define TXRXBUFFER_SIZE     RW_CHUNK_SIZE+1                            //缓冲区大小


//内部数据结构
static uint8_t TxRxBuffer[TXRXBUFFER_SIZE] = {0};                      //数据发送接收缓冲区
static uint8_t Buffer_Internal_Address = 0;                            //缓冲区地址指针 

//内部函数
static void User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c);              //接收到地址中断
static void User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c);             //接受到停止中断
static void User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c);       //接收数据中断
static void User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c);       //接收数据中断
static void User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c);       //发送数据中断
static void User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c);        //发送数据中断
static void User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c);                //未响应中断

void I2C_Slave_Transreceiver_IT_Initialize(I2C_HandleTypeDef *hi2c)    //初始化中断
{
    //失能 POS
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);
    
	  //将丛机，繁忙状态读入I2C结构
    hi2c->State = HAL_I2C_STATE_BUSY;
    hi2c->Mode = HAL_I2C_MODE_SLAVE;
    hi2c->ErrorCode = HAL_I2C_ERROR_NONE;
    
	  //将缓冲区和缓冲大小读入I2C结构
    hi2c->pBuffPtr = TxRxBuffer;
    hi2c->XferSize = TXRXBUFFER_SIZE;
    hi2c->XferCount = TXRXBUFFER_SIZE;

    //使能响应
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);

    //使能中断
    __HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
}

void I2C_Slave_Transreceiver_IT_Deinitialize(I2C_HandleTypeDef *hi2c)   //关闭中断
{
	 //清空缓存区
	  EmptyBuffer(TxRxBuffer);
	 
	  //失能中断
	  __HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
	  
	  //失能相应
	  CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_ACK);
	
	  //状态为待命
	  hi2c->State = HAL_I2C_STATE_READY;
	  
	  //模式为没有
    hi2c->Mode = HAL_I2C_MODE_NONE;
}

void User_Slave_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c)      //事件中断处理
{
    uint32_t tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0;
    tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ADDR);
    tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, (I2C_IT_EVT));
    tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_STOPF);
    tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
	
    /* ADDR 触发 --------------------------------------------------------------*/
    if((tmp1 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_ADDR(hi2c);
    }
		
    /* STOPF 触发 --------------------------------------------------------------*/
    else if((tmp3 == SET) && (tmp2 == SET))
    {
      User_I2C_Slave_STOPF(hi2c);
    }
		
    /* I2C 在 发送状态 -----------------------------------------------*/
    else if(tmp4 == SET)
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TXE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
			
      /* TXE 触发 和 BTF 未触发 -----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveTransmit_TXE(hi2c);
      }
			
      /* BTF 触发 -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveTransmit_BTF(hi2c);
      }
    }
		
    /* I2C 在 接受状态  --------------------------------------------------*/
    else
    {
      tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_RXNE);
      tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_BUF);
      tmp3 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BTF);
      tmp4 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_EVT);
			
      /* RXNE 触发 和 BTF 未触发 ----------------------------------------------*/
      if((tmp1 == SET) && (tmp2 == SET) && (tmp3 == RESET))
      {
        User_I2C_SlaveReceive_RXNE(hi2c);
      }
			
      /* BTF 触发 -------------------------------------------------------------*/
      else if((tmp3 == SET) && (tmp4 == SET))
      {
        User_I2C_SlaveReceive_BTF(hi2c);
      }
    }
}
static void User_I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c)           //接收到地址中断
{
	uint32_t tmp4 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA);
	if((tmp4 == SET) && (hi2c->XferCount < hi2c->XferSize))          //是否为接受了地址的发送模式
	{
		int i = 0;
		Buffer_Internal_Address = (*((hi2c->pBuffPtr)-1));             //设置地址
	  for(i = 0; i < RW_CHUNK_SIZE; i++)                             //将数据写入缓冲区
	  {
		  *((hi2c->pBuffPtr)+i) = Read_MemMap(Buffer_Internal_Address + i); 
	  }
  }
  else if ((tmp4 == SET) && (hi2c->XferCount == hi2c->XferSize))   //是否为未接收地址的发送模式
	{
		int j = 0;
		for(j = 0; j < RW_CHUNK_SIZE; j++)                             //将数据写入缓冲区
	  {
		  *((hi2c->pBuffPtr)+j) = Read_MemMap(Buffer_Internal_Address + j); 
	  }
	}
  __HAL_I2C_CLEAR_ADDRFLAG(hi2c);                                  //清空 ADDR FLAG
	return;                                                          //返回
}


static void User_I2C_Slave_STOPF(I2C_HandleTypeDef *hi2c)          //接受到停止中断
{
   __HAL_I2C_CLEAR_STOPFLAG(hi2c);                                 //清空 STOPF FLAG      
   int m = 0;
   int n = TXRXBUFFER_SIZE - hi2c->XferCount;        
	 if(n >0)                                                        //如果接受到了数据
	 {
     Buffer_Internal_Address = TxRxBuffer[0];                      //设置地址
	 }
	 while(m+1<n)                                                    //当除了地址还有数据时
	 {
		 Write_MemMap(Buffer_Internal_Address+m, TxRxBuffer[m+1]);     //将缓冲区数据写入虚拟寄存器
		 m++;
	 }
	 EmptyBuffer(TxRxBuffer);                                        //清空缓冲区
	 hi2c->pBuffPtr = TxRxBuffer;                                    //设置缓冲区指针
   hi2c->XferCount = TXRXBUFFER_SIZE;                              //设置缓冲区大小
   return;                                                         //返回
}
static void User_I2C_SlaveTransmit_TXE(I2C_HandleTypeDef *hi2c)         //处理 TXE FLAG
{
  if(hi2c->XferCount != 0)
  {
    /* 将数据写入DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveTransmit_BTF(I2C_HandleTypeDef *hi2c)         //处理 BTF FLAG （Tx）
{
  if(hi2c->XferCount != 0)
  {
    /* 将数据写入DR */
    hi2c->Instance->DR = (*hi2c->pBuffPtr++);
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveReceive_RXNE(I2C_HandleTypeDef *hi2c)         //处理 RXNE FLAG
{
  if(hi2c->XferCount != 0)
  {
    /* 从DR读数据 */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return;
}
static void User_I2C_SlaveReceive_BTF(I2C_HandleTypeDef *hi2c)           //处理 BTF FLAG （Rx）
{
  if(hi2c->XferCount != 0)
  {
    /* 从DR读数据 */
    (*hi2c->pBuffPtr++) = hi2c->Instance->DR;
    hi2c->XferCount--;
  }
  return;
}
void User_Slave_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c)                 //错误中断处理
{
  uint32_t tmp1 = 0;
	uint32_t tmp2 = 0;

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_BERR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C 总线错误中断发生 ----------------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;

    /* 清除 BERR FLAG */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
    
    /* 在错位的stop之后不能产生start */
    SET_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_ARLO);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Arbitration Loss 错误中断发生 ---------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;

    /* Clear ARLO flag */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_AF);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C 未响应错误中断发生 ------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    tmp1 = hi2c->Mode;
    if(tmp1 == HAL_I2C_MODE_SLAVE)
    {
      User_I2C_Slave_AF(hi2c);                     //自定义未响应中断
    }
    else
    {
      hi2c->ErrorCode |= HAL_I2C_ERROR_AF;
  
      /* 产生停止 */
      SET_BIT(hi2c->Instance->CR1,I2C_CR1_STOP);
  
      /* 清除 AF FLAG */
      __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
    }
  }

  tmp1 = __HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_OVR);
  tmp2 = __HAL_I2C_GET_IT_SOURCE(hi2c, I2C_IT_ERR);
  /* I2C Over-Run/Under-Run 中断发生 -------------------------------*/
  if((tmp1 == SET) && (tmp2 == SET))
  {
    hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;
    /* 清除 OVR FLAG */
    __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
  }

  if(hi2c->ErrorCode != HAL_I2C_ERROR_NONE)
  {
    hi2c->State = HAL_I2C_STATE_READY;
    
    /* 当中段错误发生时失能 Pos 位 */
    CLEAR_BIT(hi2c->Instance->CR1, I2C_CR1_POS);
    
    HAL_I2C_ErrorCallback(hi2c);
  }
}
static void User_I2C_Slave_AF(I2C_HandleTypeDef *hi2c)                       //未响应中断
{
  __HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);                                   //清空 AF FLAG

  EmptyBuffer(TxRxBuffer);                                                   //清空缓冲区
  hi2c->pBuffPtr = TxRxBuffer;                                               //设置缓冲区指针
  hi2c->XferCount = TXRXBUFFER_SIZE;                                         //设置缓冲区大小
  return;
}
