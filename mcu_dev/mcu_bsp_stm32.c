/**
 * @file            mcu_bsp_stm32.c
 * @brief           adc����
 * @author          wujique
 * @date            2017��12��8�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��12��8�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
       	1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱���������������뱣��WUJIQUE��Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/
/*
	��װIO�������Ա���ֲ������оƬ
*/
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "mcu_bsp.h"

GPIO_TypeDef *Stm32PortList[16] = { GPIOA,  GPIOB, GPIOC, GPIOD,
									GPIOE, GPIOF, GPIOG, GPIOH,
									GPIOI, GPIOJ, GPIOK, NULL};

void mcu_io_config_in(MCU_PORT port, u16 pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(Stm32PortList[port], &GPIO_InitStructure);//��ʼ��  	
}


void mcu_io_config_out(MCU_PORT port, u16 pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//����
	GPIO_Init(Stm32PortList[port], &GPIO_InitStructure);//��ʼ��
}

void mcu_io_output_setbit(MCU_PORT port, u16 pin)
{
	GPIO_SetBits(Stm32PortList[port], pin);
}

void mcu_io_output_resetbit(MCU_PORT port, u16 pin)
{
	GPIO_ResetBits(Stm32PortList[port], pin);
}		

u8 mcu_io_input_readbit(MCU_PORT port, u16 pin)
{
	return GPIO_ReadInputDataBit(Stm32PortList[port], pin);
}




