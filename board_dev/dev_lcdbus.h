#ifndef _DEV_LCDBUS_H_
#define _DEV_LCDBUS_H_

/*
ϵͳ�ܹ�������LCD����
*/
typedef enum{
	LCD_BUS_NULL = 0,
	LCD_BUS_SPI,
	LCD_BUS_VSPI,
	LCD_BUS_I2C,//OLEDʹ�ã�ֻҪ�����ߣ�����Ҳ����Ҫ���ƣ���λҲ����Ҫ
	LCD_BUS_8080,
	LCD_BUS_MAX
}LcdBusType;


/*
	LCD�ӿڶ���
*/
typedef struct  
{	
	char * name;
	
	s32 (*init)(void);
	s32 (*open)(void);
	s32 (*close)(void);
	s32 (*writedata)(u8 *data, u16 len);
	s32 (*writecmd)(u8 cmd);
	s32 (*bl)(u8 sta);
}_lcd_bus; 

extern _lcd_bus *dev_lcdbus_find(LcdBusType bus);
extern s32 dev_lcdbus_init(void);
#endif

