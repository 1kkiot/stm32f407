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
#include "string.h"
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "dev_lcdbus.h"
#include "dev_lcd.h"
#include "dev_ILI9341.h"
#include "dev_str7565.h"
#include "alloc.h"

//#define DEV_LCD_DEBUG

#ifdef DEV_LCD_DEBUG
#define LCD_DEBUG	wjq_log 
#else
#define LCD_DEBUG(a, ...)
#endif

s32 LcdMagNum = 95862;

u16 PenColor = BLACK;
u16 BackColor = BLUE;



/*
	����LCD�Ĺ�����
*/
_lcd_pra LCD_IIL9341 ={
		.id	  = 0x9341,
		.width = 240,	//LCD ���
		.height = 320,	//LCD �߶�
};
		
_lcd_pra LCD_IIL9325 ={
		.id   = 0x9325,
		.width = 240,	//LCD ���
		.height = 320, //LCD �߶�
};

_lcd_pra LCD_R61408 ={
		.id   = 0x1408,//
		.width = 480,	//LCD ���
		.height = 800, //LCD �߶�
};


_lcd_pra LCD_Cog12864 ={
		.id   = 0x7565,//
		.width = 64,	//LCD ���
		.height = 128, //LCD �߶�
};
		
_lcd_pra LCD_Cog12832 ={
		.id   = 0x7564,//
		.width = 32,	//LCD ���
		.height = 128, //LCD �߶�
};		
		
_lcd_pra LCD_Oled12864 ={
		.id   = 0x1315,//
		.width = 64,	//LCD ���
		.height = 128, //LCD �߶�
};

/*����LCD�б�*/
_lcd_pra *LcdPraList[5]=
			{
				&LCD_IIL9341,		
				&LCD_IIL9325,
				&LCD_R61408,
				&LCD_Cog12864,
				&LCD_Oled12864,
};

/*
	���������б�
*/
_lcd_drv *LcdDrvList[] = {
					&TftLcdILI9341Drv,
					&TftLcdILI9325Drv,
					&CogLcdST7565Drv,
					&OledLcdSSD1615rv,
};
/*

	���Զ�ʶ��ID������

*/
_lcd_drv *LcdProbDrv8080List[] = {
					&TftLcdILI9341Drv,
					&TftLcdILI9325Drv,
};

/*
	�豸������
	ָ��ϵͳ�ж��ٸ�LCD�豸�������ĸ�LCD�����ϡ�
*/
#define DEV_LCD_C 4//ϵͳ����3��LCD�豸
LcdObj LcdObjList[DEV_LCD_C]=
{
	{"i2coledlcd",  LCD_BUS_I2C,  0X1315},
	{"vspioledlcd", LCD_BUS_VSPI, 0X1315},
	{"spicoglcd",   LCD_BUS_SPI,  0X7565},
	{"tftlcd",      LCD_BUS_8080, NULL},
};


/*LCD�豸�ܽṹ�壬��ʼ���豸ʱ��ʼ��*/
DevLcd DevLcdList[DEV_LCD_C];

/**
 *@brief:      dev_lcd_cpydev
 *@details:    �����豸��Ϣ
 *@param[in]   DevLcd *src  
               DevLcd *dst  
 *@param[out]  ��
 *@retval:     
 */
#if 0
static s32 dev_lcd_cpydev(DevLcd *src, DevLcd *dst)
{
	src->gd = dst->gd;
	src->dev = dst->dev;
	src->pra = dst->pra;
	src->drv = dst->drv;
	src->dir = dst->dir;
	src->scandir = dst->scandir;
	src->width = dst->width;
	src->height = dst->height;
	return 0;
}
#endif
/**
 *@brief:      dev_lcd_findpra
 *@details:    ����ID����LCD����
 *@param[in]   u16 id  
 *@param[out]  ��
 *@retval:     _lcd_pra
 */
static _lcd_pra *dev_lcd_findpra(u16 id)
{
	u8 i =0;
	
	while(1)
	{
		if(LcdPraList[i]->id == id)
		{
			return LcdPraList[i];
		}
		i++;
		if(i>= sizeof(LcdPraList)/sizeof(_lcd_pra *))
		{
			return NULL;
		}
	}
	
}
/**
 *@brief:      dev_lcd_finddrv
 *@details:    ����ID�����豸����
 *@param[in]   u16 id  
 *@param[out]  ��
 *@retval:     _lcd_drv
 */
_lcd_drv *dev_lcd_finddrv(u16 id)
{
	u8 i =0;
	
	while(1)
	{
		if(LcdDrvList[i]->id == id)
		{
			return LcdDrvList[i];
		}
		i++;
		if(i>= sizeof(LcdDrvList)/sizeof(_lcd_drv *))
		{
			return NULL;
		}
	}
}

/**
 *@brief:      dev_lcd_setdir
 *@details:    ���ú�����������ɨ�跽��
 *@param[in]   u8 dir       0 ����1����
               u8 scan_dir  �ο��궨��L2R_U2D       
 *@param[out]  ��
 *@retval:     
 */
s32 dev_lcd_setdir(DevLcd *lcd, u8 dir, u8 scan_dir)
{
	u16 temp;
	u8 scan_dir_tmp;

	if(lcd == NULL)
		return -1;

	
	if(dir != lcd->dir)//�л���Ļ����	
	{
		
		lcd->dir = lcd->dir^0x01;
		temp = lcd->width;
		lcd->width = lcd->height;
		lcd->height = temp;
		LCD_DEBUG(LOG_DEBUG, "set dir w:%d, h:%d\r\n", lcd->width, lcd->height);
	}
	
	
	if(lcd->dir == W_LCD)//������ɨ�跽��ӳ��ת��
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
	
	lcd->scandir = scan_dir_tmp;
	
	lcd->drv->set_dir(lcd, lcd->scandir);
	
	return 0;
}

/**
 *@brief:      dev_lcd_init
 *@details:    ��ʼ��LCD
 			   �����豸������ʼ������LCD
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_lcd_init(void)
{
	s32 ret = -1;
	u8 i = 0;
	LcdObj *pobj;
	DevLcd *pdev;
	
	while(1)
	{	
		pobj = &LcdObjList[i];
		pdev = &DevLcdList[i];
		
		wjq_log(LOG_INFO, "\r\nlcd name:%s\r\n",pobj->name);

		pdev->dev = pobj;
		pdev->gd = -99;//��ʼ���ɹ�������Ϊ-1��
		ret = -1;
		/*
			����ID������������
		*/
		if(pobj->id == NULL)
		{
			LCD_DEBUG(LOG_DEBUG, "prob LCD id\r\n");
			if(pobj->bus == LCD_BUS_8080)
			{
				/*�ҵ����������󣬳�ʼ��*/
				u8 j = 0;

				while(1)
				{
					ret = LcdProbDrv8080List[j]->init(pdev);
					if(ret == 0)
					{
						LCD_DEBUG(LOG_DEBUG, "lcd drv prob ok!\r\n");	
						pdev->drv = LcdProbDrv8080List[j];
						/*
							��������ID�Ҳ���
						*/
						pdev->pra = dev_lcd_findpra(pdev->drv->id);
						break;
					}	
					else
					{
						j++;
						if(j >= sizeof(LcdProbDrv8080List)/sizeof(_lcd_drv *))
						{
							LCD_DEBUG(LOG_DEBUG, "lcd prob err\r\n");
							break;
						}
					}
				}
			}
		}
		else
		{
			pdev->drv = dev_lcd_finddrv(pobj->id);
			if(pdev->drv != NULL)
			{
				
				pdev->pra = dev_lcd_findpra(pobj->id);
				if(pdev->pra != NULL)
				{
					/*�ҵ����������󣬳�ʼ��*/
					ret = pdev->drv->init(pdev);
				}
				else
					LCD_DEBUG(LOG_DEBUG, "lcd find drv fail!\r\n");
			}
			else
				LCD_DEBUG(LOG_DEBUG, "lcd find drv fail!\r\n");


		}

		if(ret == 0)
		{
			pdev->gd = -1;
			
			pdev->dir = H_LCD;
			pdev->height = pdev->pra->height;
			pdev->width = pdev->pra->width;
			dev_lcd_setdir(pdev, W_LCD, L2R_U2D);
			pdev->drv->onoff((pdev),1);//����ʾ
			pdev->drv->color_fill(pdev, 0, pdev->width, 0, pdev->height, BLUE);

			pdev->drv->backlight(pdev, 1);

			wjq_log(LOG_INFO, "lcd init OK\r\n");
		}
		else
		{
			wjq_log(LOG_INFO, "lcd drv init err!\r\n");
		}
		

		i++;
		if(i >= sizeof(LcdObjList)/sizeof(LcdObj))
		{
			wjq_log(LOG_INFO, "lcd init finish\r\n");
			break;
		}
	}

	return 0;
}

/**
 *@brief:      dev_lcd_open
 *@details:    ��LCD
 *@param[in]   char *name  
 *@param[out]  ��
 *@retval:     DevLcd
 */
DevLcd *dev_lcd_open(char *name)
{
	DevLcd *p;
	u8 i;

	i = 0;
	while(1)
	{
		
		p = &DevLcdList[i];
		if(0 == strcmp(name, p->dev->name))
		{
			LCD_DEBUG(LOG_DEBUG, "find lcd\r\n");
			
			if(p->gd == -99)
			{
				LCD_DEBUG(LOG_DEBUG, "lcd dev no init!\r\n");
				return NULL;
			}
			
			return p;

		}
		
		i++;
		if(i>= sizeof(LcdObjList)/sizeof(LcdObj))
			return NULL;
	}

}
/**
 *@brief:      dev_lcd_close
 *@details:    �ر�LCD
 *@param[in]   DevLcd *dev  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_lcd_close(DevLcd *dev)
{
	if(dev->gd <0)
		return -1;
	else
	{
		dev->gd = -1;
		return 0;
	}
}
/*
����-1 ������ԭ��ı仯��
��APP�㣬ԭ���ǣ�1��1��������������ƽ���ˡ�

�������ͻ�Ϊ(0,0)�����۳����ǿ������Դ棬���Ǵӣ�0��0����ʼ

*/
s32 dev_lcd_drawpoint(DevLcd *lcd, u16 x, u16 y, u16 color)
{
	if(lcd == NULL)
		return -1;
	
	return lcd->drv->draw_point(lcd, x-1, y-1, color);
}

s32 dev_lcd_prepare_display(DevLcd *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	if(lcd == NULL)
		return -1;
	
	return lcd->drv->prepare_display(lcd, sx-1, ex-1, sy-1, ey-1);
}


s32 dev_lcd_fill(DevLcd *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{	
	if(lcd == NULL)
		return -1;
	
	return lcd->drv->fill(lcd, sx-1,ex-1,sy-1,ey-1,color);
}
s32 dev_lcd_color_fill(DevLcd *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{
	if(lcd == NULL)
		return -1;
	
	return lcd->drv->color_fill(lcd, sx-1,ex-1,sy-1,ey-1,color);
}
s32 dev_lcd_backlight(DevLcd *lcd, u8 sta)
{
	if(lcd == NULL)
		return -1;
	
	lcd->drv->backlight(lcd, sta);
	return 0;
}
s32 dev_lcd_display_onoff(DevLcd *lcd, u8 sta)
{
	if(lcd == NULL)
		return -1;

	return lcd->drv->onoff(lcd, sta);
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
void line (DevLcd *lcd, int x1, int y1, int x2, int y2, unsigned colidx)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if(lcd == NULL)
		return;

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
			dev_lcd_drawpoint(lcd, x1 >> 16, y1, colidx);
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
			dev_lcd_drawpoint(lcd, x1, y1 >> 16, colidx);
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
void put_cross(DevLcd *lcd, int x, int y, unsigned colidx)
{
	if(lcd == NULL)
		return;
	
	line (lcd, x - 10, y, x - 2, y, colidx);
	line (lcd, x + 2, y, x + 10, y, colidx);
	line (lcd, x, y - 10, x, y - 2, colidx);
	line (lcd, x, y + 2, x, y + 10, colidx);

	line (lcd, x - 6, y - 9, x - 9, y - 9, colidx + 1);
	line (lcd, x - 9, y - 8, x - 9, y - 6, colidx + 1);
	line (lcd, x - 9, y + 6, x - 9, y + 9, colidx + 1);
	line (lcd, x - 8, y + 9, x - 6, y + 9, colidx + 1);
	line (lcd, x + 6, y + 9, x + 9, y + 9, colidx + 1);
	line (lcd, x + 9, y + 8, x + 9, y + 6, colidx + 1);
	line (lcd, x + 9, y - 6, x + 9, y - 9, colidx + 1);
	line (lcd, x + 8, y - 9, x + 6, y - 9, colidx + 1);

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
void put_char(DevLcd *lcd, int x, int y, int c, int colidx)
{
	int i,j,bits;
	u8* p;
	
	if(lcd == NULL)
		return;	
	
	p = (u8*)font_vga_8x8.data;
	for (i = 0; i < font_vga_8x8.height; i++) 
	{
		bits =  p[font_vga_8x8.height * c + i];
		for (j = 0; j < font_vga_8x8.width; j++, bits <<= 1)
		{
			if (bits & 0x80)
			{
				lcd->drv->draw_point(lcd, x + j, y + i, colidx);
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
void put_string(DevLcd *lcd, int x, int y, char *s, unsigned colidx)
{
	int i;
	
	if(lcd == NULL)
		return;	
	
	for (i = 0; *s; i++, x += font_vga_8x8.width, s++)
		put_char(lcd, x, y, *s, colidx);
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
void put_string_center(DevLcd *lcd, int x, int y, char *s, unsigned colidx)
{
	int sl = strlen (s);
	
	if(lcd == NULL)
		return;	
	
    put_string (lcd, x - (sl / 2) * font_vga_8x8.width,
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
void rect (DevLcd *lcd, int x1, int y1, int x2, int y2, unsigned colidx)
{
	if(lcd == NULL)
		return;

	line (lcd, x1, y1, x2, y1, colidx);
	line (lcd, x2, y1, x2, y2, colidx);
	line (lcd, x2, y2, x1, y2, colidx);
	line (lcd, x1, y2, x1, y1, colidx);
}

/**
 *@brief:      dev_lcd_put_string
 *@details:    ��ʾ�ַ�����֧������
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     	
 */
s32 dev_lcd_put_string(DevLcd *lcd, FontType font, int x, int y, char *s, unsigned colidx)
{
	u16 slen;
	u16 xlen,ylen;
	u16 *framebuff;//���㻺�壬����L2R_U2D��ʽ���
	u8 *dotbuf;//�ַ����󻺳�
	s32 res;
	u16 sidx;
	u8 i,j;
	u32 xbase;

	if(lcd == NULL)
		return -1;
	
	/* ͨ��ˢһ���飬�����ʾ�ٶ� */
	slen = strlen(s);
	//uart_printf("str len:%d\r\n", slen);

	/*
		�����ַ������ȼ���ˢ�����򳤿�
	*/
	xlen = slen*FontAscList[font]->width;
	ylen = FontAscList[font]->height;

	framebuff = (u16*)wjq_malloc(xlen*ylen*sizeof(u16));//���㻺��
	dotbuf = (u8*)wjq_malloc(32);//Ҫ��Ϊ�����ֿ���������
	sidx = 0;

	/*��ȡ���󣬲�ת��ΪLCD����*/
	while(1)
	{
		if(*(s+sidx) < 0x81)//Ӣ����ĸ
		{
			//uart_printf("eng\r\n");
			u8 ch;
			/*��ȡ����*/
			ch = *(s+sidx);
			
			res = font_get_asc(font, &ch, dotbuf);
			//PrintFormat(dotbuf, 16);
			/*asc�Ǻ��*/

			for(j=0;j<FontAscList[font]->height;j++)
			{

				xbase = xlen*j + sidx*FontAscList[font]->width;//��ǰ�ַ�X��ƫ����
				for(i=0;i<FontAscList[font]->width;i++)
				{
					/*��ʱֻ����6*12��8*16��ASC��ÿһ��1���ֽ�*/
					if((dotbuf[j*1+i/8]&(0x80>>(i%8)))!= 0)
					{
						//uart_printf("* ");
						framebuff[xbase + i] = colidx;
					}
					else
					{
						//uart_printf("- ");
						framebuff[xbase + i] = BackColor;
					}
				}
				//uart_printf("\r\n");
			}	
			
			sidx++;
		}
		else//����
		{
			//uart_printf("ch\r\n");
			res = font_get_hz(font, s+sidx, dotbuf);//��SD����ȡһ��1616���ֵĵ���Ҫ1ms
			//PrintFormat(dotbuf, 32);

			/*����֧���ݿ⣬ȡģ��ʽ2,16*16*/
			for(j=0; j<FontList[font]->height; j++)
			{
				xbase = xlen*j + sidx*FontAscList[font]->width;//��ǰ�ַ�X��ƫ����
				for(i=0;i<FontList[font]->width;i++)
				{
					/*��ʱֻ��1212��1616��ÿһ��2���ֽ�����*/
					if((dotbuf[i*2+j/8]&(0x80>>(j%8)))!= 0)
					{
						//uart_printf("* ");
						framebuff[xbase + i] = colidx;
					}
					else
					{
						//uart_printf("- ");
						framebuff[xbase + i] = BackColor;
					}
				}
				//uart_printf("\r\n");
			}	
			
			sidx+= 2;
		}

		if(sidx >= slen)
		{
			//uart_printf("finish");
			break;
		}
	}
	

	dev_lcd_fill(lcd, x, x + xlen-1, y, y + ylen-1, framebuff);

	wjq_free(framebuff);
	wjq_free(dotbuf);

	return 0;	
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
	DevLcd *LcdCog = NULL;
	DevLcd *LcdOled = NULL;
	DevLcd *LcdOledI2C = NULL;
	DevLcd *LcdTft = NULL;

	/*  �������豸 */
	LcdCog = dev_lcd_open("spicoglcd");
	if(LcdCog==NULL)
		wjq_log(LOG_FUN, "open cog lcd err\r\n");

	LcdOled = dev_lcd_open("vspioledlcd");
	if(LcdOled==NULL)
		wjq_log(LOG_FUN, "open oled lcd err\r\n");
	
	LcdTft = dev_lcd_open("tftlcd");
	if(LcdTft==NULL)
		wjq_log(LOG_FUN, "open tft lcd err\r\n");

	LcdOledI2C = dev_lcd_open("i2coledlcd");
	if(LcdOledI2C==NULL)
		wjq_log(LOG_FUN, "open oled i2c lcd err\r\n");
	
	/*�򿪱���*/
	dev_lcd_backlight(LcdCog, 1);
	dev_lcd_backlight(LcdOled, 1);
	dev_lcd_backlight(LcdOledI2C, 1);
	dev_lcd_backlight(LcdTft, 1);

	#if 0/*��֧�ֺ���ʱ*/
	put_string(LcdCog, 5, 5, "spi cog lcd", BLACK);
	put_string(LcdOled, 5, 5, "vspi oled lcd", BLACK);
	put_string(LcdOledI2C, 5, 5, "i2c oled lcd", BLACK);
	put_string(LcdTft, 5, 5, "2.8 tft lcd", BLACK);
	#endif

	#if 1
	dev_lcd_put_string(LcdOled, FONT_SONGTI_1212, 10,1, "ABC-abc��", BLACK);
	dev_lcd_put_string(LcdOled, FONT_SIYUAN_1616, 1, 13, "����oled lcd", BLACK);
	dev_lcd_put_string(LcdOled, FONT_SONGTI_1212, 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOled, FONT_SIYUAN_1616, 1, 47, "�ݼ�ȸ������", BLACK);

	dev_lcd_put_string(LcdCog, FONT_SONGTI_1212, 10,1, "ABC-abc��", BLACK);
	dev_lcd_put_string(LcdCog, FONT_SIYUAN_1616, 1, 13, "����cog lcd", BLACK);
	dev_lcd_put_string(LcdCog, FONT_SONGTI_1212, 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdCog, FONT_SIYUAN_1616, 1, 47, "�ݼ�ȸ������", BLACK);

	dev_lcd_put_string(LcdTft, FONT_SONGTI_1212, 20,30, "ABC-abc��", RED);
	dev_lcd_put_string(LcdTft, FONT_SIYUAN_1616, 20,60, "����tft lcd", RED);
	dev_lcd_put_string(LcdTft, FONT_SONGTI_1212, 20,100, "www.wujique.com", RED);
	dev_lcd_put_string(LcdTft, FONT_SIYUAN_1616, 20,150, "�ݼ�ȸ������", RED);

	dev_lcd_put_string(LcdOledI2C, FONT_SONGTI_1212, 10,1, "ABC-abc��", BLACK);
	dev_lcd_put_string(LcdOledI2C, FONT_SIYUAN_1616, 1,13, "����LcdOledI2C", BLACK);
	dev_lcd_put_string(LcdOledI2C, FONT_SONGTI_1212, 10,30, "www.wujique.com", BLACK);
	dev_lcd_put_string(LcdOledI2C, FONT_SIYUAN_1616, 1,47, "�ݼ�ȸ������", BLACK);
	#endif
	
	while(1);
}


