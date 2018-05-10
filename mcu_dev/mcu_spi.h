#ifndef __MCU_SPI_H_
#define __MCU_SPI_H_

#include "list.h"

typedef enum{
	DEV_SPI_H = 1,//Ӳ��SPI������
	DEV_SPI_V = 2,//IOģ��SPI
}DEV_SPI_TYPE;

/*
	SPI �����㣬
	1����SPI��������������CS
	2����SPIͨ�����ɿ�����+CS���
	
*/
/*

	SPI �豸����

*/
typedef struct
{
	/*�豸����*/
	char name[16];
	/*�豸���ͣ�IOģ�� or Ӳ��������*/
	DEV_SPI_TYPE type;
	
	u32 clkrcc;
	GPIO_TypeDef *clkport;
	u16 clkpin;

	u32 mosircc;
	GPIO_TypeDef *mosiport;
	u16 mosipin;

	u32 misorcc;
	GPIO_TypeDef *misoport;
	u16 misopin;

}DevSpi;

/*

	SPI�������豸�ڵ�
	
*/
typedef struct
{
	/*���������Ϊ-1����Ϊ0��spi�����������ظ���*/
	s32 gd;
	/*������Ӳ����Ϣ����ʼ��������ʱ�����豸������Ϣ����*/
	DevSpi dev;	
	/*����*/
	struct list_head list;
}DevSpiNode;

/*
	SPI ͨ������
	һ��SPIͨ������һ��SPI������+һ��CS�������

*/
typedef struct
{
	/*ͨ�����ƣ��൱���豸����*/
	char name[16];
	/*SPI����������*/
	char spi[16];

	/*cs��*/
	u32 csrcc;
	GPIO_TypeDef *csport;
	u16 cspin;
}DevSpiCh;

/*SPIͨ���ڵ�*/
typedef struct
{
	/**/
	s32 gd;
	DevSpiCh dev;	
	DevSpiNode *spi;//�������ڵ�ָ��
	struct list_head list;
}DevSpiChNode;

/*

SPIģʽ

*/
typedef enum{
	SPI_MODE_0 =0,
	SPI_MODE_1,
	SPI_MODE_2,
	SPI_MODE_3,
	SPI_MODE_MAX
}SPI_MODE;

extern DevSpiChNode *mcu_spi_open(char *name, SPI_MODE mode, u16 pre);
extern s32 mcu_spi_close(DevSpiChNode * node);
extern s32 mcu_spi_transfer(DevSpiChNode * node, u8 *snd, u8 *rsv, s32 len);
extern s32 mcu_spi_cs(DevSpiChNode * node, u8 sta);


#endif

