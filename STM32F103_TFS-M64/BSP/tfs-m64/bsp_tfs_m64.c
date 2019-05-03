#include "bsp_tfs_m64.h"
#include "bsp_usart3.h"
#include "bsp_usart1.h"
#include "bsp_systick.h"

///ָ��ģ�� ��Fingerprint module

/**
  * @brief  ͨ������1���յ���ָ���ͨ������3��ָ��ģ�鷢�����������Ӧָ��
  * @param  none
  * @retval none
  */
void fpm_sendCmdBySerial(void)
{
  if(usart1_rx_cnt >= 8)
  {
    usart1_rx_cnt = 0;
    fpm_sendAndReceive(usart1_rx_buffer, 500);
  }
}
/**
  * @brief  ͨ������3��ָ��ģ�鷢�����������Ӧָ��
  * @param  cmdBuffer:�������飬�ڵ��øú���ʱֻ�轫���������CMD P1 P2 P3λ��ֵ���� 
  * @param  timeOut����ʱʱ�䣨��λ��ms�����ڴ�ʱ���������δ���յ���Ӧָ���ʱ����
  * @retval Ӧ����Ϣ
  */
uint8_t fpm_sendAndReceive(uint8_t *cmdBuffer, uint16_t timeout)
{
  uint8_t  i, j;
  uint8_t checkSum = 0;  //У����
	 
  usart3_rx_cnt = 0; //���ռ������㣬�൱����ջ�����
  
  cmdBuffer[HEAD] = 0xF5;
  cmdBuffer[TAIL] = 0xF5;
  cmdBuffer[5] = 0; //�������ݵĵ���λʼ��Ϊ��
  for(i = 1; i < 6; i++)
  {
    checkSum ^= cmdBuffer[i];
  }
  cmdBuffer[CHK]  = checkSum;
  
  USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
  for(i = 0; i < 8; i++)
  {
    usart3_sendByte(cmdBuffer[i]);
  }
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
  
  while(usart3_rx_cnt < 8 && timeout > 0)
  {
    delay_ms(1);
    timeout--;		   
  }
  /*��ӡ���յ�����Ӧ��������*/
   for(i = 0; i < 8; ++i)
  {
    printf("%02x ",usart3_rx_buffer[i]);
  }
  
  if (usart3_rx_cnt != 8) return ACK_TIMEOUT;
  if (usart3_rx_buffer[HEAD] != CMD_HEAD) return ACK_FAIL;
  if (usart3_rx_buffer[TAIL] != CMD_TAIL) return ACK_FAIL;
  if (usart3_rx_buffer[CMD] != (usart3_rx_buffer[CMD])) return ACK_FAIL;

  checkSum = 0; //У�������ǰ��������
  for (j = 1; j < CHK; j++) 
  {   
    checkSum ^= usart3_rx_buffer[j];
  }
	if (checkSum != usart3_rx_buffer[CHK]) 
  {
    return ACK_FAIL;
  }
  
	 return ACK_SUCCESS;
}	 

/**
  * @brief  ʹָ��ģ���������ģʽ
  * @param  none
  * @retval Ӧ����Ϣ��ACK_SUCCESS  ACK_FAIL��
  */
uint8_t fpm_sleep(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_SLEEP_MODE;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 500);
  
  if(res == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
  
}

/**
  * @brief  ����ָ�Ƶ����ģʽ
  * @param  mode��ָ�Ƶ����ģʽ��0�������ظ� 1����ֹ�ظ���
  * @retval Ӧ����Ϣ��ACK_SUCCESS  ACK_FAIL��
  */
uint8_t fpm_setAddMode(uint8_t mode)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_ADD_MODE;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = mode;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
}

/**
  * @brief  ��ȡָ�Ƶ����ģʽ
  * @param  none
  * @retval Ӧ����Ϣ��0�������ظ� 1����ֹ�ظ���
  */
uint8_t fpm_readAddMode(void)
{ 
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_ADD_MODE;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0X01;
  
  fpm_sendAndReceive(cmdBuffer, 200);
  
  return usart3_rx_buffer[Q2];
}

/**
  * @brief  ��ȡ�û�����ʵ����Ӧ����ָ�������������û�����ʾ��Ϊ����ͨ��Э�����һ�£�
  * @param  none
  * @retval Ӧ����Ϣ��ָ�Ƶ���Ŀ��
  */
uint16_t fpm_getUserNum(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_USER_CNT;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return usart3_rx_buffer[Q2];
  }
  else {
    return 0XFF;
  }
  
}

/**
  * @brief  ɾ�����е�ָ��
  * @param  none
  * @retval Ӧ����Ϣ��ACK_SUCCESS  ACK_FAIL��
  */
uint8_t fpm_deleteAllUser(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_DEL_ALL;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
}

/**
  * @brief  ɾ���ƶ�λ�õ�ָ��
  * @param  userNum��ָ�Ƶ�λ�ã�1-255��
  * @retval Ӧ����Ϣ��ACK_SUCCESS  ACK_FAIL��
  */
uint8_t fpm_deleteUser(uint8_t userNum)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_DEL;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = userNum;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 200);
  
  if(res == ACK_SUCCESS && usart3_rx_buffer[Q3] == ACK_SUCCESS) {
    return ACK_SUCCESS;
  }
  else {
    return ACK_FAIL;
  }
}

/**
  * @brief  ¼��ָ��
  * @param  userNum��ָ�ƴ����λ�ã�1-255��
  * @param  userPermission���û�Ȩ�ޣ�1-3�������庬�����ж��塣
  * @retval Ӧ����Ϣ��ACK_SUCCESS  ACK_FAIL  ACK_USER_EXIST  ACK_TIMEOUT��
  */
uint8_t fpm_addUser(uint8_t userNum, uint8_t userPermission)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_ADD_1;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = userNum;
  cmdBuffer[P3] = userPermission;
  
  res = fpm_sendAndReceive(cmdBuffer, 30000);

  if(res == ACK_SUCCESS) {
    if(usart3_rx_buffer[Q3] == ACK_SUCCESS) {
      cmdBuffer[CMD] = CMD_ADD_2;
      
      res = fpm_sendAndReceive(cmdBuffer, 30000);
      
      if(res == ACK_SUCCESS) {
        if(usart3_rx_buffer[Q3] == ACK_SUCCESS) {
          cmdBuffer[CMD] = CMD_ADD_3;
          
          res = fpm_sendAndReceive(cmdBuffer, 30000);
          
          if(res == ACK_SUCCESS) {
            return usart3_rx_buffer[Q3];
          }  
        }
      }
    }
  }
  return res;
 
}

/**
  * @brief  ͨ��1��N�ķ�ʽ�Ƚ�ָ��
  * @param  none
  * @retval ƥ��ָ�Ƶ���Ϣ
  */
uint8_t fpm_compareFinger(void)
{
  uint8_t res;
  uint8_t cmdBuffer[8];
  
  cmdBuffer[CMD] = CMD_MATCH;
  cmdBuffer[P1] = 0;
  cmdBuffer[P2] = 0;
  cmdBuffer[P3] = 0;
  
  res = fpm_sendAndReceive(cmdBuffer, 30000);
  
  if(res == ACK_SUCCESS) 
  {
    if(usart3_rx_buffer[Q3] == ACK_NOUSER) {
      return ACK_NOUSER;
    }
    if(usart3_rx_buffer[Q3] == ACK_TIMEOUT) {
      return ACK_TIMEOUT;
    }
    if((usart3_rx_buffer[Q2] != 0) && (usart3_rx_buffer[Q3] == 1 || usart3_rx_buffer[Q3] == 2 || usart3_rx_buffer[Q3] == 3)) {
      return ACK_SUCCESS;
    }
  }
  return res;
}

