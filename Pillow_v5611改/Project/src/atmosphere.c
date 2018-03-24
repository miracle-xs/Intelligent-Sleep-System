
/*����������MS5611����ѹǿģ�����ݲɼ����� V1.0*/
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

u16 setup,C1,C2,C3,C4,C5,C6,CRC_1;  //���ڴ��PROM�е�8������
unsigned long D1_Pres,D2_Temp;      //���ڴ���¶Ⱥ�ѹ�� 
double OFF,SENS;                    //ʵ���¶ȵ�����ʵ���¶�������
float dT,Temperature,Pressure;      //ʵ�ʺͲο��¶�֮��Ĳ��졢�¶�ֵ��ѹ��ֵ
float TEMP2,Aux,OFF2,SENS2;         //�¶ȼ���ֵ

/*******************************************************************************
	����ѹǿģ���õ���SPI1�ӿڳ�ʼ��
	�������ƣ�SPI1_Init
	���������void
	���������void
*******************************************************************************/
void SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); //ʹ��SPI1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
	
  GPIO_InitStructure.GPIO_Pin = ATMOSPHERE_NSS_PIN;    //PA4
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //���
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //100MHz
 // GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         //����
  GPIO_Init(ATMOSPHERE_GPIO, &GPIO_InitStructure);     //��ʼ��
 
  GPIO_InitStructure.GPIO_Pin = ATMOSPHERE_SCK_PIN|ATMOSPHERE_MISO_PIN|ATMOSPHERE_MOSI_PIN;//PA5~7���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                             //���ù���
 // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                                           //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                       //100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                                             //����
  GPIO_Init(ATMOSPHERE_GPIO, &GPIO_InitStructure);                                         //��ʼ��   
	
//	GPIO_PinAFConfig(ATMOSPHERE_SCK_GPIO,GPIO_PinSource5,GPIO_AF_SPI1);   //PB5����Ϊ SPI1
//	GPIO_PinAFConfig(ATMOSPHERE_MISO_GPIO,GPIO_PinSource6,GPIO_AF_SPI1);  //PB6����Ϊ SPI1
//	GPIO_PinAFConfig(ATMOSPHERE_MOSI_GPIO,GPIO_PinSource7,GPIO_AF_SPI1);  //PB7����Ϊ SPI1
 
	//����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);  //��λSPI1
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE); //ֹͣ��λSPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                    //����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		                      //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                      //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                        //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;//42M/128=328.125KHZ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	                          //CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);                                 //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI1, ENABLE);   //ʹ��SPI����

	SPI1_ReadWriteByte(0xff);//��������		 	 
}   
/*******************************************************************************
  SPI1�ٶ����ú���
	�������ƣ�SPI1_SetSpeed
	���������SPI_BaudRatePrescaler
	���������void
  SPI�ٶ�=fAPB2/��Ƶϵ��
  @ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
  fAPB2ʱ��һ��Ϊ84Mhz
*******************************************************************************/
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI1->CR1&=0XFFC7;                                             //λ3-5���㣬�������ò�����
	SPI1->CR1|=SPI_BaudRatePrescaler;	                             //����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE);                                          //ʹ��SPI1
} 

/*******************************************************************************
	SPI1 ��дһ���ֽ�
	�������ƣ�SPI1_ReadWriteByte
	���������TxData
	�����������ȡ�����ֽ�
*******************************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{		 			 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	    
}

/*******************************************************************************
	SPI1д�Ĵ���	
	�������ƣ�ATMOSPHERE_Write_Reg
	���������SlaveAddress(�ӻ���ַ)��REG_Address��ָ���ļĴ�����ַ��
	�������������״ֵ̬
*******************************************************************************/
u8 ATMOSPHERE_Write_Reg(u8 REG_Address,u8 REG_data)
{
	u8 status;
	
	ATMOSPHERE_ENABLE;                        //ʹ�ܴ���ѹǿģ��
	status=SPI1_ReadWriteByte(REG_Address);   //����д����+�Ĵ�����
	SPI1_ReadWriteByte(REG_data);             //д��Ĵ���ֵ
	ATMOSPHERE_DISENABLE;                     //��ֹ����ѹǿģ��
	return(status);                           //����״ֵ̬
}

/*******************************************************************************
	MS5611ģ�鸴λ����
	�������ƣ�MS5611_RESET
	���������void
	���������void
*******************************************************************************/
void MS5611_RESET(void)
{
	 ATMOSPHERE_ENABLE;                         //ʹ�ܴ���ѹǿģ��
   SPI1_ReadWriteByte(CMD_MS5611_RESET);
	 ATMOSPHERE_DISENABLE;                      //��ֹ����ѹǿģ��
}

/*******************************************************************************
	SPI1 ��д�����ֽ�
	�������ƣ�SPI1_ReadWrite_16bits
	���������addr
	�����������ȡ����16λ����
*******************************************************************************/
u16  SPI1_ReadWrite_16bits(unsigned char addr)
{
  unsigned char byteH,byteL;
  unsigned int return_value;
	
  ATMOSPHERE_ENABLE;//ʹ�ܴ���ѹǿģ��  
  SPI1_ReadWriteByte(addr);
  byteH = SPI1_ReadWriteByte(0);
  byteL = SPI1_ReadWriteByte(0);
  ATMOSPHERE_DISENABLE;//��ֹ����ѹǿģ��
  return_value = ((unsigned int)byteH<<8) | (byteL);
  return(return_value);    
}

/*******************************************************************************
	��PROM��ȡ����У׼����
	�������ƣ�MS5611_PROM_READ
	���������void
	���������void
*******************************************************************************/
void MS5611_PROM_READ(void)
{
  ATMOSPHERE_ENABLE;//ʹ�ܴ���ѹǿģ�� 
  C1 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C1);
  C2 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C2);
  C3 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C3);
  C4 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C4);
  C5 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C5);
  C6 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_C6);
  setup = SPI1_ReadWrite_16bits(CMD_MS5611_RESET);
  CRC_1 = SPI1_ReadWrite_16bits(CMD_MS5611_PROM_CRC); 
  ATMOSPHERE_DISENABLE;//��ֹ����ѹǿģ��
}

/*******************************************************************************
	��ȡADC���
	�������ƣ�MS5611_SPI_read_ADC
	���������void
	�������������24λ����ѹ/�¶�ֵ
*******************************************************************************/
long MS5611_SPI_read_ADC(void)
{
  unsigned char byteH,byteM,byteL;
  long return_value;
  
  ATMOSPHERE_ENABLE;//ʹ�ܴ���ѹǿģ��  
  SPI1_ReadWriteByte(MS5611_ADC);
  byteH = SPI1_ReadWriteByte(0);
  byteM = SPI1_ReadWriteByte(0);
  byteL = SPI1_ReadWriteByte(0);
  ATMOSPHERE_DISENABLE;//��ֹ����ѹǿģ��  
  return_value = (((long)byteH)<<16) | (((long)byteM)<<8) | (byteL);
  return(return_value);
}
/*******************************************************************************
	ͨ��MS5611ģ��õ���ѹֵ
	�������ƣ�MS5611_getPressure
	�����������ѹ�ֱ���ϵ��
	���������void
*******************************************************************************/
float MS5611_getPressure(unsigned char OSR_Pres)
{
	u8 temp;
	
	ATMOSPHERE_ENABLE;                                 //ʹ�ܴ���ѹǿģ��  
  temp = SPI1_ReadWriteByte(CMD_CONVERT_D1_OSR256);
	ATMOSPHERE_DISENABLE;                              //��ֹ����ѹǿģ��
	delay_ms(10);                                      //�����
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
	ͨ��MS5611ģ��õ��¶�ֵ
	�������ƣ�MS5611_getTemperature
	����������¶ȷֱ���ϵ��
	���������void
*******************************************************************************/
float MS5611_getTemperature(unsigned char OSR_Temp)    
{
	u8 temp = 0;
		
	ATMOSPHERE_ENABLE;                                  //ʹ�ܴ���ѹǿģ��  
  temp = SPI1_ReadWriteByte(CMD_CONVERT_D2_OSR4096);
	ATMOSPHERE_DISENABLE;                               //��ֹ����ѹǿģ��
  delay_ms(10);                                       //�˴���ʱ�����
  D2_Temp=MS5611_SPI_read_ADC();	
//  dT=D2_Temp - (((unsigned long)C5)<<8);
  dT=(float)D2_Temp - (((float)C5)*256);
//	Temperature=2000+dT*((unsigned long)C6)/8388608;
	Temperature=2000+dT*((float)C6)/8388608;
	return Temperature;
}

/*******************************************************************************
	���ݴ���ѹ������θ߶� 
	�������ƣ�get_altitude
	���������void
	������������θ߶�
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
