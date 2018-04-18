#ifndef _DEV_LCD_H_
#define _DEV_LCD_H_
/*
	LCD��������
*/
typedef struct  
{	
	u16 id;
	
	s32 (*init)(void);
	s32 (*draw_point)(u16 x, u16 y, u16 color);
	s32 (*color_fill)(u16 sx,u16 ex,u16 sy,u16 ey, u16 color);
	s32 (*fill)(u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
	s32 (*onoff)(u8 sta);
	s32 (*prepare_display)(u16 sx, u16 ex, u16 sy, u16 ey);
	void (*set_dir)(u8 scan_dir);
	void (*backlight)(u8 sta);
}_lcd_drv; 


struct _strlcd_obj
{
	_lcd_drv *drv;

	u8  dir;	//���������������ƣ�0��������1��������	
	u8  scandir;//ɨ�跽��
	u16 width;	//LCD ��� 
	u16 height;	//LCD �߶�

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


extern struct _strlcd_obj LCD;

extern s32 dev_lcd_init(void);
extern void dev_lcd_test(void);


#endif

