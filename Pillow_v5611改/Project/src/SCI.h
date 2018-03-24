/***********************************************************************
文件名称：SCI.h
功    能：
编写时间：2012.11.22
编 写 人：
注    意：
***********************************************************************/
#ifndef  _SCI_H_
#define  _SCI_H_

#include <stdio.h>

void RS232_Send1_Data(unsigned char *send_buff,unsigned int length);
void RS232_Send2_Data(unsigned char *send_buff,unsigned int length);
void RS232_Send3_Data(unsigned char *send_buff,unsigned int length);

#endif
