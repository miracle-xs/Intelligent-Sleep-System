/**
  ******************************************************************************
  * @file    main.c
  * @author  
  * @version V1.0.0
  * @date    3/14/2015
  * @brief   Main program body
	2017年11月24日   云服务器通信成功
	第三块板子测试程序
	转发串口数据，串口数据格式为:   H:xx;B:xx;S:x;HH:0;HX:1321;
	连接wifi务必注意波特率115200，串口2 中断优先级
  ******************************************************************************/
#include <stdio.h>
#include "SCI.H"
#include "USER_delay.h"
#include "stm32f10x_adc.h"
#include "string.h"
#include "myiic.h"
#include "lcd.h"



void init(void);

 /************************************************************************
* 函数名：int main(void)										    
* 功能说明：主函数                                       
************************************************************************/

void chage_char(long data1,u8 len1,long data2,u8 len2,u8 *str32) //数据转换函数
{
	u16 i;
	if(len1+len2>30)
	{
		LCD_ShowString(40,200,200,16,16,"Long error!");
  }
  else	
	{
		for(i=0;i<len1;i++)
		{
			*str32 = (u8)(data1%10)+'0';
			data1 = data1/10;
			str32++;		
		}	
		for(i=len1;i<len1+len2;i++)
		{
			*str32 = (u8)(data2%10)+'0';
			data2 = data2/10;
			str32++;			
		}	
	}	
}

int main(void)
{
	float altitude=0,Temp=0,press=0,ALT=0,TEM=0,PRE=0;
	float T[10],P[10],A[10];
	int pressure;
	char pre[5];
	u8 i;
	u8 tmp_buf[33]; 
  init();
	LCD_Init();
	IIC_Init();
	bmp180_Init();
	
	POINT_COLOR=BLUE;
	LCD_ShowString(40,100,200,16,16,"Tempreature:");	
//	LCD_ShowxNum(18+8*sizeof("Tempreature:"),100,bmp_data.temperature,3,16,0);
	LCD_ShowString(40,120,200,16,16,"Pressure:");
//	LCD_ShowxNum(18+8*sizeof("Pressure:"),120,bmp_data.pressure,5,16,0);
//	LCD_ShowString(40,140,200,16,16,"Height:");
//	LCD_ShowxNum(18+8*sizeof("Height:"),140,height,4,16,0);	
	
	while(1)
	{
//		for(i=0;i<5;i++)
//		{
//		Temp=Temperature_Convert();
		press=Pressure_Convert();
		pressure=press;
		delay_ms(50);
//			ALT+=get_altitude();
//			PRE+=Pressure_Convert();
//		}
		
//		Temp=TEM/5;
//    press=PRE/5;
//    altitude =ALT/10;
//		pressure=press/5;
		itoa(pressure,pre,10);
		USART_printf(USART1,pre);
		TEM=0;
		PRE=0;
//		ALT=0;
		
		POINT_COLOR=BRRED;
		LCD_ShowxNum(40+8*sizeof("Tempreature:"),100,Temp,4,16,0);
		LCD_ShowxNum(40+8*sizeof("Pressure:"),120,pressure,5,16,0);
//		LCD_ShowxNum(40+8*sizeof("Height:"),140,altitude,4,16,0);
		chage_char(Temp,3,press,6,tmp_buf);
	}	
}
		
void init(void)
{
	SystemInit();
	delay_init(72);
	NVIC_Configuration();
	GPIO_Configuration(); 
	USART1_Configuration();
	USART_ClearFlag(USART1,USART_FLAG_TC);
	USART2_Configuration();
	USART_ClearFlag(USART2,USART_FLAG_TC); 
	USART3_Configuration();
	USART_ClearFlag(USART3,USART_FLAG_TC); 
	//ADC_Configuration();
}
