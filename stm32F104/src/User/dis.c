#include "include.h"
#include "dis.h"
#include "Lptmr.h"
#include "UART.h"
#include "GPIO.h"
long  Weight_Maopi =0 ;
u32 dis(void)
{
  u32 iDistance;
    GPIO_Init(PORTD,5,GPO,0); //TRIG信号
   GPIO_Init(PORTD,6,GPI,0);  //ECHO信号
   UART_Init (UART1,115200);
   char data[100];
 GPIO_Ctrl (PORTD,5,1);
   LPTMR_delay_us(10);
  GPIO_Ctrl (PORTD,5,0);
   while(GPIO_Get(PTD6)==0)
   {}

   LPTMR_time_start_us();      //开始计时(ms)

   while(GPIO_Get(PTD6)==1)
     {}
   
   iDistance= LPTMR_time_get_us()*34/2000;//获取计时时间
  data[0]=0x22;
  data[1]=0xA;
  data[2]=01;
  data[3]=iDistance;
  data[4]=iDistance>>8;
 
  // sprintf(data, "：%d\n ",iDistance);
    UART_Put_Buff(UART1, data,5);
 
return iDistance;
      

}
