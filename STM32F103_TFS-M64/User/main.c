#include "stm32f10x.h"

#include "bsp.h"



int main(void)
{
  uint8_t res;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  
  bsp_init();
   
//  fpm_addUser(1,1);
//  fpm_addUser(2,1);
//  fpm_addUser(3,1);
  res = fpm_getUserNum();
  printf("user number:%d\n",res);
  res = fpm_readAddMode();
  printf("Add Mode:%d\n",res);
//  res = fpm_compareFinger();
//  printf("match result:%d\n",res);
//  if(res == ACK_SUCCESS)
//  {
//    GPIO_SetBits(GPIOC, GPIO_Pin_13);
//  }
  
  while(1)
  {
    fpm_sendCmdBySerial();
    
  }
}
