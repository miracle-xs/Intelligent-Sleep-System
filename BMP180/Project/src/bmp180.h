#ifndef __bmp180_H
#define __bmp180_H
#include "myiic.h" 
#include  <math.h>    //Keil library  
#include  <stdlib.h>  //Keil library  
#include  <stdio.h>   //Keil library	
//#include  <intrins.h> //Keil library  

#define	bmp180_SlaveAddress   0xee	  //定义器件在IIC总线中的从地址                              
#define OSS 0	// Oversampling Setting (note: code is not set up to use other OSS values)

short Multiple_read(u8 ST_Address);
long bmp180ReadTemp(void);
long bmp180ReadPressure(void);
void bmp180_Init(void);
long Temperature_Convert(void);
long Pressure_Convert(void);
//void bmp180Convert(void);
#endif
