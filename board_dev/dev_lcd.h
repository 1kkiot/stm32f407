#ifndef _DEV_LCD_H_
#define _DEV_LCD_H_

#include "dev_lcdbus.h"
#include "font.h"

typedef struct _strDevLcd DevLcd;
/*
	LCD��������
*/
typedef struct  
{	
	u16 id;
	
	s32 (*init)(DevLcd *lcd);
	
	s32 (*draw_point)(DevLcd *lcd, u16 x, u16 y, u16 color);
	s32 (*color_fill)(DevLcd *lcd, u16 sx,u16 ex,u16 sy,u16 ey, u16 color);
	s32 (*fill)(DevLcd *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
	
	s32 (*prepare_display)(DevLcd *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
	
	s32 (*onoff)(DevLcd *lcd, u8 sta);
	void (*set_dir)(DevLcd *lcd, u8 scan_dir);
	void (*backlight)(DevLcd *lcd, u8 sta);
}_lcd_drv; 

/* 
	LCD����, �����豸��һ����
	ͬһ��LCD�ӿڿ��ԽӲ�ͬ��LCD�����в���ͨ������ȡ��
*/
typedef struct
{
	u16 id;
	u16 width;	//LCD ���  ����
	u16 height;	//LCD �߶�    ����
}_lcd_pra;

/*
	�豸����
	�������ط�ʽ����
	Ҳ����˵����һʲôID���豸����ʲô�ط�
	���綨��һ��COG LCD������SPI3��
	��ʲô������LCD���������ʲô��ͨ��IDƥ��
	ͬһ�����͵�LCD��������ͬ��ֻ�����ش�С��һ������δ���
	��������һ�������ṹ�壬����һ����ID��һ����
*/
typedef struct
{
	char *name;//�豸����
	LcdBusType bus;//��������LCD������
	u16 id;
}LcdObj;
/*
	��ʼ����ʱ�������豸�����壬
	����ƥ������������������ʼ��������
	�򿪵�ʱ��ֻ�ǻ�ȡ��һ��ָ��
*/
struct _strDevLcd
{
	s32 gd;//����������Ƿ���Դ�
	
	LcdObj	 *dev;
	/* LCD�������̶������ɱ�*/
	_lcd_pra *pra;
	
	/* LCD���� */
	_lcd_drv *drv;

	/*������Ҫ�ı���*/
	u8  dir;	//���������������ƣ�0��������1��������	
	u8  scandir;//ɨ�跽��
	u16 width;	//LCD ��� 
	u16 height;	//LCD �߶�

	void *pri;//˽�����ݣ��ڰ�����OLED���ڳ�ʼ����ʱ��Ὺ���Դ�
};


#define H_LCD 0//����
#define W_LCD 1//����

//ɨ�跽����
//BIT 0 ��ʶLR��1 R-L��0 L-R
//BIT 1 ��ʶUD��1 D-U��0 U-D
//BIT 2 ��ʶLR/UD��1 DU-LR��0 LR-DU
#define LR_BIT_MASK 0X01
#define UD_BIT_MASK 0X02
#define LRUD_BIT_MASK 0X04

#define L2R_U2D  (0) //������,���ϵ���
#define L2R_D2U  (0 + UD_BIT_MASK)//������,���µ���
#define R2L_U2D  (0 + LR_BIT_MASK) //���ҵ���,���ϵ���
#define R2L_D2U  (0 + UD_BIT_MASK + LR_BIT_MASK) //���ҵ���,���µ���

#define U2D_L2R  (LRUD_BIT_MASK)//���ϵ���,������
#define U2D_R2L  (LRUD_BIT_MASK + LR_BIT_MASK) //���ϵ���,���ҵ���
#define D2U_L2R  (LRUD_BIT_MASK + UD_BIT_MASK) //���µ���,������
#define D2U_R2L  (LRUD_BIT_MASK + UD_BIT_MASK+ LR_BIT_MASK) //���µ���,���ҵ���	 

//������ɫ
/*
	���ںڰ���
	WHITE���ǲ���ʾ�����
	BLACK������ʾ
*/
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  

#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ 
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
//#define LIGHTGRAY      0XEF5B //ǳ��ɫ(PANNEL)
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

extern DevLcd *dev_lcd_open(char *name);
extern s32 dev_lcd_init(void);
extern s32 dev_lcd_drawpoint(DevLcd *lcd, u16 x, u16 y, u16 color);
extern s32 dev_lcd_prepare_display(DevLcd *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
extern s32 dev_lcd_fill(DevLcd *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
extern s32 dev_lcd_color_fill(DevLcd *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
extern s32 dev_lcd_backlight(DevLcd *lcd, u8 sta);
extern s32 dev_lcd_display_onoff(DevLcd *lcd, u8 sta);
extern s32 dev_lcd_put_string(DevLcd *lcd, FontType font, int x, int y, char *s, unsigned colidx);

extern void put_string_center(DevLcd *lcd, int x, int y, char *s, unsigned colidx);
extern s32 dev_lcd_setdir(DevLcd *lcd, u8 dir, u8 scan_dir);

#endif

