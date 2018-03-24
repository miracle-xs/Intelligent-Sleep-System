
#include "USER_delay.h"

//��AHB�ķ�Ƶϵ��Ϊ1��ʱ��SYSTEM��ʱ�Ӻ�HCLK��һ��
//��ֹ��ʱ��׼ȷ,����do while�ṹ!
static u8  fac_us = 0;//us��ʱ������
static u16 fac_ms = 0;//ms��ʱ������
//��ʼ���ӳٺ���
//SysTick��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ�ӣ��˴�ֱ�Ӵ���MHZ�ĵ�λ����72
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL &= 0xfffffffb;             //bit2���,ѡ���ⲿʱ��  HCLK/8 = SYSCLK/8  
	fac_us = SYSCLK/8;                       //������ֵΪSYSCLK/8����������Ϊ 8/SYSCLK��us�������Ϊ1US	    
	fac_ms = (u16)fac_us*1000;               //����1000��΢��Ϊһ����
}								    
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=��2^24��*8/��SYSCLK*1000��= 1864MS����72MHZ�������£�
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD = (u32)nms*fac_ms;       //ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL = 0x00;                   //д��ǰ����ֵ�Ĵ���ʹ����0
	SysTick->CTRL = 0x01 ;                 //ʹ�ܼ���  
	do
	{
		temp = SysTick->CTRL;                //��ͣ�Ķ�ȡ���Ƽ�״̬�Ĵ���
	}
	while(temp&0x01&&!(temp&(1<<16)));     //���ʹ���˲��ҵ�16λCOUNTFLAGΪ0����ѭ��   
	SysTick->CTRL = 0x00;                  //���������رռ�����
	SysTick->VAL = 0X00;                   //д��ǰ��ֵ�Ĵ���ʹ֮���  	    
}   
//��ʱnus
//nusΪҪ��ʱ��us��.
//nus < 1864135 (��72MHZ����)		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD = nus*fac_us;          //ʱ�����	  		 
	SysTick->VAL = 0x00;                 //��ռ�����
	SysTick->CTRL = 0x01 ;               //��ʼ���� 	 
	do
	{
		temp = SysTick->CTRL;	             //���϶�ȡ���Ƽ�״̬�Ĵ���
	}
	while(temp&0x01&&!(temp&(1<<16)));   //���ʹ���˲��ҵ�16λCOUNTFLAGΪ0����ѭ��      
	SysTick->CTRL = 0x00;                //�رռ�����
	SysTick->VAL  = 0X00;                //��ռ�����	 
}
/**********************************************************************
SysTick��һ��24λ�ĵ�������ʱ��,���Ƶ�0ʱ������RELOAD�Ĵ������Զ���װ�ض�
ʱ��ֵ.ֻҪ��������SysTick���Ƽ�״̬�Ĵ����е�ʹ��λ�����������ͣϢ��
 
  SysTick���Ƽ�״̬�Ĵ���SysTick->CTRL
	  ��16λ COUNTFLAG  ������ϴζ�ȡ���Ĵ�����SysTick�Ѿ�������
	                    0�����λΪ1�������ȡ��λ����λ���Զ�����
	  ��2λ  CLKSOURCE  ֵΪ0��ʾѡ���ⲿʱ��Դ(STCLK)
	                    ֵΪ1��ʾѡ���ں�ʱ��(FCLK) 
	  ��1λ  TICKINT    ��1��ʱ��SysTick������0ʱ����SysTick�쳣����ֻ�������˲Ż�����Ӧ�� 
	                    ��0��ʱ������0ʱ�޶��� 
	  ��0λ  ENABLE     SysTick��ʱ����ʹ��λ 

  SysTick��װ����ֵ�Ĵ���SysTick->LOAD
      23:0   RELOAD      ����������ʱ��������װ�ص�ֵ,ע����24λ�ģ�

  SysTick��ǰ��ֵ�Ĵ���SysTick->VAL
      23:0  CURRENT      ��ȡʱ���ص�ǰ��������ֵ��д����ʹ֮���㣬
                         ͬʱ��������� SysTick ���Ƽ�״̬�Ĵ����е�COUNTFLAG��־

  SysTickУ׼��ֵ�Ĵ���SysTick->CALIB
      31    NOREF      1=û���ⲿ�ο�ʱ�ӣ�STCLK�����ã� 
                       0=�ⲿ�ο�ʱ�ӿ��� 
      30    SKEW       1=У׼ֵ����׼ȷ��10ms 
                       0=У׼ֵ��׼ȷ��10ms 
      23:0  TENMS      10ms��ʱ���ڵ������ĸ�����оƬ�����Ӧ��ͨ
                       �� Cortex�\M3 �������ź��ṩ����ֵ������ֵ��
                       ���㣬���ʾ�޷�ʹ��У׼����

RCCͨ��AHBʱ��(HCLK)8��Ƶ����ΪCortexϵͳ��ʱ��(SysTick)���ⲿʱ�ӡ�ͨ����
SysTick������״̬�Ĵ��������ã���ѡ��FCLKʱ�ӻ�Cortex/8(HCLK/8)ʱ����ΪSysTick
ʱ�ӡ�
ͨ�����У׼�Ĵ�����λ[31](NOREF)�������ж��Ƿ��п��õ��ⲿʱ��Դ��
��оƬ���������Ѹ�������������ȷ�ĵ�ƽ�� 
 ��SysTick��ʱ����1�Ƶ�0ʱ��������COUNTFLAGλ��λ��������������������֮ 
  1����ȡSysTick���Ƽ�״̬�Ĵ�����STCSR�� 
  2����SysTick��ǰֵ�Ĵ�����STCVR����д�κ����� 
 
**********************************************************************/


































