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
#include "ds18b20.h"

unsigned char RS232_REC_Flag1 = 0;								/*串口1接收数据标志*/
unsigned char RS232_REC_Count1 = 0;								/*串口1接收计数*/
unsigned char RS232_buff1[100];	   								/*串口1接收缓冲*/

unsigned char RS232_REC_Flag2 = 0;								/*串口2接收数据标志*/
unsigned char RS232_REC_Count2 = 0;								/*串口2接收计数*/
unsigned char RS232_buff2[100];	   								/*串口2接收缓冲*/

unsigned char RS232_REC_Flag3 = 0;								/*串口3接收数据标志*/
unsigned char RS232_REC_Count3 = 0;								/*串口3接收计数*/
unsigned char RS232_buff3[100];	   								/*串口3接收缓冲*/

unsigned char CrcH,CrcL;										/*CRC校验高低位*/
unsigned short Crc1,Crc2,Crch,Crcl;

unsigned char temp[5];

unsigned int value_0,value_1,value_2,value_3,value_4,heart,breath,turn;
char br[2],ht[3],tr[3],b0[4],h0[6],t0[6];
int i=0;
u8 t=0,flag1=0,count=0,flag=0;

void init(void);

 /************************************************************************
* 函数名：int main(void)										    
* 功能说明：主函数                                       
************************************************************************/

int main(void)
{
	unsigned char adx_buff[10]={0x01};
	unsigned char adc_buff[50];
	unsigned char i,a,b,c,d,j;
	unsigned char asca,ascb,ascc,ascd,asctemp;
	unsigned short result,adc[10];
	
/*
	自建wifi
	miracle123       0123456789
	端口5000
	*/
//	unsigned char AT[]="AT\r\n";
//  unsigned char CWMODE[]="AT+CWMODE=3\r\n";
//  unsigned char CWSAP[]="AT+CWSAP=\"miracle123\",\"0123456789\",11,3\r\n";
//  unsigned char CIPMUX[]="AT+CIPMUX=1\r\n";
//  unsigned char CIPAP[]="AT+CIPAP=\"192.168.4.1\"\r\n";
//  unsigned char CIPSERVER[]="AT+CIPSERVER=1,5000\r\n";
//  unsigned char CIPSEND1[]="AT+CIPSEND=3\r\n";
//	unsigned char CIPSEND2[]="AT+CIPSEND=9\r\n";
//	unsigned char CIPSEND3[]="AT+CIPSEND=12\r\n";
//  unsigned char MAS1[]="The massage send succeed!";
//	unsigned char space[]="\r\n";     
	
	/*
		连接wifi
		目标wifi:qianqian123   87234728
	*/
	unsigned char AT[]="AT\r\n";
	unsigned char RESTORE[]="AT+RESTORE\r\n";
	unsigned char CWMODE[]="AT+CWMODE=3\r\n";
//	unsigned char CWJAP[]="AT+CWJAP=\"qianqian123\",\"87234728\"\r\n";
	unsigned char CWJAP[]="AT+CWJAP=\"miracle\",\"1234567890\"\r\n";	
	unsigned char CIPSTART[]="AT+CIPSTART=\"TCP\",\"115.159.220.110\",2016\r\n";
//	unsigned char CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.155.1\",2016\r\n";
	unsigned char CIPSEND[]="AT+CIPSEND=0\r\n";
	unsigned char CIPSEND1[]="AT+CIPSEND=1\r\n";
	unsigned char CIPSEND2[]="AT+CIPSEND=2\r\n";
	unsigned char CIPSEND3[]="AT+CIPSEND=3\r\n";
	unsigned char CIPSEND4[]="AT+CIPSEND=4\r\n";
	unsigned char CIPSEND5[]="AT+CIPSEND=5\r\n";
	unsigned char CIPSEND6[]="AT+CIPSEND=6\r\n";
	unsigned char CIPSEND7[]="AT+CIPSEND=7\r\n";
	unsigned char CIPSEND8[]="AT+CIPSEND=8\r\n";
	unsigned char CIPSEND9[]="AT+CIPSEND=9\r\n";
	unsigned char CIPSEND10[]="AT+CIPSEND=10\r\n";
	unsigned char CIPSEND11[]="AT+CIPSEND=11\r\n";
	unsigned char CIPSEND12[]="AT+CIPSEND=12\r\n";
	unsigned char CIPSEND13[]="AT+CIPSEND=13\r\n";
	unsigned char CIPSEND14[]="AT+CIPSEND=14\r\n";
	unsigned char CIPSEND15[]="AT+CIPSEND=15\r\n";
	unsigned char CIPSEND16[]="AT+CIPSEND=16\r\n";
	unsigned char CIPSEND17[]="AT+CIPSEND=17\r\n";
	unsigned char CIPSEND18[]="AT+CIPSEND=18\r\n";
	unsigned char CIPSEND19[]="AT+CIPSEND=19\r\n";
	unsigned char CIPSEND20[]="AT+CIPSEND=20\r\n";
	unsigned char CIPSEND21[]="AT+CIPSEND=21\r\n";
	unsigned char CIPSEND22[]="AT+CIPSEND=22\r\n";
	unsigned char CIPSEND23[]="AT+CIPSEND=23\r\n";
	unsigned char CIPSEND24[]="AT+CIPSEND=24\r\n";
	unsigned char CIPSEND25[]="AT+CIPSEND=25\r\n";
	unsigned char CIPSEND26[]="AT+CIPSEND=26\r\n";
	unsigned char CIPSEND27[]="AT+CIPSEND=27\r\n";
	unsigned char CIPSEND28[]="AT+CIPSEND=28\r\n";
	unsigned char CIPSEND29[]="AT+CIPSEND=29\r\n";
	unsigned char CIPSEND30[]="AT+CIPSEND=30\r\n";
	unsigned char CIPSEND31[]="AT+CIPSEND=31\r\n";
	unsigned char MAS1[]="The massage send succeed!";
	unsigned char space[]="\r\n";  
	
	unsigned char a1[]="a1\r\n";
	unsigned char a2[]="a2\r\n";
	unsigned char a3[]="a3\r\n";
	unsigned char a5[]="a5\r\n";
	char h[]="h-";
	char b1[]="b-";
	char t[]="t-";
	char zr[]="0";
	unsigned char gang[]="-";
	unsigned char aaa[100];
	unsigned char data[40];
	unsigned char Recv[2];

	turn=0;
	init();
	delay_ms(1000);
	USART_printf(USART2,RESTORE);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	USART_printf(USART2,AT);
	delay_ms(1000);delay_ms(1000);
	USART_printf(USART2,CWMODE);
	delay_ms(1000);delay_ms(1000);	
	USART_printf(USART2,CWJAP);
	delay_ms(1000);delay_ms(1000);
	delay_ms(1000);delay_ms(1000);
	delay_ms(1000);delay_ms(1000);
	delay_ms(1000);delay_ms(1000);
	USART_printf(USART2,CIPSTART);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
	while(1)
	{
		/*蜂鸣器*/
//		if(temp)
//			GPIO_SetBits(GPIOA,GPIO_Pin_3);
//		else
//			GPIO_ResetBits(GPIOA,GPIO_Pin_3);

//			gettemp();
//		USART_printf(USART2,temp);
//		delay_ms(100);
		while(RS232_REC_Flag3)											  
		{
			int count=0;	
			delay_ms(40);
			RS232_REC_Flag3 = 0;		
			for(count=0;count<RS232_REC_Count3;count++)
			{
					data[count]=RS232_buff3[count];
			}
			RS232_REC_Count3=0;
			switch(count){
				case(10):USART_printf(USART2,CIPSEND8);break;
				case(11):USART_printf(USART2,CIPSEND9);break;
				case(12):USART_printf(USART2,CIPSEND10);break;
				case(13):USART_printf(USART2,CIPSEND11);break;
				case(14):USART_printf(USART2,CIPSEND12);break;
				case(15):USART_printf(USART2,CIPSEND13);break;
				case(16):USART_printf(USART2,CIPSEND14);break;
				case(17):USART_printf(USART2,CIPSEND15);break;
				case(18):USART_printf(USART2,CIPSEND16);break;
				case(19):USART_printf(USART2,CIPSEND17);break;
				case(20):USART_printf(USART2,CIPSEND18);break;
				case(21):USART_printf(USART2,CIPSEND19);break;
				case(22):USART_printf(USART2,CIPSEND20);break;
				case(23):USART_printf(USART2,CIPSEND21);break;
				case(24):USART_printf(USART2,CIPSEND22);break;
				case(25):USART_printf(USART2,CIPSEND23);break;
				case(26):USART_printf(USART2,CIPSEND24);break;
				case(27):USART_printf(USART2,CIPSEND25);break;
				case(28):USART_printf(USART2,CIPSEND26);break;
				case(29):USART_printf(USART2,CIPSEND27);break;
				case(30):USART_printf(USART2,CIPSEND28);break;
				case(31):USART_printf(USART2,CIPSEND29);break;
				case(32):USART_printf(USART2,CIPSEND30);break;
				case(33):USART_printf(USART2,CIPSEND31);break;
				default:
					break;
			}
			delay_ms(40);
			RS232_Send2_Data(data,count-2);
			
	//		USART_printf(USART2,data);
	//		USART_printf(USART1,"OK");
			
			USART_printf(USART2,space);
	//		delay_ms(40);
	//		RS232_REC_Count3=0;
		}		
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
