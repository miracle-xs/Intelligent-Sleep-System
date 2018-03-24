/***********************************************************************
�ļ����ƣ�NVIC.C
��    �ܣ����STM32�ж�����
��дʱ�䣺2012.11.22
�� д �ˣ�
ע    �⣺ID�ı������������ǣ�����+Ŀ�ĵ�ַ+Դ��ַ,���磺
			#define CRM_CHARGER_BMS	  	0X1801F456		//������ʶ����ID
			����ΪCRM,Դ��ַΪCHARGER��Ŀ�ĵ�ַΪBMS
***********************************************************************/
#include "stm32f10x.h"
#include "sys.h"

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
 	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	  //�ж�Ϊ����
	
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //��2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	  /* Enable the USART3 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //��2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 //��2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
}