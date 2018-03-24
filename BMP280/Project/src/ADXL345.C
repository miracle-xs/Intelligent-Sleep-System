#include "stm32f10x.h"
#include "ADXL345.h"
#include "USER_delay.h"
#include "SCI.H"
#include "stm32f10x_gpio.h"

/*PB12--SCL,PB13--SDA*/
void ADXL345_GPIO_CLOCK(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* Enable the GPIO Clock */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIO , ENABLE); 						 
				
		GPIO_InitStructure.GPIO_Pin = SCL ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIO, &GPIO_InitStructure);
}

void GPIO_DATAIN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable the GPIO Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIO , ENABLE);
	GPIO_InitStructure.GPIO_Pin = SDA ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIO, &GPIO_InitStructure);
}

void GPIO_DATAOUT(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		/* Enable the GPIO Clock */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIO , ENABLE); 						 
				
		GPIO_InitStructure.GPIO_Pin = SDA ;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIO, &GPIO_InitStructure);
}

/**************************************
起始信号  SDA 1--0
**************************************/
void ADXL345_Start(void)
{
	GPIO_DATAOUT();
	SDAH;
	delay_us(10);
	SCLH;
	delay_us(10);
	SDAL;
	delay_us(10);
	SCLL;
}

/**************************************
停止信号   SDA 0--1
**************************************/
void ADXL345_Stop(void)
{
	GPIO_DATAOUT();
	SDAL;
	delay_us(10);
	SCLH;
	delay_us(10);
	SDAH;
	delay_us(10);
	SCLL;
	//delay_us(10);
}	
/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(unsigned char ack_flag)
{
	SCLL;
	GPIO_DATAOUT();
	if(ack_flag)
	{
		SDAH;
		delay_us(5);
		SCLH;
		delay_us(5);
		SCLL;
		SDAL;
	}
		
	else  
	{
		SDAL;
		delay_us(5);
		SCLH;
		delay_us(5);
		SCLL;
		SDAH;
	}
}
/**************************************
接收应答信号
**************************************/
unsigned char ADXL345_RecvACK(void)
{
	unsigned char CY;
	GPIO_DATAIN();
	SDAH;
	delay_us(5);
	SCLH;
	while(GPIO_ReadInputDataBit(GPIO,SDA))
	{
		CY++;
		if(CY>250)
		{
			ADXL345_Stop();
			return 1;
		}
	}
	SCLL;
	return 0;
}

/**************************************
向IIC总线发送一个字节数据
**************************************/
void ADXL345_SendByte(unsigned char dat)
{
	uchar i;
	GPIO_DATAOUT();
	SCLL;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
			SDAH;
		else SDAL;
		delay_us(5);
		SCLH;
		delay_us(5);
		SCLL;
		delay_us(5);
		dat<<=1;
	}
	ADXL345_RecvACK();
}
/**************************************
从IIC总线接收一个字节数据
**************************************/
unsigned char ADXL345_RecvByte(void)
{
	uchar i,data=0;
	GPIO_DATAIN();

	for(i=0;i<8;i++)
	{
		SCLL;
		delay_us(5);
		SCLH;
		data<<=1;
		delay_us(1);
		SCLH;
		if(SDA_DATA)
			data|=0x01;
		else data&=0xfe;
	}
	if(ADXL345_RecvACK())
		ADXL345_SendACK(0);
	else
		ADXL345_SendACK(1);
	return data;
}

/**************************************
单个字节写入
**************************************/
void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    ADXL345_Start();                  //起始信号
    ADXL345_SendByte(SlaveAddress);   //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    ADXL345_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    ADXL345_Stop();                   //发送停止信号
}
/**************************************
单个字节读取
**************************************/
uchar Single_Read_ADXL345(uchar REG_Address)
{  	
		uchar REG_data;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    ADXL345_SendByte(REG_Address);            //发送存储单元地址，从0开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=ADXL345_RecvByte();              //读出寄存器数据
		ADXL345_SendACK(1);   
		ADXL345_Stop();                           //停止信号
    return REG_data; 
}

/**************************************
连续读出ADXL345内部加速度数据，地址范围0x32~0x37
**************************************/
void Multiple_read_ADXL345(unsigned char *BUF)
{  
		uchar i;
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress);           //发送设备地址+写信号
    ADXL345_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
    ADXL345_Start();                          //起始信号
    ADXL345_SendByte(SlaveAddress+1);         //发送设备地址+读信号
		for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = ADXL345_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)
        {
           ADXL345_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //回应ACK
        }
   }
    ADXL345_Stop();                          //停止信号
    delay_ms(5);
}

//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345(void)
{
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Single_Write_ADXL345(0x2C,0x08);   //速率设定为12.5 参考pdf13页
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
   Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x20,0x7f);   //Z 偏移量 根据测试传感器的状态写入pdf29页
	
//	Iic_Init();
//	I2C_WriteByte(0xA6,BW_RATE,0x08);   //速率设定为12.5 参考pdf13页
//	I2C_WriteByte(0xA6,POWER_CTL,0x08);   //电源工作方式设置
//	I2C_WriteByte(0xA6,DATA_FORMAT,0x0B);	
//	I2C_WriteByte(0xA6,INT_ENABLE,0X80);	
}


void get_x(unsigned char *buff1,unsigned char *x_buff)
{
	int data;
	float temp;
	data=(buff1[1]<<8)+buff1[0];

	if(data<0)
	{
		data=-data;
		x_buff[0]='-';
	}
	else x_buff[0]='+';
	temp= (float)data*3.9;
	data=temp;
	
	x_buff[1]=data/1000 +0x30;
	//x_buff[1]=0+0x30;
	data=data%1000;
	x_buff[2]='.';
	x_buff[3]=data/100+0x30;
	data%=100;
	x_buff[4]=data/10+0x30;
	data%=10;
	x_buff[5]=data+0x30;
	x_buff[6]='\0';
	
}
void get_z(unsigned char *buff1,unsigned char *z_buff)
{
	int data;
	float temp;
	data=(buff1[5]<<8)+buff1[4];
//	data+=0x30;
//	USART_printf(USART2,data);
	if(data<0)
	{
		data=-data;
		z_buff[0]='-';
	}
	else z_buff[0]='+';
		temp= (float)data*3.9;
	data=temp;
	z_buff[1]=data/1000 +0x30;
	data%=1000;
	z_buff[2]='.';
	z_buff[3]=data/100+0x30;
	data%=100;
	z_buff[4]=data/10+0x30;
	data%=10;
	z_buff[5]=data+0x30;
	z_buff[6]='\0';
}
void get_y(unsigned char *buff1,unsigned char *y_buff)
{
	int data;
	float temp;
	data=(buff1[3]<<8)+buff1[2];

	if(data<0)
	{
		data=-data;
		y_buff[0]='-';
	}
	else y_buff[0]='+';
		temp= (float)data*3.9;
	data=temp;
//	test[0]=data/1000;
//	RS232_Send_Data(test,1);
	y_buff[1]=data/1000 +0x30;
	data%=1000;
	y_buff[2]='.';
	y_buff[3]=data/100+0x30;
	data%=100;
	y_buff[4]=data/10+0x30;
	data%=10;
	y_buff[5]=data+0x30;
	y_buff[6]='\0';
}


