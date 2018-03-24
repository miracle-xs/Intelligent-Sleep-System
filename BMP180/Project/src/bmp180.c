#include "myiic.h" 
#include "USER_delay.h"
#include "bmp180.h"
//BMP180控制程序
//
//
/***********************/
//****校准系数*****//
short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;
/***********************/
//void IIC_Init(void);                //初始化IIC的IO口				 
//void IIC_Start(void);				//发送IIC开始信号
//void IIC_Stop(void);	  			//发送IIC停止信号
//void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
//u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
//u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
//void IIC_Ack(void);					//IIC发送ACK信号
//void IIC_NAck(void);				//IIC不发送ACK信号

//void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	 

/**********************************/
//IMP180数据读取函数
//输入参数：寄存器地址
/**********************************/
short Multiple_read(u8 ST_Address)
{   
	u8 msb, lsb;
	short _data;
    IIC_Start();                          //起始信号
    IIC_Send_Byte(bmp180_SlaveAddress);    //发送设备地址+写信号
    IIC_Send_Byte(ST_Address);             //发送储存单元地址
    IIC_Start();                         //起始信号
    IIC_Send_Byte(bmp180_SlaveAddress+1);         //发送设备地址+读信号
	//	IIC_Wait_Ack();
		
    msb = IIC_Read_Byte(1);                 //BUF[0]存储
                                            //回应ACK
    lsb = IIC_Read_Byte(0);     
																						//最后一个数据需要回应NOACK
    IIC_Stop();                           //停止信号
    delay_ms(5);                          //延时5ms
    _data = msb << 8;
	_data |= lsb;	
	return _data;
}
/**********************************/
//温度读取函数
//输入参数：NONE
/**********************************/
long bmp180ReadTemp(void)
{
	
    IIC_Start();                          //起始信号
    IIC_Send_Byte(bmp180_SlaveAddress);    //发送设备地址+写信号
//				LED1 = !LED1 ;
    IIC_Send_Byte(0xF4);	          // write register address
    IIC_Send_Byte(0x2E);       	// write register data for temp
    IIC_Stop();                   //停止信号
	  delay_ms(10);	// max time is 4.5ms
	  return (long) Multiple_read(0xF6);
}
/**********************************/
//气压读取函数
//输入参数：NONE
/**********************************/
long bmp180ReadPressure(void)
{
	long pressure = 0;

    IIC_Start();                          //起始信号
    IIC_Send_Byte(bmp180_SlaveAddress);    //发送设备地址+写信号
    IIC_Send_Byte(0xF4);	          // write register address
    IIC_Send_Byte(0xF4);//OSS 精度设置（这里为最高精度） // write register data for pressure
    IIC_Stop();                    //停止信号
	  delay_ms(80);    	                  // max time is 4.5ms
	  pressure = Multiple_read(0xF6);
	  pressure &= 0x0000FFFF;
	  return pressure;	
	//return (long) bmp180ReadShort(0xF6);
}
/**********************************/
//气压计初始化函数 
//输入参数：NONE
/**********************************/
void bmp180_Init()
{
	ac1 = Multiple_read(0xAA);
	ac2 = Multiple_read(0xAC);
	ac3 = Multiple_read(0xAE);
	ac4 = Multiple_read(0xB0);
	ac5 = Multiple_read(0xB2);
	ac6 = Multiple_read(0xB4);
	b1 =  Multiple_read(0xB6);
	b2 =  Multiple_read(0xB8);
	mb =  Multiple_read(0xBA);
	mc =  Multiple_read(0xBC);
	md =  Multiple_read(0xBE);
}
/**********************************/
//温度气压修正函数
//输入参数：NONE
/**********************************/
long Temperature_Convert()
{
  long ut;
	long x1, x2, b5;
	long  temperature;
	
	//ut = bmp180ReadTemp();
	ut = bmp180ReadTemp();	   // 读取温度
	
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	temperature = (b5 + 8) >> 4;
	return temperature; 
}

long Pressure_Convert()
{
  long ut;
	long up;
	long x1, x2, b5, b6, x3, b3, p;
	unsigned long b4, b7;
//	long  temperature;
	long  pressure;
	
//	ut = bmp180ReadTemp();
	ut = bmp180ReadTemp();	   // 读取温度
//	up = bmp180ReadPressure();
	up = bmp180ReadPressure();  // 读取气压
	
	x1 = ((long)ut - ac6) * ac5 >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
//	 temperature = (b5 + 8) >> 4;

	 //*************

	b6 = b5 - 4000;
	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
	x2 = ac2 * b6 >> 11;
	x3 = x1 + x2;
	b3 = (((long)ac1 * 4 + x3) + 2)/4;
	x1 = ac3 * b6 >> 13;
	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
	if( b7 < 0x80000000)
	     p = (b7 * 2) / b4 ;
           else  
		    p = (b7 / b4) * 2;
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	 pressure = p + ((x1 + x2 + 3791) >> 4);
	 return pressure; 
}

//void bmp180Convert()
//{
//	long ut;
//	long up;
//	long x1, x2, b5, b6, x3, b3, p;
//	unsigned long b4, b7;
//	long  temperature;
//	long  pressure;
//	
//	ut = bmp180ReadTemp();
//	ut = bmp180ReadTemp();	   // 读取温度
//	up = bmp180ReadPressure();
//	up = bmp180ReadPressure();  // 读取气压
//	
//	x1 = ((long)ut - ac6) * ac5 >> 15;
//	x2 = ((long) mc << 11) / (x1 + md);
//	b5 = x1 + x2;
//	 temperature = (b5 + 8) >> 4;

//	 //*************

//	
//	b6 = b5 - 4000;
//	x1 = (b2 * (b6 * b6 >> 12)) >> 11;
//	x2 = ac2 * b6 >> 11;
//	x3 = x1 + x2;
//	b3 = (((long)ac1 * 4 + x3) + 2)/4;
//	x1 = ac3 * b6 >> 13;
//	x2 = (b1 * (b6 * b6 >> 12)) >> 16;
//	x3 = ((x1 + x2) + 2) >> 2;
//	b4 = (ac4 * (unsigned long) (x3 + 32768)) >> 15;
//	b7 = ((unsigned long) up - b3) * (50000 >> OSS);
//	if( b7 < 0x80000000)
//	     p = (b7 * 2) / b4 ;
//           else  
//		    p = (b7 / b4) * 2;
//	x1 = (p >> 8) * (p >> 8);
//	x1 = (x1 * 3038) >> 16;
//	x2 = (-7357 * p) >> 16;
//	 pressure = p + ((x1 + x2 + 3791) >> 4);
//}
