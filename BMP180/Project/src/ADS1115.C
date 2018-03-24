#include "stm32f10x.h"
#include "ADS1115.h"
#include "stm32f10x_adc.h"
#include "USER_delay.h"


void ADC_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;  //
	ADC_InitTypeDef ADC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1,ENABLE);//
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AIN;//
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  
  
  ADC_DeInit(ADC1);
  ADC_InitStructure.ADC_Mode =ADC_Mode_Independent;//
  ADC_InitStructure.ADC_ScanConvMode =DISABLE;//
  ADC_InitStructure.ADC_ContinuousConvMode =ENABLE;//
  ADC_InitStructure.ADC_ExternalTrigConv =ADC_ExternalTrigConv_None;//
  ADC_InitStructure.ADC_DataAlign =ADC_DataAlign_Right;//
  ADC_InitStructure.ADC_NbrOfChannel =1;//
  ADC_Init(ADC1,&ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_239Cycles5);//
  ADC_Cmd(ADC1,ENABLE);//
  ADC_ResetCalibration(ADC1);//
	while(ADC_GetResetCalibrationStatus(ADC1));// 
	ADC_StartCalibration(ADC1);//
	while(ADC_GetCalibrationStatus(ADC1));//
  ADC_SoftwareStartConvCmd(ADC1,ENABLE);//
  
}
/*************ADS1115初始化函数*************/
/***********输入值 通道 0,1,2,3*************/
/**********返回值 最终电压 单位V************/
/***********返回值是带有小数的**************/
/*********在进行这类数据转换时**************/
/*******一定要注意数据类型的定义************/
//float ADS1115(u8 Channel)
//{
//	unsigned char  MSB,LSB;  //用于存放读出来的寄存器数据
//	float Value;
//	///////////配置Config 寄存器
//	
//	ADS1115_Start();          //发送起始信号
//	ADS1115_Write_Byte(0x90); //器件地址+写
//	while(!get_ack());
//	ADS1115_Write_Byte(0x01); //Config registor寄存器地址
//	while(!get_ack());
//	switch(Channel)
//	{ 
//		//经实测,该16位寄存器第一位必须为1,否则无法完成转化,不知道为啥
//		case 0:   //使用通道0,FS为4.096
//		ADS1115_Write_Byte(0xc2);	
//		break;
//		case 1:   //使用通道1
//		ADS1115_Write_Byte(0xd2);	
//		break;
//	  case 2:   //使用通道2
//		ADS1115_Write_Byte(0xe2);	
//		break;
//		case 3:   //使用通道3
//		ADS1115_Write_Byte(0xf2);	
//		break;
//	}
//	while(!get_ack());
//	ADS1115_Write_Byte(0xe2); //网上下载的一个程序是如下配置的
//	while(!get_ack());
//	ADS1115_Stop();   //发送终止信号
//	
//	delay_ms(2);
//	//下面时开始读取的过程
//	////////////配置pointer 寄存器,如下配置是固定的
//	ADS1115_Start();          //发送起始信号
//	ADS1115_Write_Byte(0x90); //器件地址+写
//	while(!get_ack());
//	ADS1115_Write_Byte(0x00);
//	while(!get_ack());
//	ADS1115_Stop();   //发送终止信号
//	delay_ms(2);
//	
//	ADS1115_Start(); 
//	ADS1115_Write_Byte(0x91);  //器件地址+读
//	while(!get_ack());
//	MSB = ADS1115_Read_Byte(); //高八位
//	ACK();
//	LSB = ADS1115_Read_Byte(); //低八位
//	NACK();    //这里存在疑问,为什么最后一次发送非应答信号??
//	ADS1115_Stop();   //发送终止信号
//	Value = MSB*256 + LSB;
//	
//	//Vin = +FS/32768 * Value
//	Value = Value*4.096/32768;  //最终输出结果,单位:伏特V
//	return Value;
//}
