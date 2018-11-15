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

const DevI2c DevVi2c1={
		.name = "VI2C1",
		
		.sclport = MCU_PORT_D,
		.sclpin = GPIO_Pin_6,

		.sdaport = MCU_PORT_D,
		.sdapin = GPIO_Pin_7,
		};
	
/*

	����IO��ģ��I2C���;��󰴼���ģ��SPI��ͻ

*/		
#if 0
const DevI2c DevVi2c2={
		.name = "VI2C2",
		
		.sclport = MCU_PORT_F,
		.sclpin = GPIO_Pin_11,

		.sdaport = MCU_PORT_F,
		.sdapin = GPIO_Pin_10,
		};	
#endif
/*

	IO��ģ��SPI������

*/
/*
	VSPI1��ʹ�ô��������߽ӿ�ģ��SPI������XPT2046�������������ɶ���д��
*/					
const DevSpi DevVSpi1IO={
		.name = "VSPI1",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = GPIO_Pin_0,
		/*mosi*/
		.mosiport = MCU_PORT_D,
		.mosipin = GPIO_Pin_11,
		/*miso*/
		.misoport = MCU_PORT_D,
		.misopin = GPIO_Pin_12,
	};

/*  �����ӿ�ģ��VSPI2�� �������̣�ģ��I2C2��ͻ    */			
#if 0
const DevSpi DevVspi2IO={
		.name = "VSPI2",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_F,
		.clkpin = GPIO_Pin_11,
		
		/*mosi*/
		.mosiport = MCU_PORT_F,
		.mosipin = GPIO_Pin_10,

		/*miso*/
		.misoport = MCU_PORT_F,
		.misopin = GPIO_Pin_9,

	};
#endif
/*

	Ӳ��SPI��������SPI3
	SPI������ʱ֧��SPI3��
	���������������������޸�mcu_spi.c�е�Ӳ��SPI��������ʼ������
	*/
const DevSpi DevSpi3IO={
		.name = "SPI3",
		.type = DEV_SPI_H,
		
		/*clk*/
		.clkport = MCU_PORT_B,
		.clkpin = GPIO_Pin_3,
		
		/*mosi*/
		.mosiport = MCU_PORT_B,
		.mosipin = GPIO_Pin_5,

		/*miso*/
		.misoport = MCU_PORT_B,
		.misopin = GPIO_Pin_4,
	};

/* SPIͨ��*/
/* FLASH 1*/
const DevSpiCh DevSpi3CH1={
		.name = "SPI3_CH1",
		.spi = "SPI3",
		
		.csport = MCU_PORT_B,
		.cspin = GPIO_Pin_14,
		
	};
/* flash 2*/		
const DevSpiCh DevSpi3CH2={
		.name = "SPI3_CH2",
		.spi = "SPI3",
		
		.csport = MCU_PORT_G,
		.cspin = GPIO_Pin_15,
		
	};
		
		
/*����SPI���ɽ�COG��OLED��SPI TFT��RF24L01*/			
const DevSpiCh DevSpi3CH3={
		.name = "SPI3_CH3",
		.spi = "SPI3",
		
		.csport = MCU_PORT_G,
		.cspin = GPIO_Pin_6,
		
	};
		
#if 0		
/*������SPI, �����Ĵ�����*/
const DevSpiCh DevSpi3CH4={
		.name = "SPI3_CH4",
		.spi = "SPI3",
		
		.csport = MCU_PORT_F,
		.cspin = GPIO_Pin_2,
		
	};
#else
const DevSpi DevVspi3IO={
		.name = "VSPI3",
		.type = DEV_SPI_V,
		
		/*clk*/
		.clkport = MCU_PORT_G,
		.clkpin = GPIO_Pin_6,
		
		/*mosi*/
		.mosiport = MCU_PORT_F,
		.mosipin = GPIO_Pin_2,

		/*miso*/
		.misoport = NULL,
		.misopin = NULL,

	};
const DevSpiCh DevVSpi3CH1={
		.name = "VSPI3_CH1",
		.spi = "VSPI3",
		
		.csport = NULL,
		.cspin = NULL,
		
	};

#endif

/* ������, IOģ��SPI*/
const DevSpiCh DevVSpi1CH1={
		.name = "VSPI1_CH1",
		.spi = "VSPI1",
		
		.csport = MCU_PORT_B,
		.cspin = GPIO_Pin_1,
		
	};
/* SPI������������������ͬ�Ŀ�����*/		
const DevSpiCh DevVSpi1CH2={
		.name = "VSPI1_CH2",
		.spi = "VSPI1",
		
		.csport = MCU_PORT_D,
		.cspin = GPIO_Pin_14,
	};
/*����IO*/
#if 0
const DevSpiCh DevVSpi2CH1={
		.name = "VSPI2_CH1",
		.spi = "VSPI2",
		
		.csport = MCU_PORT_F,
		.cspin = GPIO_Pin_12,
		
	};
#endif	
#if 1
/*
	����LCD�ӿڣ�ʹ��������SPI����
	����SPI
*/
const DevLcdBus BusLcdSpi3={
	.name = "BusLcdSpi3",
	.type = LCD_BUS_SPI,
	.basebus = "SPI3_CH3",

	.A0port = MCU_PORT_G,
	.A0pin = GPIO_Pin_4,

	.rstport = MCU_PORT_G,
	.rstpin = GPIO_Pin_7,

	.blport = MCU_PORT_G,
	.blpin = GPIO_Pin_9,
};
#else
const DevLcdBus BusLcdVSpi3={
	.name = "BusLcdVSpi3",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI3_CH1",

	.A0port = MCU_PORT_G,
	.A0pin = GPIO_Pin_4,

	.rstport = MCU_PORT_G,
	.rstpin = GPIO_Pin_7,

	.blport = MCU_PORT_G,
	.blpin = GPIO_Pin_9,
};

#endif 

const DevLcdBus BusLcdI2C1={
	.name = "BusLcdI2C1",
	.type = LCD_BUS_I2C,
	.basebus = "VI2C1",

	/*I2C�ӿڵ�LCD���ߣ�����Ҫ����IO*/

};
	
const DevLcdBus BusLcd8080={
	.name = "BusLcd8080",
	.type = LCD_BUS_8080,
	.basebus = "8080",//��ʹ���ã�8080����ֱ��Ƕ����LCD BUS������

	/*8080 ����A0�ţ�����ȥ*/
	.A0port = MCU_PORT_B,
	.A0pin = GPIO_Pin_15,

	.rstport = MCU_PORT_A,
	.rstpin = GPIO_Pin_15,
	
	.blport = MCU_PORT_B,
	.blpin = GPIO_Pin_15,

};

/* ģ��SPI2������IO��*/
#if 0
const DevLcdBus BusLcdVSpi2CH1={
	.name = "BusLcdVSpi2CH1",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI2_CH1",

	.A0port = MCU_PORT_F,
	.A0pin = GPIO_Pin_8,

	.rstport = MCU_PORT_F,
	.rstpin = GPIO_Pin_13,

	.blport = MCU_PORT_F,
	.blpin = GPIO_Pin_14,
};
#endif
const DevLcdBus BusLcdVSpi1CH2={
	.name = "BusLcdVSpi1CH2",
	.type = LCD_BUS_SPI,
	.basebus = "VSPI1_CH2",

	.A0port = MCU_PORT_D,
	.A0pin = GPIO_Pin_15,

	.rstport = MCU_PORT_A,
	.rstpin = GPIO_Pin_15,

	.blport = MCU_PORT_B,
	.blpin = GPIO_Pin_15,
};


const DevSpiFlash DevSpiFlashCore={
	/*��һ������board_spiflash��SPI FLASH����DEV_SPI_3_2�ϣ��ͺ�δ֪*/
	"board_spiflash", 
	"SPI3_CH2", 
	NULL
};

const DevSpiFlash DevSpiFlashBoard={
	"core_spiflash",  
	"SPI3_CH1", 
	NULL
};

/*
	lcd�豸������
	ָ��ϵͳ�ж��ٸ�LCD�豸�������ĸ�LCD�����ϡ�
*/

/*I2C�ӿڵ� OLED*/
const DevLcd DevLcdOled1={
	.name = "i2coledlcd",  
	.buslcd = "BusLcdI2C1",  
	.id = 0X1315, 
	.width = 64, 
	.height = 128
	};
//LcdObj DevLcdOled2	=	{"i2coledlcd2", LCD_BUS_VI2C2,  0X1315, 64, 128};
/*SPI�ӿڵ� OLED*/
//LcdObj DevLcdOled3	=	{"vspioledlcd", LCD_BUS_VSPI, 	0X1315, 64, 128};
//DevLcd DevLcdOled4	=	{"spioledlcd", 	"BusLcdSpi3", 	0X1315, 64, 128};
/*SPI�ӿڵ� COG LCD*/
//const DevLcd DevLcdCOG1	=	{"spicoglcd", 	"BusLcdSpi3", 	0X7565, 64, 128};
//LcdObj DevLcdCOG2	=	{"vspicoglcd", 	LCD_BUS_VSPI, 	0X7565, 64, 128};
/*fsmc�ӿڵ� tft lcd*/
const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	NULL, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x9325, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x9341, 240, 320};
//const DevLcd DevLcdtTFT	=	{"tftlcd", 		"BusLcd8080", 	0x1408, 480, 800};

/*SPI�ӿڵ� tft lcd*/
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdSpi3", 	0x9342, 240, 320};
//const DevLcd DevLcdtTFT	=	{"spitftlcd", 		"BusLcdVSpi1CH2", 	0x9342, 240, 320};
const DevLcd DevSpiLcdtTFT	=	{"spitftlcd",   "BusLcdSpi3", 	0x7735, 128, 128};

/* ֻ��SCL&SDA��SPI�ӿ�LCD*/
//const DevLcd DevLcdVSPITFT =	{"vspitftlcd",		"BusLcdVSpi3",	0x7789, 240, 240};

/*
	ϵͳ�豸ע��
*/
s32 sys_dev_register(void)
{
	/*ע��I2C����*/
	mcu_i2c_register(&DevVi2c1);

	#ifdef SYS_USE_VI2C2
	//mcu_i2c_register(&DevVi2c2);
	#endif
/*------------------------------------------------*/
	/*ע��SPI������*/
	mcu_spi_register(&DevSpi3IO);
	#ifdef SYS_USE_VSPI1
	mcu_spi_register(&DevVSpi1IO);
	#endif
	#ifdef SYS_USE_VSPI2
	mcu_spi_register(&DevVspi2IO);
	#endif
	mcu_spi_register(&DevVspi3IO);
/*------------------------------------------------*/
	/*ע��SPI ͨ��*/
	mcu_spich_register(&DevSpi3CH1);
	mcu_spich_register(&DevSpi3CH2);
	mcu_spich_register(&DevSpi3CH3);
	//mcu_spich_register(&DevSpi3CH4);
	
	mcu_spich_register(&DevVSpi1CH1);
	mcu_spich_register(&DevVSpi1CH2);
	//mcu_spich_register(&DevVSpi2CH1);
	mcu_spich_register(&DevVSpi3CH1);
/*------------------------------------------------*/	
	/*ע��LCD����*/
	dev_lcdbus_register(&BusLcdSpi3);
	
	dev_lcdbus_register(&BusLcdI2C1);
	dev_lcdbus_register(&BusLcd8080);
	//dev_lcdbus_register(&BusLcdVSpi2CH1);
	//dev_lcdbus_register(&BusLcdVSpi1CH2);
	//dev_lcdbus_register(&BusLcdVSpi3);
/*------------------------------------------------*/	
	/*ע���豸*/

	/*ע��FLASH�豸*/
	dev_spiflash_register(&DevSpiFlashCore);
	dev_spiflash_register(&DevSpiFlashBoard);
	
	/*ע��LCD�豸*/
	dev_lcd_register(&DevLcdOled1);
	dev_lcd_register(&DevLcdtTFT);
	dev_lcd_register(&DevSpiLcdtTFT);
	
	return 0;
}



