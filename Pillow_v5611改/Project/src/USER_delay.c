
#include "USER_delay.h"

//当AHB的分频系数为1的时候SYSTEM的时钟和HCLK的一样
//防止延时不准确,采用do while结构!
static u8  fac_us = 0;//us延时倍乘数
static u16 fac_ms = 0;//ms延时倍乘数
//初始化延迟函数
//SysTick的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟，此处直接带入MHZ的单位，如72
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL &= 0xfffffffb;             //bit2清空,选择外部时钟  HCLK/8 = SYSCLK/8  
	fac_us = SYSCLK/8;                       //计数初值为SYSCLK/8，计数周期为 8/SYSCLK（us），相乘为1US	    
	fac_ms = (u16)fac_us*1000;               //计数1000个微秒为一毫秒
}								    
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=（2^24）*8/（SYSCLK*1000）= 1864MS（在72MHZ的条件下）
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD = (u32)nms*fac_ms;       //时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;                   //写当前计数值寄存器使其清0
	SysTick->CTRL = 0x01 ;                 //使能计数  
	do
	{
		temp = SysTick->CTRL;                //不停的读取控制及状态寄存器
	}
	while(temp&0x01&&!(temp&(1<<16)));     //如果使能了并且第16位COUNTFLAG为0则死循环   
	SysTick->CTRL = 0x00;                  //计数完毕则关闭计数器
	SysTick->VAL = 0X00;                   //写当前数值寄存器使之清空  	    
}   
//延时nus
//nus为要延时的us数.
//nus < 1864135 (以72MHZ计算)		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD = nus*fac_us;          //时间加载	  		 
	SysTick->VAL = 0x00;                 //清空计数器
	SysTick->CTRL = 0x01 ;               //开始倒数 	 
	do
	{
		temp = SysTick->CTRL;	             //不断读取控制及状态寄存器
	}
	while(temp&0x01&&!(temp&(1<<16)));   //如果使能了并且第16位COUNTFLAG为0则死循环      
	SysTick->CTRL = 0x00;                //关闭计数器
	SysTick->VAL  = 0X00;                //清空计数器	 
}
/**********************************************************************
SysTick是一个24位的倒计数定时器,当计到0时，将从RELOAD寄存器中自动重装载定
时初值.只要不把它在SysTick控制及状态寄存器中的使能位清除，就永不停息。
 
  SysTick控制及状态寄存器SysTick->CTRL
	  第16位 COUNTFLAG  如果在上次读取本寄存器后，SysTick已经数到了
	                    0，则该位为1。如果读取该位，该位将自动清零
	  第2位  CLKSOURCE  值为0表示选择外部时钟源(STCLK)
	                    值为1表示选择内核时钟(FCLK) 
	  第1位  TICKINT    置1的时候SysTick倒数到0时产生SysTick异常请求（只有请求了才会有响应） 
	                    置0的时候数到0时无动作 
	  第0位  ENABLE     SysTick定时器的使能位 

  SysTick重装载数值寄存器SysTick->LOAD
      23:0   RELOAD      当倒数至零时，将被重装载的值,注意是24位的！

  SysTick当前数值寄存器SysTick->VAL
      23:0  CURRENT      读取时返回当前倒计数的值，写它则使之清零，
                         同时还会清除在 SysTick 控制及状态寄存器中的COUNTFLAG标志

  SysTick校准数值寄存器SysTick->CALIB
      31    NOREF      1=没有外部参考时钟（STCLK不可用） 
                       0=外部参考时钟可用 
      30    SKEW       1=校准值不是准确的10ms 
                       0=校准值是准确的10ms 
      23:0  TENMS      10ms的时间内倒计数的格数。芯片设计者应该通
                       过 Cortex‐M3 的输入信号提供该数值。若该值读
                       回零，则表示无法使用校准功能

RCC通过AHB时钟(HCLK)8分频后作为Cortex系统定时器(SysTick)的外部时钟。通过对
SysTick控制与状态寄存器的设置，可选择FCLK时钟或Cortex/8(HCLK/8)时钟作为SysTick
时钟。
通过检查校准寄存器的位[31](NOREF)，可以判定是否有可用的外部时钟源，
而芯片厂商则必须把该引线连接至正确的电平。 
 当SysTick定时器从1计到0时，它将把COUNTFLAG位置位；而下述方法可以清零之 
  1、读取SysTick控制及状态寄存器（STCSR） 
  2、往SysTick当前值寄存器（STCVR）中写任何数据 
 
**********************************************************************/


































