#include "include.h"
#include "hx711.h"
#include "Lptmr.h"
#include "UART.h"
#include "GPIO.h"
#include "ADC.h"
long Count;
 int  j;

void time_delay_ms(u32 ms)
{
  u16 i;
  while(ms--)
  {
    for(i=0;i<1200;i++);
  }
}

long  ReadWeightCount()
{
   GPIO_Ctrl(PORTD,5,0);
    Count=0;
  while(GPIO_Get(PTD6)==1) //AD转换未结束则等待，否则开始读取
{}
 for (j=0;j<24;j++)
{
  GPIO_Ctrl(PORTD,5,1); //p3.4
   Count=Count<<1;
 GPIO_Ctrl(PORTD,5,0); 
  if(GPIO_Get(PTD6)==1)
  {
  Count++;
  }}
 GPIO_Ctrl(PORTD,5,1); //p3.4
 Count=Count^0x800000;//第25个脉冲下降沿来时，转换数据
GPIO_Ctrl(PORTD,5,0);
 Count =Count /780; 	//????êμ??μ?êμ?ê??á?
 }

  
//主函数
void main(void)
{
   PLL_Init(PLL180);
   //初始化PLL为180M
   
    Count=0;
   char data[100]; 
  UART_Init (UART1,115200);
   GPIO_Init(PORTD,5,GPO,1); //p3.4
   GPIO_Init(PORTD,6,GPI,1); 
   GPIO_Init(PORTD,13,GPO,1);
   GPIO_Init(PORTD,14,GPO,1);
   GPIO_Init(PORTD,15,GPO,1);
    GPIO_Init(PORTA,5,GPO,1);
   GPIO_Init(PORTB,21,GPO,1);
   GPIO_Init(PORTA,4,GPO,1);
  key_clean();
  EXTI_Init(PTA, 5, falling_down);
  EXTI_Init(PTB, 21, falling_down);
  EXTI_Init(PTA, 4, falling_down); 
 LPTMR_delay_us(3000);
  uart_get();  
  
  
  while(1){
    
  
    ReadWeightCount();
  
   LPTMR_delay_us(10);
    data[0]=0x22;
    data[1]=0xA;
    data[2]=01;
   data[3]= Count;
   data[4]=Count>>8;
   data[5]=02;
   data[6]=Key_valu();
   data[7]=0;
   data[8]=03;
   data[9]=0;
   data[10]=0;
   data[11]=04;
   data[12]=0;
   data[13]=0;
   data[14]=0x88;
    LPTMR_delay_ms(10);
  if(uart_get()==1){
   
   UART_Put_Buff(UART1, data,15);   //就发送出去1, data,15);*/
    LPTMR_delay_ms(10);
 }
 
      // sprintf(data,"电子称%dmg\n",Count);
      // UART_Put_Buff(UART1, data,15);  
       
    }
     
}

   

