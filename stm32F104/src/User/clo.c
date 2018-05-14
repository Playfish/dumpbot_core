#include "include.h"
#include "clo.h"
#include "GPIO.h"
#include "UART.h"
 u32 clo(void)
{
  
   GPIO_Init(PORTA,5,GPI,1); 
   GPIO_Init(PORTC,6,GPO,0); 
   char b[100];
   UART_Init (UART1,115200); 
   u32 a;
   a=GPIO_Get(PTA5);
  // b[0]=0x22;
   b[0]=0xB;
   b[1]=02;
   b[2]=a;
   b[3]=a>>8;
   b[4]=0x88;
  // sprintf(b, " ox22\n ºÚ°×Çò¼ì²â£º%d \nox88\n",a); 
   UART_Put_Buff(UART1, b,5);
   return a ; 
}
