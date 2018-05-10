/**
 * @file            wujique_sysconf.c
 * @brief           ϵͳ����
 * @author          wujique
 * @date            2018��4��9�� ����һ
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��9�� ����һ
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
*/
#include "stm32f4xx.h"
#include "wujique_sysconf.h"
#include "mcu_i2c.h"
#include "mcu_spi.h"

#include "dev_spiflash.h"
#include "dev_lcdbus.h"

#include "dev_lcd.h"

/*

	���ļ���������ϵͳ����Щ�豸
	1.��ײ㣬����STM32��ƣ���������CPU��Ҫ��ֲ��

*/

/*

	IO��ģ���I2C1
	WM8978��TEA5767������I2Cʹ��

*/
#define MCU_I2C1_PORT GPIOD
#define MCU_I2C1_SCL GPIO_Pin_6
#define MCU_I2C1_SDA GPIO_Pin_7
#define MCU_I2C1_RCC RCC_AHB1Periph_GPIOD

DevI2c DevVi2c1={
		.name = "VI2C1",
		
		.sclrcc = MCU_I2C1_RCC,
		.sclport = MCU_I2C1_PORT,
		.sclpin = MCU_I2C1_SCL,

		.sdarcc = MCU_I2C1_RCC,
		.sdaport = MCU_I2C1_PORT,
		.sdapin = MCU_I2C1_SDA,
		};
	
/*

	����IO��ģ��I2C���;��󰴼���ģ��SPI��ͻ

*/		
#define MCU_I2C2_PORT GPIOF
#define MCU_I2C2_SCL GPIO_Pin_11
#define MCU_I2C2_SDA GPIO_Pin_10
#define MCU_I2C2_RCC RCC_AHB1Periph_GPIOF

DevI2c DevVi2c2={
		.name = "VI2C2",
		
		.sclrcc = MCU_I2C2_RCC,
		.sclport = MCU_I2C2_PORT,
		.sclpin = MCU_I2C2_SCL,

		.sdarcc = MCU_I2C2_RCC,
		.sdaport = MCU_I2C2_PORT,
		.sdapin = MCU_I2C2_SDA,
		};	
/*

	IO��ģ��SPI������

*/
/*
	VSPI1��ʹ�ô��������߽ӿ�ģ��SPI������XPT2046�������������ɶ���д��
*/					
DevSpi DevVSpi1IO={
		"VSPI1",
		DEV_SPI_V,
		
		/*clk*/
		RCC_AHB1Periph_GPIOB,
		GPIOB,
		GPIO_Pin_0,
		/*mosi*/
		RCC_AHB1Periph_GPIOD,
		GPIOD,
		GPIO_Pin_11,
		/*miso*/
		RCC_AHB1Periph_GPIOD,
		GPIOD,
		GPIO_Pin_12,
	};
		
/*  �����ӿ�ģ��VSPI2�� �������̣�ģ��I2C2��ͻ    */						
DevSpi DevVspi2IO={
		"VSPI2",
		DEV_SPI_V,
		
		RCC_AHB1Periph_GPIOF,
		GPIOF,
		GPIO_Pin_11,
		
		RCC_AHB1Periph_GPIOF,
		GPIOF,
		GPIO_Pin_10,

		RCC_AHB1Periph_GPIOF,
		GPIOF,
		GPIO_Pin_9,

	};

/*

	Ӳ��SPI��������SPI3
	SPI������ʱ֧��SPI3��
	���������������������޸�mcu_spi.c�е�Ӳ��SPI��������ʼ������
	*/
DevSpi DevSpi3IO={
		"SPI3",
		DEV_SPI_H,
		
		RCC_AHB1Periph_GPIOB,
		GPIOB,
		GPIO_Pin_11,
		
		RCC_AHB1Periph_GPIOB,
		GPIOB,
		GPIO_Pin_10,

		RCC_AHB1Periph_GPIOB,
		GPIOB,
		GPIO_Pin_9,
	};

/* SPIͨ��*/
/* FLASH*/
DevSpiCh DevSpi3CH1={
		"SPI3_CH1",
		"SPI3",
		
		RCC_AHB1Periph_GPIOB,
		GPIOB,
		GPIO_Pin_14,
		
	};
		
DevSpiCh DevSpi3CH2={
		"SPI3_CH2",
		"SPI3",
		
		RCC_AHB1Periph_GPIOG,
		GPIOG,
		GPIO_Pin_15,
		
	};
/*����SPI*/		
DevSpiCh DevSpi3CH3={
		"SPI3_CH3",
		"SPI3",
		
		RCC_AHB1Periph_GPIOG,
		GPIOG,
		GPIO_Pin_6,
		
	};
/* ������IOģ��SPI*/
DevSpiCh DevVSpi1CH1={
		"VSPI1_CH1",
		"VSPI1",
		
		RCC_AHB1Periph_GPIOB,
		GPIOB,
		GPIO_Pin_1,
		
	};
/*����IO*/
DevSpiCh DevVSpi2CH1={
		"VSPI2_CH1",
		"VSPI2",
		
		RCC_AHB1Periph_GPIOF,
		GPIOF,
		GPIO_Pin_12,
		
	};
	
/*
	����LCD�ӿڣ�ʹ��������SPI����

*/
DevLcdBus BusLcdSpi3={
	.name = "BusLcdSpi3",
	.type = LCD_BUS_SPI,
	.basebus = "SPI3_CH3",

	.A0rcc = RCC_AHB1Periph_GPIOG,
	.A0port = GPIOG,
	.A0pin = GPIO_Pin_4,

	.rstrcc = RCC_AHB1Periph_GPIOG,
	.rstport = GPIOG,
	.rstpin = GPIO_Pin_7,

	.blrcc = RCC_AHB1Periph_GPIOG,
	.blport = GPIOG,
	.blpin = GPIO_Pin_9,
};


DevLcdBus BusLcdI2C1={
	.name = "BusLcdI2C1",
	.type = LCD_BUS_I2C,
	.basebus = "VI2C1",

	/*I2C�ӿڵ�LCD���ߣ�����Ҫ����IO*/

};
DevLcdBus BusLcd8080={
	.name = "BusLcd8080",
	.type = LCD_BUS_8080,
	.basebus = "8080",//�����ã�8080����ֱ��Ƕ����LCD BUS������

	/*8080 ����A0�ţ�����ȥ*/
	.A0rcc = RCC_AHB1Periph_GPIOB,
	.A0port = GPIOB,
	.A0pin = GPIO_Pin_15,

	.rstrcc = RCC_AHB1Periph_GPIOA,
	.rstport = GPIOA,
	.rstpin = GPIO_Pin_15,

	.blrcc = RCC_AHB1Periph_GPIOB,
	.blport = GPIOB,
	.blpin = GPIO_Pin_15,

};

DevLcdBus BusLcdVSpi2CH1={
	.name = "BusLcdVSpi2CH1",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI2_CH1",

	.A0rcc = RCC_AHB1Periph_GPIOF,
	.A0port = GPIOF,
	.A0pin = GPIO_Pin_8,

	.rstrcc = RCC_AHB1Periph_GPIOF,
	.rstport = GPIOF,
	.rstpin = GPIO_Pin_13,

	.blrcc = RCC_AHB1Periph_GPIOF,
	.blport = GPIOF,
	.blpin = GPIO_Pin_14,
};


DevSpiFlash DevSpiFlashCore={
	/*��һ������board_spiflash��SPI FLASH����DEV_SPI_3_2�ϣ��ͺ�δ֪*/
	"board_spiflash", 
	"SPI3_CH2", 
	NULL
};

DevSpiFlash DevSpiFlashBoard={
	"core_spiflash",  
	"SPI3_CH1", 
	NULL
};

/*
	�豸������
	ָ��ϵͳ�ж��ٸ�LCD�豸�������ĸ�LCD�����ϡ�
*/
DevLcd DevLcdOled1	=	{"i2coledlcd",  "BusLcdI2C1",  0X1315};
//LcdObj DevLcdOled2	=	{"i2coledlcd2", LCD_BUS_VI2C2,  0X1315};
//LcdObj DevLcdOled3	=	{"vspioledlcd", LCD_BUS_VSPI, 	0X1315};
DevLcd DevLcdOled4	=	{"spioledlcd", 	"BusLcdSpi3", 	0X1315};
DevLcd DevLcdCOG1	=	{"spicoglcd", 	"BusLcdSpi3", 	0X7565};
//LcdObj DevLcdCOG2	=	{"vspicoglcd", 	LCD_BUS_VSPI, 	0X7565};
DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	NULL};


s32 sys_dev_init(void)
{
	/*ע��I2C����*/
	mcu_i2c_register(&DevVi2c1);

	#ifdef SYS_USE_VI2C2
	//mcu_i2c_register(&DevVi2c2);
	#endif
	
	/*ע��SPI������*/
	mcu_spi_register(&DevSpi3IO);
	
	#ifdef SYS_USE_VSPI1
	mcu_spi_register(&DevVSpi1IO);
	#endif

	#ifdef SYS_USE_VSPI2
	mcu_spi_register(&DevVspi2IO);
	#endif
	
	/*ע��SPI ͨ��*/
	mcu_spich_register(&DevSpi3CH1);
	mcu_spich_register(&DevSpi3CH2);
	mcu_spich_register(&DevSpi3CH3);
	
	mcu_spich_register(&DevVSpi1CH1);

	mcu_spich_register(&DevVSpi2CH1);
	
	/*ע��LCD����*/
	dev_lcdbus_register(&BusLcdSpi3);
	dev_lcdbus_register(&BusLcdI2C1);
	dev_lcdbus_register(&BusLcd8080);
	dev_lcdbus_register(&BusLcdVSpi2CH1);
	/*
		ע���豸

	*/
	/*ע��FLASH�豸*/
	dev_spiflash_register(&DevSpiFlashCore);
	dev_spiflash_register(&DevSpiFlashBoard);
	/*ע��LCD�豸*/
	dev_lcd_register(&DevLcdOled1);
	dev_lcd_register(&DevLcdtTFT);
	dev_lcd_register(&DevLcdCOG1);
}



