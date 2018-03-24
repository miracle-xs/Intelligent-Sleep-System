
/*功能描述：MS5611大气压强模块数据采集计算 V1.0*/
/********************************************************************************    
  * @file    atmosphere.c
  * @author  pei4566
  * @version V1.0.2
  * @date    04-October-2015
  * @brief   pressure sensor SPI interface program source code, 
	*          the realization of temperature, pressure, collecting calculate altitude.
  *           
*********************************************************************************/

#include  "atmosphere.h"
#include "stm32f10x.h"
#include "USER_delay.h"
#include "math.h"

u16 setup,C1,C2,C3,C4,C5,C6,CRC_1;  //用于存放PROM中的8组数据
unsigned long D1_Pres,D2_Temp;      //用于存放温度和压力 
double OFF,SENS;                    //实际温度抵消、实际温度灵敏度
float dT,Temperature,Pressure;      //实际和参考温度之间的差异、温度值、压力值
float TEMP2,Aux,OFF2,SENS2;         //温度检验值

/*******************************************************************************
	大气压强模块用到的SPI1接口初始化
	函数名称：SPI1_Init
	输入参数：void
	输出参数：void
*******************************************************************************/
void SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); //使能SPI1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOB时钟
	
  GPIO_InitStructure.GPIO_Pin = ATMOSPHERE_NSS_PIN;    //PA4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //输出
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //100MHz
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         //上拉
  GPIO_Init(ATMOSPHERE_GPIO, &GPIO_InitStructure);     //初始化
 
  GPIO_InitStructure.GPIO_Pin = ATMOSPHERE_SCK_PIN|ATMOSPHERE_MISO_PIN|ATMOSPHERE_MOSI_PIN;//PA5~7复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                             //复用功能
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                           //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                       //100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                             //上拉
  GPIO_Init(ATMOSPHERE_GPIO, &GPIO_InitStructure);                                         //初始化   
	
//	GPIO_PinAFConfig(ATMOSPHERE_SCK_GPIO,GPIO_PinSource5,GPIO_AF_SPI1);   //PB5复用为 SPI1
//	GPIO_PinAFConfig(ATMOSPHERE_MISO_GPIO,GPIO_PinSource6,GPIO_AF_SPI1);  //PB6复用为 SPI1
//	GPIO_PinAFConfig(ATMOSPHERE_MOSI_GPIO,GPIO_PinSource7,GPIO_AF_SPI1);  //PB7复用为 SPI1
 
	//这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);  //复位SPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE); //停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                    //设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                //设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                      //串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                      //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                        //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//42M/128=328.125KHZ定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                          //CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);                                 //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE);   //使能SPI外设

	SPI1_ReadWriteByte(0xff);//启动传输		 	 
}   
/*******************************************************************************
  SPI1速度设置函数
	函数名称：SPI1_SetSpeed
	输入参数：SPI_BaudRatePrescaler
	输出参数：void
  SPI速度=fAPB2/分频系数
  @ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
  fAPB2时钟一般为84Mhz
*******************************************************************************/
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;                                             //位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	                             //设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE);                                          //使能SPI1
} 

/*******************************************************************************
	SPI1 读写一个字节
	函数名称：SPI1_ReadWriteByte
	输入参数：TxData
	输出参数：读取到的字节
*******************************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据	    
}

/*******************************************************************************
	SPI1写寄存器	
	函数名称：ATMOSPHERE_Write_Reg
	输入参数：SlaveAddress(从机地址)、REG_Address（指定的寄存器地址）
	输出参数：返回状态值
*******************************************************************************/
u8 ATMOSPHERE_Write_Reg(u8 REG_Address,u8 REG_data)
{
	u8 status;
	
	ATMOSPHERE_ENABLE;                        //使能大气压强模块
	status=SPI1_ReadWriteByte(REG_Address);   //发送写命令+寄存器号
	SPI1_ReadWriteByte(REG_data);             //写入寄存器值
	ATMOSPHERE_DISENABLE;                     //禁止大气压强模块
	return(status);                           //返回状态值
}

/*******************************************************************************
	MS5611模块复位函数
	函数名称：MS5611_RESET
	输入参数：void
	输出参数：void
*******************************************************************************/
void MS5611_RESET(void)
{
	 ATMOSPHERE_ENABLE;                         //使能大气压强模块
   SPI1_ReadWriteByte(CMD_MS5611_RESET);
	 ATMOSPHERE_DISENABLE;                      //禁止大气压强模块
}

/*******************************************************************************
	SPI1 读写两个字节
	函数名称：SPI1_ReadWrite_16bits
	输入参数：addr
	输出参数：读取到的16位数据
*******************************************************************************/
u16  SPI1_ReadWrite_16bits(unsigned char addr)
{
  unsigned char byteH,byteL;
  unsigned int return_value;
	
  ATMOSPHERE_ENABLE;//使能大气压强模块  
  SPI1_ReadWriteByte(addr);
  byteH = SPI1_ReadWriteByte(0);
  byteL = SPI1_ReadWriteByte(0);
  ATMOSPHERE_DISENABLE;//禁止大气压强模块
  return_value = ((unsigned int)byteH<<8) | (byteL);
  return(return_value);    
}

/*******************************************************************************
	从PROM读取出厂校准数据
	函数名称：MS5611_PROM_READ
	输入参数：void
	输出参数：void
*******************************************************************************/
void MS5611_PROM_READ(void)
{
  ATMOSPHERE_ENABLE;//使能大气压强模块 
  C1 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C1);
  C2 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C2);
  C3 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C3);
  C4 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C4);
  C5 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C5);
  C6 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C6);
  setup = SPI1_ReadWrite_16bits(CMD_MS5611_RESET);
  CRC_1 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_CRC); 
  ATMOSPHERE_DISENABLE;//禁止大气压强模块
}

/*******************************************************************************
	读取ADC结果
	函数名称：MS5611_SPI_read_ADC
	输入参数：void
	输出参数：返回24位的气压/温度值
*******************************************************************************/
long MS5611_SPI_read_ADC(void)
{
  unsigned char byteH,byteM,byteL;
  long return_value;
  
  ATMOSPHERE_ENABLE;//使能大气压强模块  
  SPI1_ReadWriteByte(MS5611_ADC);
  byteH = SPI1_ReadWriteByte(0);
  byteM = SPI1_ReadWriteByte(0);
  byteL = SPI1_ReadWriteByte(0);
  ATMOSPHERE_DISENABLE;//禁止大气压强模块  
  return_value = (((long)byteH)<<16) | (((long)byteM)<<8) | (byteL);
  return(return_value);
}
/*******************************************************************************
	通过MS5611模块得到气压值
	函数名称：MS5611_getPressure
	输入参数：气压分辨率系数
	输出参数：void
*******************************************************************************/
float MS5611_getPressure(unsigned char OSR_Pres)
{
	u8 temp;
	
	ATMOSPHERE_ENABLE;                                 //使能大气压强模块  
  temp = SPI1_ReadWriteByte(CMD_CONVERT_D1_OSR256);
	ATMOSPHERE_DISENABLE;                              //禁止大气压强模块
	delay_ms(10);                                      //必须加
  D1_Pres=MS5611_SPI_read_ADC();

  OFF=(unsigned long)C2*pow(2,17)+((unsigned long)C4*dT)/64;
  SENS=(unsigned long)C1*pow(2,16)+((unsigned long)C3*dT)/128;
  
  if(Temperature<2000)
  {
    // second order temperature compensation when under 20 degrees C
    TEMP2 = (dT*dT) / 0x80000000;
    Aux = Temperature*Temperature;
    OFF2 = 2.5*Aux;
    SENS2 = 1.25*Aux;
    Temperature = Temperature - TEMP2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2; 
  }
  Pressure=(D1_Pres*SENS/2097152-OFF)/32768;
	return Pressure;
}

/*******************************************************************************
	通过MS5611模块得到温度值
	函数名称：MS5611_getTemperature
	输入参数：温度分辨率系数
	输出参数：void
*******************************************************************************/
float MS5611_getTemperature(unsigned char OSR_Temp)    
{
	u8 temp = 0;
		
	ATMOSPHERE_ENABLE;                                  //使能大气压强模块  
  temp = SPI1_ReadWriteByte(CMD_CONVERT_D2_OSR4096);
	ATMOSPHERE_DISENABLE;                               //禁止大气压强模块
  delay_ms(10);                                       //此处延时必须加
  D2_Temp=MS5611_SPI_read_ADC();	
//  dT=D2_Temp - (((unsigned long)C5)<<8);
  dT=(float)D2_Temp - (((float)C5)*256);
//	Temperature=2000+dT*((unsigned long)C6)/8388608;
	Temperature=2000+dT*((float)C6)/8388608;
	return Temperature;
}

/*******************************************************************************
	根据大气压算出海拔高度 
	函数名称：get_altitude
	输入参数：void
	输出参数：海拔高度
*******************************************************************************/
float get_altitude(void)                             
{
  float tmp_float,Altitude;    
	
  tmp_float = (Pressure / 101325.0);
  tmp_float = pow(tmp_float, 0.190295);               
  Altitude = 44330 * (1.0 - tmp_float);
  return (Altitude);
}

/*end*/
