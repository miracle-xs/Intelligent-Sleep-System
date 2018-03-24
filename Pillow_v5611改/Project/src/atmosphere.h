#ifndef __ATMOSPHERE_H
#define __ATMOSPHERE_H
#include "sys.h"

#define ATMOSPHERE_ENABLE 	          GPIO_ResetBits(GPIOA, GPIO_Pin_4)    //片选有效
#define ATMOSPHERE_DISENABLE          GPIO_SetBits(GPIOA, GPIO_Pin_4)      //片选无效

/*SPI1相关引脚定义*/
#define ATMOSPHERE_GPIO               GPIOA                     
#define ATMOSPHERE_GPIO_CLK           RCC_AHB1Periph_GPIOA         

#define ATMOSPHERE_NSS_GPIO           GPIOA
#define ATMOSPHERE_NSS_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define ATMOSPHERE_NSS_PIN            GPIO_Pin_4

#define ATMOSPHERE_SCK_GPIO           GPIOA
#define ATMOSPHERE_SCK_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define ATMOSPHERE_SCK_PIN            GPIO_Pin_5

#define ATMOSPHERE_MISO_GPIO          GPIOA
#define ATMOSPHERE_MISO_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ATMOSPHERE_MISO_PIN           GPIO_Pin_6

#define ATMOSPHERE_MOSI_GPIO          GPIOA
#define ATMOSPHERE_MOSI_GPIO_CLK      RCC_AHB1Periph_GPIOA
#define ATMOSPHERE_MOSI_PIN           GPIO_Pin_7

/*MS5611模块传感器地址定义*/
#define MS5611_ADC       0x00
#define CMD_MS5611_RESET 0x1E
#define CMD_MS5611_PROM_Setup 0xA0
#define CMD_MS5611_PROM_C1 0xA2
#define CMD_MS5611_PROM_C2 0xA4
#define CMD_MS5611_PROM_C3 0xA6
#define CMD_MS5611_PROM_C4 0xA8
#define CMD_MS5611_PROM_C5 0xAA
#define CMD_MS5611_PROM_C6 0xAC
#define CMD_MS5611_PROM_CRC 0xAE
#define CMD_CONVERT_D1_OSR4096 0x48   // Maximun resolution
#define CMD_CONVERT_D2_OSR4096 0x58   // Maximun resolution
#define CMD_CONVERT_D1_OSR256 0x40


/*相关函数声明*/
void SPI1_Init(void);
void SPI1_SetSpeed(u8 SpeedSet); 
u8 SPI1_ReadWriteByte(u8 TxData);
u8 ATMOSPHERE_Write_Reg(u8 REG_Address,u8 REG_data);
u8 ATMOSPHERE_Read_Reg(u8 REG_Address);
void MS5611_RESET(void);
u16  SPI1_ReadWrite_16bits(unsigned char addr);
void MS5611_PROM_READ(void);
long MS5611_SPI_read_ADC(void);
float MS5611_getPressure(unsigned char OSR_Pres);
float MS5611_getTemperature(unsigned char OSR_Temp);
float get_altitude(void);
#endif
