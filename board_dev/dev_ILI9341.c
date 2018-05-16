/**
 * @file            dev_ILI9341.c
 * @brief           TFT LCD ����оƬILI6341��������
 * @author          wujique
 * @date            2017��11��8�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2017��11��8�� ������
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
#include "stdlib.h"
#include "string.h"

#include "stm32f4xx.h"
#include "stm324xg_eval_fsmc_sram.h"
#include "wujique_log.h"
#include "dev_lcdbus.h"
#include "dev_lcd.h"
#include "dev_ILI9341.h"

#define DEV_ILI9341_DEBUG

#ifdef DEV_ILI9341_DEBUG
#define ILI9341_DEBUG	wjq_log 
#else
#define ILI9341_DEBUG(a, ...)
#endif

extern void Delay(__IO uint32_t nTime);

/*
8080 ���ߣ�ֱ�Ӳ������ٶȿ죬�����ߺ��������ܶ�
����SPI�ӿڷ�����
	�ڱ��������жϽӿڣ������SPI���͵���LCD BUS������
	�����8080����ֱ�Ӳ�����ַ��
*/
extern u16 *LcdReg;
extern u16 *LcdData;


/*

	���ļ�������Ȼ��9341������������ʱ������в�������LCD����

*/
#define TFT_LCD_DRIVER_9341
#define TFT_LCD_DRIVER_9325

/* ----------����Ϊ��ͬLCD ����--------*/

/*

	9341����

*/
#ifdef TFT_LCD_DRIVER_9341
/*9341�����*/
#define ILI9341_CMD_WRAM 0x2c
#define ILI9341_CMD_SETX 0x2a
#define ILI9341_CMD_SETY 0x2b

s32 drv_ILI9341_init(DevLcdNode *lcd);
static s32 drv_ILI9341_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ILI9341_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ILI9341_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_ILI9341_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ILI9341_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ILI9341_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ILI9341_lcd_bl(DevLcdNode *lcd, u8 sta);

/*

	����һ��TFT LCD��ʹ��ILI9341����IC���豸

*/
_lcd_drv TftLcdILI9341Drv = {
							.id = 0X9341,

							.init = drv_ILI9341_init,
							.draw_point = drv_ILI9341_drawpoint,
							.color_fill = drv_ILI9341_color_fill,
							.fill = drv_ILI9341_fill,
							.onoff = drv_ILI9341_display_onoff,
							.prepare_display = drv_ILI9341_prepare_display,
							.set_dir = drv_ILI9341_scan_dir,
							.backlight = drv_ILI9341_lcd_bl
							};

void drv_ILI9341_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}
	
/**
 *@brief:      drv_ILI9341_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_ILI9341_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;

	/*���ô���ߵ��ұ߻����ұߵ����*/
	switch(dir)
	{
		case R2L_U2D:
		case R2L_D2U:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<6); 
			break;	 
	}

	/*���ô��ϵ��»��Ǵ��µ���*/
	switch(dir)
	{
		case L2R_D2U:
		case R2L_D2U:
		case D2U_L2R:
		case D2U_R2L:
			regval|=(1<<7); 
			break;	 
	}

	/*
		���������һ��������� Reverse Mode
		�������Ϊ1��LCD�������Ѿ����и��жԵ��ˣ�
		�����Ҫ����ʾ�н��е���
	*/
	switch(dir)
	{
		case U2D_L2R:
		case D2U_L2R:
		case U2D_R2L:
		case D2U_R2L:
			regval|=(1<<5);
			break;	 
	}
	/*
		����������RGB����GBR
		���������õ�ת����
	*/	
	regval|=(1<<3);//0:GBR,1:RGB  ��R61408�෴

	
	DevLcdBusNode * node;
	u8 tmp[2];
	tmp[0] = (regval);
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_cmd(node, (0x36));
	bus_lcd_write_data(node, tmp, 1);
	bus_lcd_close(node);
	
}

/**
 *@brief:      drv_ILI9341_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_set_cp_addr(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep)
{

	DevLcdBusNode * node;
	u8 tmp[4];

	node = bus_lcd_open(lcd->dev.buslcd);
	
	bus_lcd_write_cmd(node, (ILI9341_CMD_SETX));
	tmp[0] = (sc>>8);
	tmp[1] = (sc&0XFF);
	tmp[2] = (ec>>8);
	tmp[3] = (ec&0XFF);
	bus_lcd_write_data(node, tmp, 4);

	bus_lcd_write_cmd(node, (ILI9341_CMD_SETY));
	tmp[0] = (sp>>8);
	tmp[1] = (sp&0XFF);
	tmp[2] = (ep>>8);
	tmp[3] = (ep&0XFF);
	bus_lcd_write_data(node, tmp, 4);

	bus_lcd_write_cmd(node, (ILI9341_CMD_WRAM));
	
	bus_lcd_close(node);
	
	return 0;
}

/**
 *@brief:      drv_ILI9341_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9341_display_onoff(DevLcdNode *lcd, u8 sta)
{
	if(sta == 1)
		*LcdReg	= 0x29;
	else
		*LcdReg	= 0x28;

	return 0;
}

/**
 *@brief:      drv_ILI9341_init
 *@details:    ��ʼ��FSMC�����Ҷ�ȡILI9341���豸ID
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_init(DevLcdNode *lcd)
{
	u16 data;
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);

	*LcdReg = 0x00d3;
	data = *LcdData; //dummy read
	data = *LcdData; //���� 0X00
	data = *LcdData; //��ȡ 93 
	data<<=8;
	data |= *LcdData; //��ȡ 41

	ILI9341_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);

	
	if(data != TftLcdILI9341Drv.id)
	{
		ILI9341_DEBUG(LOG_DEBUG, "lcd drive no 9341\r\n");	
		bus_lcd_close(node);
		return -1;
	}

	*LcdReg=0xCF ;//Power control B
	*LcdData = 0x00;*LcdData = 0xC1;*LcdData = 0x30;

	*LcdReg = 0xED;//Power on sequence control 
	*LcdData = 0x64;*LcdData = 0x03;*LcdData = 0x12;*LcdData = 0x81;

	*LcdReg = 0xE8;	//Driver timing control A
	*LcdData = 0x85;*LcdData = 0x01;*LcdData = 0x7A;

	*LcdReg = 0xCB;//Power control 
	*LcdData = 0x39;*LcdData = 0x2C;*LcdData = 0x00;
	*LcdData = 0x34;*LcdData = 0x02;

	*LcdReg = 0xF7;//Pump ratio control
	*LcdData = 0x20;

	*LcdReg = 0xEA;//Driver timing control
	*LcdData = 0x00;*LcdData = 0x00;

	*LcdReg = 0xC0; //Power control
	*LcdData = 0x21; //VRH[5:0]

	*LcdReg = 0xC1; //Power control
	*LcdData = 0x11; //SAP[2:0];BT[3:0]

	*LcdReg = 0xC5; //VCM control
	*LcdData = 0x31;*LcdData = 0x3C;

	*LcdReg = 0xC7; //VCM control2
	*LcdData = 0x9f;

	*LcdReg = 0x36; // Memory Access Control
	*LcdData = 0x08;

	*LcdReg = 0x3A; // Memory Access Control
	*LcdData = 0x55;

	*LcdReg = 0xB1;
	*LcdData = 0x00;*LcdData = 0x1B;

	*LcdReg = 0xB6; // Display Function Control
	*LcdData = 0x0A;*LcdData = 0xA2;

	*LcdReg = 0xF2; // 3Gamma Function Disable
	*LcdData = 0x00;

	*LcdReg = 0x26; //Gamma curve selected
	*LcdData = 0x01;

	*LcdReg = 0xE0; //Set Gamma
	*LcdData = 0x0F;*LcdData = 0x20;*LcdData = 0x1d;*LcdData = 0x0b;
	*LcdData = 0x10;*LcdData = 0x0a;*LcdData = 0x49;*LcdData = 0xa9;
	*LcdData = 0x3b;*LcdData = 0x0a;*LcdData = 0x15;*LcdData = 0x06;
	*LcdData = 0x0c;*LcdData = 0x06;*LcdData = 0x00;
	
	*LcdReg = 0XE1; //Set Gamma
	*LcdData = 0x00;*LcdData = 0x1f;*LcdData = 0x22;*LcdData = 0x04;
	*LcdData = 0x0f;*LcdData = 0x05;*LcdData = 0x36;*LcdData = 0x46;
	*LcdData = 0x46;*LcdData = 0x05;*LcdData = 0x0b;*LcdData = 0x09;
	*LcdData = 0x33;*LcdData = 0x39;*LcdData = 0x0F;

	*LcdReg = 0x11; // Sleep out
	bus_lcd_close(node);
	
	Delay(120);
	
	return 0;
}
/**
 *@brief:      drv_ILI9341_xy2cp
 *@details:    ��xy����ת��ΪCP����
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
{
	/*
		��ʾXY�᷶Χ
	*/
	if(sx > lcd->width)
		sx = lcd->width;
	
	if(ex > lcd->width)
		ex = lcd->width;
	
	if(sy > lcd->height)
		sy = lcd->height;
	
	if(ey > lcd->height)
		ey = lcd->height;
	/*
		XY�ᣬʵ��Ƕ�����������ȡ���ں�����������
		CP�ᣬ�ǿ������Դ�Ƕȣ�
		XY���ӳ���ϵȡ����ɨ�跽��
	*/
	if(
		(((lcd->scandir&LRUD_BIT_MASK) == LRUD_BIT_MASK)
		&&(lcd->dir == H_LCD))
		||
		(((lcd->scandir&LRUD_BIT_MASK) == 0)
		&&(lcd->dir == W_LCD))
		)
		{
			*sc = sy;
			*ec = ey;
			*sp = sx;
			*ep = ex;
		}
	else
	{
		*sc = sx;
		*ec = ex;
		*sp = sy;
		*ep = ey;
	}
	
	return 0;
}
/**
 *@brief:      drv_ILI9341_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9341_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, x, x, y, y, &sc,&ec,&sp,&ep);
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);
	
	*LcdData = color; 
	return 0;
}
/**
 *@brief:      drv_ILI9341_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i,j;
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(ep+1)-sp;//�߶�
	
	//uart_printf("ili9341 width:%d, height:%d\r\n", width, height);

	DevLcdBusNode * node;
#define TMP_BUF_SIZE 32
	u16 tmp[TMP_BUF_SIZE];
	u32 cnt;

	for(cnt = 0; cnt < TMP_BUF_SIZE; cnt ++)
	{
		tmp[cnt] = color;
	}
	
	cnt = height*width;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	while(1)
	{
		if(cnt < TMP_BUF_SIZE)
		{
			bus_lcd_write_data(node, (u8 *)tmp, cnt);
			cnt -= cnt;
		}
		else
		{
			bus_lcd_write_data(node, (u8 *)tmp, TMP_BUF_SIZE);
			cnt -= TMP_BUF_SIZE;
		}

		if(cnt <= 0)
			break;
	}
	
	bus_lcd_close(node);

	return 0;

}

/**
 *@brief:      drv_ILI9341_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9341_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u32 i,j;
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);

	width=(ec+1)-sc;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(ep+1)-sp;//�߶�
	
	DevLcdBusNode * node;

	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(node, (u8 *)color, height*width);	
	bus_lcd_close(node);	 

	return 0;

} 

s32 drv_ILI9341_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 sc,ec,sp,ep;

	drv_ILI9341_xy2cp(lcd, sx, ex, sy, ey, &sc,&ec,&sp,&ep);
	drv_ILI9341_set_cp_addr(lcd, sc, ec, sp, ep);	
	return 0;
}
#endif

//----------------------------------------------------------------------
/*

	9325

*/
#ifdef TFT_LCD_DRIVER_9325

/*
	9325�ص㣺
	9325��ʹ��page��colum��ʹ��Horizontal��Vertical���������ǵ�ģ�飬H��240�̱ߣ�V��320����
	1 ����ɨ����ʼ��ַ,  Ҳ���� H, V��ַ�� HV��ַ��ɲ���AD
	  AD�Ͱ�λ��ӦH���߰�λ��ӦV
	  
	2 ɨ�贰�ڵ�ַ��
	  9325����HSA/HEA/VSA/VEA
	  ����HҪ���ڵ���4
	  ��00��h �� HSA[7:0]< HEA[7:0] �� ��EF��h. and ��04��h�QHEA-HAS
	  ��000��h �� VSA[8:0]< VEA[8:0] �� ��13F��h.

	3 ������ɨ�跽�������»���������ʱ��H��V���䡣

	4 ����ɨ�跽����ô�䣬ԭ�㶼�����������Ͻǡ�������Ļ��ɨ�跽�����㡣
	���磬����Ϊ���������ң��ϵ���
	dev_tftlcd_setdir( W_LCD, L2R_U2D);
	Ȼ��ˢ��һ�����ݿ�
	TftLcd->color_fill(0, TftLcd->width/4, 0 , TftLcd->height/4, RED);
	������Ļˢ�µ��Ǻ��������Ͻǣ�
	9325ˢ���������½ǣ�
	��������ˢ�����һ�У�Ҳ���ǵ�0�У������Ǵ����ҡ�
	Ȼ����ˢ����һ�У�Ҳ�Ǵ����ҡ�
	�����Ҫ��ɨ�������ΪXY������
	
	
*/
#define ILI9325_CMD_WRAM 0x22
#define ILI9325_CMD_SETV 0x21
#define ILI9325_CMD_SETH 0x20


s32 drv_ILI9325_init(DevLcdNode *lcd);
static s32 drv_ILI9325_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color);
s32 drv_ILI9325_color_fill(DevLcdNode *lcd, u16 sx,u16 sy,u16 ex,u16 ey,u16 color);
s32 drv_ILI9325_fill(DevLcdNode *lcd, u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);
static s32 drv_ILI9325_display_onoff(DevLcdNode *lcd, u8 sta);
s32 drv_ILI9325_prepare_display(DevLcdNode *lcd, u16 sc, u16 ec, u16 sp, u16 ep);
static void drv_ILI9325_scan_dir(DevLcdNode *lcd, u8 dir);
void drv_ILI9325_lcd_bl(DevLcdNode *lcd, u8 sta);
/*

	9325����

*/
_lcd_drv TftLcdILI9325Drv = {
							.id = 0X9325,

							.init = drv_ILI9325_init,
							.draw_point = drv_ILI9325_drawpoint,
							.color_fill = drv_ILI9325_color_fill,
							.fill = drv_ILI9325_fill,
							.onoff = drv_ILI9325_display_onoff,
							.prepare_display = drv_ILI9325_prepare_display,
							.set_dir = drv_ILI9325_scan_dir,
							.backlight = drv_ILI9325_lcd_bl
							};
void drv_ILI9325_lcd_bl(DevLcdNode *lcd, u8 sta)
{
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_bl(node, sta);
	bus_lcd_close(node);

}	
/**
 *@brief:	   drv_ILI9325_scan_dir
 *@details:    �����Դ�ɨ�跽��
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:	   static OK
 */
static void drv_ILI9325_scan_dir(DevLcdNode *lcd, u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
	
	switch(dir)
    {
        case L2R_U2D://������,���ϵ���
            regval|=(1<<5)|(1<<4)|(0<<3); 
            break;
        case L2R_D2U://������,���µ���
            regval|=(0<<5)|(1<<4)|(0<<3); 
            break;
        case R2L_U2D://���ҵ���,���ϵ���
            regval|=(1<<5)|(0<<4)|(0<<3);
            break;
        case R2L_D2U://���ҵ���,���µ���
            regval|=(0<<5)|(0<<4)|(0<<3); 
            break;   
        case U2D_L2R://���ϵ���,������
            regval|=(1<<5)|(1<<4)|(1<<3); 
            break;
        case U2D_R2L://���ϵ���,���ҵ���
            regval|=(1<<5)|(0<<4)|(1<<3); 
            break;
        case D2U_L2R://���µ���,������
            regval|=(0<<5)|(1<<4)|(1<<3); 
            break;
        case D2U_R2L://���µ���,���ҵ���
            regval|=(0<<5)|(0<<4)|(1<<3); 
            break;   
    }
	
    dirreg=0X03;
    regval|=1<<12;  
    
	*LcdReg  = dirreg; 
	*LcdData = regval; 
}

/**
 *@brief:      drv_ILI9325_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     9325����ɨ��������һ�����ƣ��Ǿ��Ǵ��ڿ�Ȳ���С����
 */
static s32 drv_ILI9325_set_cp_addr(DevLcdNode *lcd, u16 hsa, u16 hea, u16 vsa, u16 vea)
{
	u16 heatmp;

	/* ����ɨ�贰�� */
	if((hsa+4) > hea)
		heatmp = hsa+4;
	else
		heatmp = hea;
	
	*LcdReg = 0x0050;//HSA
	*LcdData = hsa;
	
	*LcdReg = 0x0051;//HEA
	*LcdData = heatmp;

	*LcdReg = 0x0052;//VSA
	*LcdData = vsa;
	
	*LcdReg = 0x0053;//VEA
	*LcdData = vea;

	/*
		����ɨ����ʼ��ַ��
	*/
	if((lcd->scandir&LR_BIT_MASK) == LR_BIT_MASK)
	{
		*LcdReg  = ILI9325_CMD_SETH; 
		*LcdData = hea&0XFF; 
	}
	else
	{
		*LcdReg  = ILI9325_CMD_SETH; 
		*LcdData = hsa&0XFF; 	  
	}

	if((lcd->scandir&UD_BIT_MASK) == UD_BIT_MASK)
	{
		*LcdReg  = ILI9325_CMD_SETV;  
		*LcdData = vea&0X1FF;
	}
	else
	{
		*LcdReg  = ILI9325_CMD_SETV;  
		*LcdData = vsa&0X1FF;
	}
	
	*LcdReg  = ILI9325_CMD_WRAM; 
	
	return 0;
}

/**
 *@brief:	   drv_ILI9325_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:	   static OK
 */
static s32 drv_ILI9325_display_onoff(DevLcdNode *lcd, u8 sta)
{
	if(sta == 1)
	{

		*LcdReg = 0X07;
		*LcdData = 0x0173;
	}
	else
	{
		*LcdReg = 0X07;
		*LcdData = 0x00;
	}
	return 0;
}

/**
 *@brief:	   drv_ILI9325_init
 *@details:    ��ʼ��FSMC�����Ҷ�ȡILI9325���豸ID
 *@param[in]   void  
 *@param[out]  ��
 *@retval:	   
 */
s32 drv_ILI9325_init(DevLcdNode *lcd)
{
	u16 data;
	
	DevLcdBusNode * node;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	bus_lcd_rst(node, 1);
	Delay(50);
	bus_lcd_rst(node, 0);
	Delay(50);
	bus_lcd_rst(node, 1);
	Delay(50);
	/*
		��9325��ID
		
	*/
	*LcdReg = 0x0000;
	*LcdData = 0x0001;
	
	*LcdReg = 0x0000;
    data = *LcdData; 

	ILI9341_DEBUG(LOG_DEBUG, "read reg:%04x\r\n", data);
	if(data != TftLcdILI9325Drv.id)
	{
		ILI9341_DEBUG(LOG_DEBUG, "lcd drive no 9325\r\n");	
		bus_lcd_close(node);
		return -1;
	}

	*LcdReg = 0x00E5;
	*LcdData = 0x78F0;

	*LcdReg = 0x0001;
	*LcdData = 0x0100;

	*LcdReg = 0x0002;
	*LcdData = 0x0700;
	
	*LcdReg = 0x0003;
	*LcdData = 0x1030;
	
	*LcdReg = 0x0004;
	*LcdData = 0x0000;
	
	*LcdReg = 0x0008;
	*LcdData = 0x0202;
	
 	*LcdReg = 0x0009;
	*LcdData = 0x0000;
	
	*LcdReg = 0x000A;
	*LcdData = 0x0000;
	
	*LcdReg = 0x000C;
	*LcdData = 0x0000;

	*LcdReg = 0x000D;
	*LcdData = 0x0000;

	*LcdReg = 0x000F;
	*LcdData = 0x0000;

	//power on sequence VGHVGL
	*LcdReg = 0x0010;
	*LcdData = 0x0000;
	
  	*LcdReg = 0x0011;
	*LcdData = 0x0007;
	
 	*LcdReg = 0x0012;
	*LcdData = 0x0000;
	
 	*LcdReg = 0x0013;
	*LcdData = 0x0000;
	
 	*LcdReg = 0x0007;
	*LcdData = 0x0000;
	
	//vgh 
	*LcdReg = 0x0010;
	*LcdData = 0x1690;

	*LcdReg = 0x0011;
	*LcdData = 0x0227;
	
	//vregiout 
	*LcdReg = 0x0012;
	*LcdData = 0x009D;

	//vom amplitude
	*LcdReg = 0x0013;
	*LcdData = 0x1900;

	//vom H
	*LcdReg = 0x0029;
	*LcdData = 0x0025;

	*LcdReg = 0x002B;
	*LcdData = 0x000D;

	//gamma
	*LcdReg = 0x0030;
	*LcdData = 0x0007;

	*LcdReg = 0x0031;
	*LcdData = 0x0303;
	
	*LcdReg = 0x0032;
	*LcdData = 0x0003;

	*LcdReg = 0x0035;
	*LcdData = 0x0206;
	
	*LcdReg = 0x0036;
	*LcdData = 0x0008;
	
	*LcdReg = 0x0037;
	*LcdData = 0x0406;

	*LcdReg = 0x0038;
	*LcdData = 0x0304;

	*LcdReg = 0x0039;
	*LcdData = 0x0007;
	
	*LcdReg = 0x003C;
	*LcdData = 0x0602;
	
	*LcdReg = 0x003D;
	*LcdData = 0x0008;
	
	/*
	Horizontal and Vertical RAM Address Position 219*319
	����ɨ�贰��

	*/
	*LcdReg = 0x0050;
	*LcdData = 0x0000;
	
	*LcdReg = 0x0051;
	*LcdData = 0x00EF;

	*LcdReg = 0x0052;
	*LcdData = 0x0000;
	
	*LcdReg = 0x0053;
	*LcdData = 0x013F;
	//-------------------------------------
 	*LcdReg = 0x0060;
	*LcdData = 0xA700;
	
	*LcdReg = 0x0061;
	*LcdData = 0x0001;

	*LcdReg = 0x006A;
	*LcdData = 0x0000;
	
	*LcdReg = 0x0080;
	*LcdData = 0x0000;

	*LcdReg = 0x0081;
	*LcdData = 0x0000;

	*LcdReg = 0x0082;
	*LcdData = 0x0000;
	
	*LcdReg = 0x0083;
	*LcdData = 0x0000;

	*LcdReg = 0x0084;
	*LcdData = 0x0000;

	*LcdReg = 0x0085;
	*LcdData = 0x0000;

	*LcdReg = 0x0090;
	*LcdData = 0x0010;

	*LcdReg = 0x0092;
	*LcdData = 0x0600;

	*LcdReg = 0x0007;
	*LcdData = 0x0133;	

	*LcdReg = 0x00;
	*LcdData = 0x0022;

	bus_lcd_close(node);
	return 0;
}
/**
 *@brief:      drv_ILI9325_xy2cp
 *@details:    ��xy����ת��ΪCP����
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9325_xy2cp(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey, u16 *hsa, u16 *hea, u16 *vsa, u16 *vea)
{
	/*
		��ʾXY�᷶Χ
	*/
	if(sx >= lcd->width)
		sx = lcd->width-1;
	
	if(ex >= lcd->width)
		ex = lcd->width-1;
	
	if(sy >= lcd->height)
		sy = lcd->height-1;
	
	if(ey >= lcd->height)
		ey = lcd->height-1;
	/*
		XY�ᣬʵ������������ȡ���ں�����������
		CP�ᣬ�ǿ������Դ棬
		ӳ���ϵȡ����ɨ�跽��
	*/
	/* 
		�������û��ӽǵ�XY���꣬��LCDɨ���CP����Ҫ����һ���Ե�
		���ң�9325�ں�����ҲҪ����ӳ��
		
	*/
	if(lcd->dir == W_LCD)
	{
		*hsa = (lcd->height - ey) - 1;
		*hea = (lcd->height - sy) - 1;
		
		*vsa = sx;
		*vea = ex;
	}
	else
	{
		*hsa = sx;
		*hea = ex;
		*vsa = sy;
		*vea = ey;
	}
	
	return 0;
}
/**
 *@brief:      drv_ILI9325_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ILI9325_drawpoint(DevLcdNode *lcd, u16 x, u16 y, u16 color)
{
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, x, x, y, y, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);
	*LcdData = color; 
	return 0;
}
/**
 *@brief:      dev_ILI9325_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9325_color_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{

	u16 height,width;
	u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width = hea - hsa + 1;//�õ����Ŀ��
	height = vea - vsa + 1;//�߶�
	
	//uart_printf("ili9325 width:%d, height:%d\r\n", width, height);
	//GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	
	DevLcdBusNode * node;
	#define TMP_BUF_SIZE 32
	u16 tmp[TMP_BUF_SIZE];
	u32 cnt;

	for(cnt = 0; cnt < TMP_BUF_SIZE; cnt ++)
	{
		tmp[cnt] = color;
	}
	
	cnt = height*width;
	
	node = bus_lcd_open(lcd->dev.buslcd);

	while(1)
	{
		if(cnt < TMP_BUF_SIZE)
		{
			bus_lcd_write_data(node, (u8 *)tmp, cnt);
			cnt -= cnt;
		}
		else
		{
			bus_lcd_write_data(node, (u8 *)tmp, TMP_BUF_SIZE);
			cnt -= TMP_BUF_SIZE;
		}

		if(cnt <= 0)
			break;
	}
	
	bus_lcd_close(node);

	//GPIO_SetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	return 0;

}

/**
 *@brief:      dev_ILI9325_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ILI9325_fill(DevLcdNode *lcd, u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{

	u16 height,width;
	u16 i,j;
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);

	width=(hea +1) - hsa ;//�õ����Ŀ�� +1����Ϊ�����0��ʼ
	height=(vea +1) - vsa;//�߶�
	
	//uart_printf("ili9325 width:%d, height:%d\r\n", width, height);
	//GPIO_ResetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);

	DevLcdBusNode * node;

	node = bus_lcd_open(lcd->dev.buslcd);
	bus_lcd_write_data(node, (u8 *)color, height*width);	
	bus_lcd_close(node);

	//GPIO_SetBits(GPIOG, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2| GPIO_Pin_3);
	
	return 0;

} 

s32 drv_ILI9325_prepare_display(DevLcdNode *lcd, u16 sx, u16 ex, u16 sy, u16 ey)
{
	u16 hsa,hea,vsa,vea;

	drv_ILI9325_xy2cp(lcd, sx, ex, sy, ey, &hsa,&hea,&vsa,&vea);
	drv_ILI9325_set_cp_addr(lcd, hsa, hea, vsa, vea);	
	return 0;
}
#endif


