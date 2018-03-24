/*
	改进了v5611在温度低于20度情况下异常的情况
*/
#include <stdio.h>
#include <math.h>
#include "stm32f10x_spi.h"
#include "SCI.H"
#include "USER_delay.h"
#include "stm32f10x_adc.h"
#include "string.h"
#include "lcd.h"
#include "atmosphere.h"
//#include <avr/io.h>


#define TRUE 1
#define FALSE 0
//#define F_CPU 4000000UL // 4 MHz XTAL
//#define CMD_RESET 0x1E // ADC reset command
//#define CMD_ADC_READ 0x00 // ADC read command
//#define CMD_ADC_CONV 0x40 // ADC conversion command
//#define CMD_ADC_D1 0x00 // ADC D1 conversion
//#define CMD_ADC_D2 0x10 // ADC D2 conversion
//#define CMD_ADC_256 0x00 // ADC OSR=256
//#define CMD_ADC_512 0x02 // ADC OSR=512
//#define CMD_ADC_1024 0x04 // ADC OSR=1024
//#define CMD_ADC_2048 0x06 // ADC OSR=2056
//#define CMD_ADC_4096 0x08 // ADC OSR=4096
//

#define CMD_PROM_RD 0xA0 // Prom read command

//********************************************************
//! @brief send 8 bit using SPI hardware interface
//!
//! @return 0
//********************************************************
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

//********************************************************
//! @brief send reset sequence
//!
//! @return 0
//********************************************************

//********************************************************
//! @brief main program
//!
//! @return 0
//********************************************************


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


int main (void)
{
	float altitude=0,Temp=0,ALT=0,TEM=0,PRE=0;
	float T[10],P[10],A[10];
	int press=0;
	char pre[5];
	u8 i;
	u8 tmp_buf[33]; 
  init();
	LCD_Init();
	SPI1_Init();
	SPI1_SetSpeed(SPI_BaudRatePrescaler_64);
	MS5611_RESET();
	MS5611_PROM_READ();
	
	POINT_COLOR=BLUE;
	LCD_ShowString(40,100,200,16,16,"Tempreature:");	
//	LCD_ShowxNum(18+8*sizeof("Tempreature:"),100,bmp_data.temperature,3,16,0);
	LCD_ShowString(40,120,200,16,16,"Pressure:");
//	LCD_ShowxNum(18+8*sizeof("Pressure:"),120,bmp_data.pressure,5,16,0);
	LCD_ShowString(40,140,200,16,16,"Height:");
//	LCD_ShowxNum(18+8*sizeof("Height:"),140,height,4,16,0);	
	
	while(1)
	{
		for(i=0;i<10;i++)
		{
			TEM+=MS5611_getTemperature(CMD_CONVERT_D2_OSR4096);
			PRE+=MS5611_getPressure(CMD_CONVERT_D1_OSR4096);
			ALT+=get_altitude();
		}
		Temp=TEM/10;
    press=PRE/10;
    altitude =ALT/10;
		itoa(press,pre,10);
		USART_printf(USART1,pre);
		TEM=0;
		PRE=0;
		ALT=0;
		
		POINT_COLOR=BRRED;
		LCD_ShowxNum(40+8*sizeof("Tempreature:"),100,Temp,4,16,0);
		LCD_ShowxNum(40+8*sizeof("Pressure:"),120,press,5,16,0);
		LCD_ShowxNum(40+8*sizeof("Height:"),140,altitude,4,16,0);
		chage_char(Temp,3,press,6,tmp_buf);
	}	
}

