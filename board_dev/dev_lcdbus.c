/**
 * @file            dev_lcdbus.c
 * @brief           �Ը���LCD�ӿڷ�װ
 * @author          wujique
 * @date            2018��4��18�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��18�� ������
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
#include <stdarg.h>
#include <stdio.h>
#include "stm32f4xx.h"
#include "main.h"
#include "wujique_log.h"
#include "list.h"
#include "mcu_spi.h"
#include "mcu_i2c.h"
#include "dev_lcdbus.h"


/*
	д�Ĵ���Ҫ����
	*LcdReg = LCD_Reg; //д��Ҫд�ļĴ������
	*LcdData = LCD_RegValue; //д������ 
*/

volatile u16 *LcdReg = (u16*)0x6C000000;
volatile u16 *LcdData = (u16*)0x6C010000;

/*
	һ��LCD�ӿ�
	����ͨ�ŵĽӿ�
	��������������ͨ�Žӿڵ��ź�
*/

/*LCD �����豸�ڵ���ڵ�*/
struct list_head DevBusLcdRoot = {&DevBusLcdRoot, &DevBusLcdRoot};	

static void bus_lcd_IO_init(DevLcdBus *dev) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	if(dev->type == LCD_BUS_I2C)
		return;

	RCC_AHB1PeriphClockCmd(dev->A0rcc,  ENABLE);
	RCC_AHB1PeriphClockCmd(dev->rstrcc, ENABLE);
	RCC_AHB1PeriphClockCmd(dev->blrcc,  ENABLE);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	//DC(A0)
	
	GPIO_InitStructure.GPIO_Pin = dev->A0pin;
	GPIO_Init(dev->A0port,  &GPIO_InitStructure);
	GPIO_SetBits(dev->A0port, dev->A0pin);

	//RST
	GPIO_InitStructure.GPIO_Pin = dev->rstpin; //OUT�������   RST
	GPIO_Init(dev->rstport, &GPIO_InitStructure);
	GPIO_SetBits(dev->rstport, dev->rstpin);

	//bl
	GPIO_InitStructure.GPIO_Pin = dev->blpin; //OUT������� 
	GPIO_Init(dev->blport, &GPIO_InitStructure);
	GPIO_SetBits(dev->blport, dev->blpin);	

}

s32 bus_lcd_bl(DevLcdBusNode *node, u8 sta)
{
	if(sta ==1)
	{
		GPIO_SetBits(node->dev.blport, node->dev.blpin);
	}
	else
	{
		GPIO_ResetBits(node->dev.blport, node->dev.blpin);	
	}
	return 0;
}

s32 bus_lcd_rst(DevLcdBusNode *node, u8 sta)
{
	if(sta ==1)
	{
		GPIO_SetBits(node->dev.rstport, node->dev.rstpin);
	}
	else
	{
		GPIO_ResetBits(node->dev.rstport, node->dev.rstpin);	
	}
	return 0;
}

static s32 bus_lcd_a0(DevLcdBusNode *node, u8 sta)
{
	if(node->dev.type == LCD_BUS_8080)
		return 0;
	
	if(sta ==1)
	{
		GPIO_SetBits(node->dev.A0port, node->dev.A0pin);
	}
	else
	{
		GPIO_ResetBits(node->dev.A0port, node->dev.A0pin);	
	}
	return 0;
}


DevLcdBusNode *bus_lcd_open(char *name)
{
	/*���豸*/
	DevLcdBusNode *node;
	struct list_head *listp;

	//wjq_log(LOG_INFO, "lcd bus name:%s!\r\n", name);
	
	listp = DevBusLcdRoot.next;
	node = NULL;
	
	while(1)
	{
		if(listp == &DevBusLcdRoot)
			break;

		node = list_entry(listp, DevLcdBusNode, list);
		//wjq_log(LOG_INFO, "lcd bus name:%s!\r\n", node->dev.name);
		
		if(strcmp(name, node->dev.name) == 0)
		{
			break;
		}
		else
		{
			node = NULL;
		}
		
		listp = listp->next;
	}

	if(node != NULL)
	{
		if(node->gd == 0)
		{
			wjq_log(LOG_INFO, "lcd bus open err:using!\r\n");
			node = NULL;
		}
		else
		{
			
			if(node->dev.type == LCD_BUS_SPI)
			{
				node->basenode = (void *)mcu_spi_open(node->dev.basebus, SPI_MODE_3, SPI_BaudRatePrescaler_4);

			}
			else if(node->dev.type == LCD_BUS_I2C)
			{
				node->basenode = mcu_i2c_open(node->dev.basebus);
			}
			else if(node->dev.type == LCD_BUS_8080)
			{
				/*8080���⴦��*/
				node->basenode = (void *)1;
			}
			
			if(node->basenode == NULL)
			{
				wjq_log(LOG_INFO, "lcd bus open base bus err!\r\n");	
				node =  NULL;
			}
			else
			{
				node->gd = 0;

			}
		}
	}
	else
	{
		wjq_log(LOG_INFO, "lcd bus open err:%s!\r\n", name);
	}

	return node;
}

s32 bus_lcd_close(DevLcdBusNode *node)
{
	if(node->gd != 0)
		return -1;
	
	if(node->dev.type == LCD_BUS_SPI)
	{
		mcu_spi_close((DevSpiChNode *)node->basenode);
		
	}
	else if(node->dev.type == LCD_BUS_I2C)
	{
		mcu_i2c_close((DevI2cNode *)node->basenode);	
	}
	else if(node->dev.type == LCD_BUS_8080)
	{
		/*8080���⴦��*/
		node->basenode = NULL;
	}
	
	node->gd = -1;
	
	return 0;
}

s32 bus_lcd_write_data(DevLcdBusNode *node, u8 *data, u16 len)
{
	/*������BUF��Ҫ����len��̬����*/
	u8 tmp[256];
	u16 i;
	
	if(node->dev.type == LCD_BUS_SPI)
	{
		bus_lcd_a0(node, 1);	
		mcu_spi_cs((DevSpiChNode *)node->basenode, 0);
		mcu_spi_transfer((DevSpiChNode *)node->basenode,  data, NULL, len);
		mcu_spi_cs((DevSpiChNode *)node->basenode, 1);
		
	}
	else if(node->dev.type == LCD_BUS_I2C)
	{
		
		tmp[0] = 0x40;
		memcpy(&tmp[1], data, len);
		mcu_i2c_transfer((DevI2cNode *)node->basenode, 0x3C, MCU_I2C_MODE_W, tmp, len+1);
		

	}
	else if(node->dev.type == LCD_BUS_8080)
	{
		u16 *p;
		p = (u16 *)data;
		for(i=0; i<len; i++)
		{
			*LcdData = *(p+i);	
		}
	}
	return 0;
}

s32 bus_lcd_write_cmd(DevLcdBusNode *node, u8 cmd)
{
	u8 tmp[2];

	if(node->dev.type == LCD_BUS_SPI)
	{	
		bus_lcd_a0(node, 0);
		tmp[0] = cmd;
		mcu_spi_cs((DevSpiChNode *)node->basenode, 0);
		mcu_spi_transfer((DevSpiChNode *)node->basenode,  &tmp[0], NULL, 1);
		mcu_spi_cs((DevSpiChNode *)node->basenode, 1);
	}
	else if(node->dev.type == LCD_BUS_I2C)
	{	
		tmp[0] = 0x00;
		tmp[1] = cmd;
		
		mcu_i2c_transfer((DevI2cNode *)node->basenode, 0x3C, MCU_I2C_MODE_W, tmp, 2);
	}
	else if(node->dev.type == LCD_BUS_8080)
	{
		*LcdReg = cmd;	
	}
	return 0;
}


/**
 *@brief:      dev_lcdbus_init
 *@details:    ��ʼ������LCD����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_lcdbus_register(DevLcdBus *dev)
{
	struct list_head *listp;
	DevLcdBusNode *p;

	wjq_log(LOG_INFO, "[register] lcd bus :%s, base on:%s!\r\n", dev->name, dev->basebus);

	/*
		��Ҫ��ѯ��ǰ����ֹ����
	*/
	listp = DevBusLcdRoot.next;
	while(1)
	{
		if(listp == &DevBusLcdRoot)
			break;

		p = list_entry(listp, DevLcdBusNode, list);

		if(strcmp(dev->name, p->dev.name) == 0)
		{
			wjq_log(LOG_INFO, "bus lcd dev name err!\r\n");
			return -1;
		}
		
		listp = listp->next;
	}

	/* 
		����һ���ڵ�ռ� 
		
	*/
	p = (DevLcdBusNode *)wjq_malloc(sizeof(DevLcdBusNode));
	list_add(&(p->list), &DevBusLcdRoot);
	/*�����豸��Ϣ*/
	memcpy((u8 *)&p->dev, (u8 *)dev, sizeof(DevLcdBus));
	p->gd = -1;

	/*��ʼ��*/
	bus_lcd_IO_init(dev);

	if(dev->type == LCD_BUS_8080)
	{
		//��ʼFSMC
		mcu_fsmc_lcd_Init();
	}
	return 0;
}





