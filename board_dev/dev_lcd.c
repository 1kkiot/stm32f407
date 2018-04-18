/**
 * @file            dev_lcd.c
 * @brief           LCD �м��
 * @author          wujique
 * @date            2018��4��17�� ���ڶ�
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��4��17�� ���ڶ�
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
#include "wujique_log.h"
#include "dev_lcd.h"

//#define DEV_LCD_DEBUG

#ifdef DEV_LCD_DEBUG
#define LCD_DEBUG	wjq_log 
#else
#define LCD_DEBUG(a, ...)
#endif


struct _strlcd_obj LCD;

extern _lcd_drv TftLcdILI9341Drv;
extern _lcd_drv TftLcdILI9325Drv;
extern _lcd_drv CogLcdST7565Drv;
extern _lcd_drv OledLcdSSD1615rv;

/**
 *@brief:      dev_lcd_setdir
 *@details:    ���ú�����������ɨ�跽��
 *@param[in]   u8 dir       0 ����1����
               u8 scan_dir  �ο��궨��L2R_U2D       
 *@param[out]  ��
 *@retval:     
 */
void dev_lcd_setdir(u8 dir, u8 scan_dir)
{
	struct _strlcd_obj *obj;
	obj = &LCD;

	u16 temp;
	u8 scan_dir_tmp;
	
	if(dir != obj->dir)//�л���Ļ����	
	{
		
		obj->dir = obj->dir^0x01;
		temp = obj->width;
		obj->width = obj->height;
		obj->height = temp;
		LCD_DEBUG(LOG_DEBUG, "set dir w:%d, h:%d\r\n", obj->width, obj->height);
	}
	
	
	if(obj->dir == W_LCD)//������ɨ�跽��ӳ��ת��
	{
		/*
			����	 ����
			LR----UD
			RL----DU
			UD----RL
			DU----LR
			UDLR----LRUD
		*/
		scan_dir_tmp = 0;
		if((scan_dir&LRUD_BIT_MASK) == 0)
		{
			scan_dir_tmp += LRUD_BIT_MASK;
		}

		if((scan_dir&LR_BIT_MASK) == LR_BIT_MASK)
		{
			scan_dir_tmp += UD_BIT_MASK;	
		}

		if((scan_dir&UD_BIT_MASK) == 0)
		{
			scan_dir_tmp += LR_BIT_MASK;
		}
	}
	else
	{
		scan_dir_tmp = scan_dir;
	}
	
	obj->scandir = scan_dir_tmp;
	
	obj->drv->set_dir(obj->scandir);
}


s32 dev_lcd_init(void)
{
	s32 ret = -1;

	#if 0
	/*��ʼ��8080�ӿڣ����������ź�*/
	bus_8080interface_init();

	if(ret != 0)
	{
		/*���Գ�ʼ��9341*/
		ret = drv_ILI9341_init();
		if(ret == 0)
		{
			LCD.drv = &TftLcdILI9341Drv;//��9341������ֵ��LCD
			LCD.dir = H_LCD;//Ĭ������
			LCD.height = 320;
			LCD.width = 240;
		}
	}
	

	if(ret != 0)
	{
		/* ���Գ�ʼ��9325 */
		ret = drv_ILI9325_init();
		if(ret == 0)
		{
			LCD.drv = &TftLcdILI9325Drv;
			LCD.dir = H_LCD;
			LCD.height = 320;
			LCD.width = 240;
		}
	}

	#else
	#if 1
	if(ret != 0)
	{
		/* ��ʼ��COG 12864 LCD */
		ret = drv_ST7565_init();
		if(ret == 0)
		{
			LCD.drv = &CogLcdST7565Drv;
			LCD.dir = W_LCD;
			LCD.height = 64;
			LCD.width = 128;
		}
	}
	#else
	if(ret != 0)
	{
		/* ��ʼ��OLED LCD */
		ret = drv_ssd1615_init();
		if(ret == 0)
		{
			LCD.drv = &OledLcdSSD1615rv;
			LCD.dir = W_LCD;
			LCD.height = 64;
			LCD.width = 128;
		}
	}
	#endif
	
	#endif
	/*������Ļ����ɨ�跽��*/
	dev_lcd_setdir(W_LCD, U2D_L2R);
	LCD.drv->onoff(1);//����ʾ
	bus_8080_lcd_bl(1);//�򿪱���	
	LCD.drv->color_fill(0, LCD.width, 0, LCD.height, YELLOW);
	
	return 0;
}


s32 dev_lcd_drawpoint(u16 x, u16 y, u16 color)
{
	return LCD.drv->draw_point(x, y, color);
}

s32 dev_lcd_prepare_display(u16 sx, u16 ex, u16 sy, u16 ey)
{
	return LCD.drv->prepare_display(sx, ex, sy, ey);
}

s32 dev_lcd_display_onoff(u8 sta)
{
	return LCD.drv->onoff(sta);
}
/* 

��tslib����һЩ��ʾ����������
��Щ�������Թ�ΪGUI


*/
#include "font.h"

/**
 *@brief:      line
 *@details:    ��һ����
 *@param[in]   int x1           
               int y1           
               int x2           
               int y2           
               unsigned colidx  
 *@param[out]  ��
 *@retval:     
 */
void line (int x1, int y1, int x2, int y2, unsigned colidx)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (abs (dx) < abs (dy)) 
	{
		if (y1 > y2) 
		{
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2)
		{
			LCD.drv->draw_point (x1 >> 16, y1, colidx);
			x1 += dx;
			y1++;
		}
	} 
	else 
	{
		if (x1 > x2) 
		{
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) 
		{
			LCD.drv->draw_point (x1, y1 >> 16, colidx);
			y1 += dy;
			x1++;
		}
	}
}

/**
 *@brief:     put_cross
 *@details:   ��ʮ��
 *@param[in]  int x            
              int y            
              unsigned colidx  
 *@param[out]  ��
 *@retval:     
 */
void put_cross(int x, int y, unsigned colidx)
{
	line (x - 10, y, x - 2, y, colidx);
	line (x + 2, y, x + 10, y, colidx);
	line (x, y - 10, x, y - 2, colidx);
	line (x, y + 2, x, y + 10, colidx);

	line (x - 6, y - 9, x - 9, y - 9, colidx + 1);
	line (x - 9, y - 8, x - 9, y - 6, colidx + 1);
	line (x - 9, y + 6, x - 9, y + 9, colidx + 1);
	line (x - 8, y + 9, x - 6, y + 9, colidx + 1);
	line (x + 6, y + 9, x + 9, y + 9, colidx + 1);
	line (x + 9, y + 8, x + 9, y + 6, colidx + 1);
	line (x + 9, y - 6, x + 9, y - 9, colidx + 1);
	line (x + 8, y - 9, x + 6, y - 9, colidx + 1);

}
/**
 *@brief:      put_char
 *@details:    ��ʾһ��Ӣ��
 *@param[in]   int x       
               int y       
               int c       
               int colidx  
 *@param[out]  ��
 *@retval:     
 */
void put_char(int x, int y, int c, int colidx)
{
	int i,j,bits;

	for (i = 0; i < font_vga_8x8.height; i++) 
	{
		bits = font_vga_8x8.data [font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
		{
			if (bits & 0x80)
			{
				LCD.drv->draw_point(x + j, y + i, colidx);
			}
		}
	}
}
/**
 *@brief:      put_string
 *@details:    ��ʾһ���ַ���
 *@param[in]   int x            
               int y            
               char *s          
               unsigned colidx  
 *@param[out]  ��
 *@retval:     
 */
void put_string(int x, int y, char *s, unsigned colidx)
{
	int i;
	
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char(x, y, *s, colidx);
}
/**
 *@brief:      put_string_center
 *@details:    ������ʾһ���ַ���
 *@param[in]   int x            
               int y            
               char *s          
               unsigned colidx  
 *@param[out]  ��
 *@retval:     
 */
void put_string_center(int x, int y, char *s, unsigned colidx)
{
	int sl = strlen (s);
	
    put_string (x - (sl / 2) * font_vga_8x8.width,
                y - font_vga_8x8.height / 2, s, colidx);
}

/**
 *@brief:      rect
 *@details:    ��һ�����ο�
 *@param[in]   int x1           
               int y1           
               int x2           
               int y2           
               unsigned colidx  
 *@param[out]  ��
 *@retval:     
 */
void rect (int x1, int y1, int x2, int y2, unsigned colidx)
{
	line (x1, y1, x2, y1, colidx);
	line (x2, y1, x2, y2, colidx);
	line (x2, y2, x1, y2, colidx);
	line (x1, y2, x1, y1, colidx);
}


/**
 *@brief:      dev_lcd_test
 *@details:    LCD���Ժ���
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void dev_lcd_test(void)
{

	while(1)
	{		
		#if 0//���Բ���
		LCD.drv->color_fill(0,LCD.width,0,LCD.height,BLUE);
		Delay(1000);
		LCD.drv->color_fill(0,LCD.width/2,0,LCD.height/2,RED);
		Delay(1000);
		LCD.drv->color_fill(0,LCD.width/4,0,LCD.height/4,GREEN);
		Delay(1000);
		
		put_string_center (LCD.width/2+50, LCD.height/2+50,
			   "ADCD WUJIQUE !", 0xF800);
		Delay(1000);
		#else//����COG LCD��OLED LCD
		put_string_center (20, 32,
			   "ADCD WUJIQUE !", BLACK);
		Delay(1000);
		LCD.drv->color_fill(0,LCD.width,0,LCD.height,WHITE);
		Delay(1000);
		LCD.drv->color_fill(0,LCD.width,0,LCD.height,BLACK);
		Delay(1000);
		LCD.drv->color_fill(0,LCD.width,0,LCD.height,WHITE);
		Delay(1000);
		#endif
	}

}

