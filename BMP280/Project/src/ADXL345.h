#ifndef __ADXL345_H
#define __ADXL345_H

//#include "delay.h"
//#include "stm32f10x.h"
//#include "stm32f10x_gpio.h"

#define SCL GPIO_Pin_12
#define SDA GPIO_Pin_13
#define GPIO GPIOB
#define RCC_APB2Periph_GPIO RCC_APB2Periph_GPIOB
#define uchar unsigned char
	
#define SDAH GPIO_SetBits(GPIO,SDA)
#define SDAL GPIO_ResetBits(GPIO,SDA)
#define SCLH GPIO_SetBits(GPIO,SCL)
#define SCLL GPIO_ResetBits(GPIO,SCL)
#define SDA_DATA GPIO_ReadInputDataBit(GPIO,SDA)

#define	SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A
#define  DEVID  0x00 
#define THRESH_TAP 0x1D 
#define OFSX  0x1E 
#define OFSY 0x1F 
#define OFSZ 0x20 
#define DUR 0x21 
#define Latent 0x22 
#define Window 0x23 
#define THRESH_ACT 0x24 
#define THRESH_INACT 0x25 
#define TIME_INACT 0x26 
#define ACT_INACT_CTL 0x27 
#define THRESH_FF 0x28 
#define TIME_FF 0x29  
#define TAP_AXES 0x2A 
#define ACT_TAP_STATUS 0x2B 
#define BW_RATE 0x2C 
#define POWER_CTL 0x2D 
#define INT_ENABLE 0x2E 
#define INT_MAP 0x2F 
#define INT_SOURCE 0x30 
#define DATA_FORMAT 0x31 
#define DATAX0 0x32 
#define DATAX1 0x33 
#define DATAY0 0x34 
#define DATAY1 0x35 
#define DATAZ0 0x36 
#define DATAZ1 0x37 
#define FIFO_CTL 0x38 



typedef struct
{
	u16 x;
	u16 y;
	u16 z;
	double xangle;
	double yangle;
}GVALUE;



void ADXL345_GPIO_CLOCK(void);
void GPIO_DATAIN(void);
void GPIO_DATAOUT(void);

void ADXL345_Start(void);
void ADXL345_Stop(void);
void ADXL345_SendACK(unsigned char ack_flag);
unsigned char ADXL345_RecvACK(void);
void ADXL345_SendByte(unsigned char dat);
unsigned char ADXL345_RecvByte(void);
void ADXL345_ReadPage(void);
void ADXL345_WritePage(void);
void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //单个写入数据
uchar Single_Read_ADXL345(uchar REG_Address);                   //单个读取内部寄存器数据
void  Multiple_Read_ADXL345(unsigned char *BUF);                                  //连续的读取内部寄存器数据
void Init_ADXL345(void);
void cack(void);

void get_x(unsigned char *buff1,unsigned char *x_buff);
void get_y(unsigned char *buff1,unsigned char *y_buff);
void get_z(unsigned char *buff1,unsigned char *z_buff);






#endif
