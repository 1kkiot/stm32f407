/**
 * @file            dev_cog12864.c
 * @brief           COG LCD����
 * @author          wujique
 * @date            2018��1��10�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��1��10�� ������
 *   ��    ��:         �ݼ�ȸ������
 *   �޸�����:   �����ļ�
		��Ȩ˵����
		1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱�������������ұ�����Ȩ˵����
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
#include "mcu_spi.h"
#include "dev_lcd.h"
#include "dev_str7565.h"

/*
	һ��LCD�ӿ�
	����ͨ�ŵĽӿ�
	��������������ͨ�Žӿڵ��ź�
	���ж��η�װ
*/

/*
	LCD1�ӿڣ�ʹ��������SPI����

*/
#define SERIALLCD_A0_PORT GPIOG
#define SERIALLCD_A0_PIN GPIO_Pin_4
	
#define SERIALLCD_RST_PORT GPIOG
#define SERIALLCD_RST_PIN GPIO_Pin_7
	
#define SERIALLCD_BL_PORT GPIOG
#define SERIALLCD_BL_PIN GPIO_Pin_9

//��λ
#define SERIALLCD_RST_Clr() GPIO_ResetBits(SERIALLCD_RST_PORT, SERIALLCD_RST_PIN)
#define SERIALLCD_RST_Set() GPIO_SetBits(SERIALLCD_RST_PORT, SERIALLCD_RST_PIN)
//����
#define SERIALLCD_RS_Clr() GPIO_ResetBits(SERIALLCD_A0_PORT, SERIALLCD_A0_PIN)
#define SERIALLCD_RS_Set() GPIO_SetBits(SERIALLCD_A0_PORT, SERIALLCD_A0_PIN)
/**
 *@brief:      bus_lcd_1_init
 *@details:    ��ʼ��LCD SPI ����1
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void bus_seriallcd_IO_init(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	//DC(A0)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = SERIALLCD_A0_PIN;
	GPIO_Init(SERIALLCD_A0_PORT, &GPIO_InitStructure);
	GPIO_SetBits(SERIALLCD_A0_PORT,SERIALLCD_A0_PIN);

	//RST
	GPIO_InitStructure.GPIO_Pin = SERIALLCD_RST_PIN; //OUT�������   RST
	GPIO_Init(SERIALLCD_RST_PORT, &GPIO_InitStructure);
	GPIO_SetBits(SERIALLCD_RST_PORT,SERIALLCD_RST_PIN);

	//bl
	GPIO_InitStructure.GPIO_Pin = SERIALLCD_BL_PIN; //OUT������� 
	GPIO_Init(SERIALLCD_BL_PORT, &GPIO_InitStructure);
	GPIO_SetBits(SERIALLCD_BL_PORT, SERIALLCD_BL_PIN);	

}
/**
 *@brief:	   bus_lcd_bl
 *@details:    �������
 *@param[in]   LcdBusType bus  
			   u8 sta		   
 *@param[out]  ��
 *@retval:	   
 */
s32 bus_seriallcd_bl(u8 sta)
{
	if(sta ==1)
	{
		GPIO_SetBits(SERIALLCD_BL_PORT, SERIALLCD_BL_PIN);
	}
	else
	{
		GPIO_ResetBits(SERIALLCD_BL_PORT, SERIALLCD_BL_PIN);	
	}
	return 0;
}

/**
 *@brief:      bus_lcd_spi_init
 *@details:    ��ʼ����Ӧ�������ӿ�
 			   ��Ҫ��ʼ��������⣬��λ������
 *@param[in]   LcdBusType bus  
 *@param[out]  ��
 *@retval:     
 */
s32 bus_seriallcd_init()
{
	bus_seriallcd_IO_init();
	Delay(100);
	SERIALLCD_RST_Clr();
	Delay(100);
	SERIALLCD_RST_Set();
	Delay(100);
	return 0;
}
/**
 *@brief:      bus_lcd_spi_open
 *@details:    ��LCD�ӿ�
 *@param[in]   LcdBusType bus  
 *@param[out]  ��
 *@retval:     
 */
s32 bus_seriallcd_open()
{
	s32 res;
	res = mcu_spi_open(DEV_SPI_3_3, SPI_MODE_3, SPI_BaudRatePrescaler_4);
	return res;
}
/**
 *@brief:      bus_lcd_spi_close
 *@details:    �ر�LCD�ӿ�
 *@param[in]   LcdBusType bus  
 *@param[out]  ��
 *@retval:     
 */
s32 bus_seriallcd_close()
{
	s32 res;
	res = mcu_spi_close(DEV_SPI_3_3);
	return res;
}
/**
 *@brief:      bus_lcd_spi_write_data
 *@details:    д����
 *@param[in]   LcdBusType bus  
               u8 data         
 *@param[out]  ��
 *@retval:     
 */
s32 bus_seriallcd_write_data(u8 *data, u16 len)
{
	SERIALLCD_RS_Set();	
	mcu_spi_cs(DEV_SPI_3_3,0);
	mcu_spi_transfer(DEV_SPI_3_3, data, NULL, len);
	mcu_spi_cs(DEV_SPI_3_3,1);
	return 0;
}
/**
 *@brief:      bus_lcd_spi_write_cmd
 *@details:    д����
 *@param[in]   LcdBusType bus  
               u8 cmd          
 *@param[out]  ��
 *@retval:     
 */
s32 bus_seriallcd_write_cmd(u8 cmd)
{
	u8 tmp[2];
	
	SERIALLCD_RS_Clr();
	
	tmp[0] = cmd;
	mcu_spi_cs(DEV_SPI_3_3,0);
	mcu_spi_transfer(DEV_SPI_3_3, &tmp[0], NULL, 1);
	mcu_spi_cs(DEV_SPI_3_3,1);
	return 0;
}

/*

	COG LCD ������

*/
/*-----------------------------


------------------------------*/
/*
	����ʹ�õ����ݽṹ��������
*/
struct _cog_drv_data
{
	u8 gram[8][128];	
};	

struct _cog_drv_data LcdGram;

#define TFT_LCD_DRIVER_COG12864

#ifdef TFT_LCD_DRIVER_COG12864

s32 drv_ST7565_init(void);
static s32 drv_ST7565_drawpoint(u16 x, u16 y, u16 color);
s32 drv_ST7565_color_fill(u16 sx,u16 ex,u16 sy,u16 ey,u16 color);
s32 drv_ST7565_fill(u16 sx,u16 ex,u16 sy,u16 ey,u16 *color);
static s32 drv_ST7565_display_onoff(u8 sta);
s32 drv_ST7565_prepare_display(u16 sx, u16 ex, u16 sy, u16 ey);
static void drv_ST7565_scan_dir(u8 dir);
void drv_ST7565_lcd_bl(u8 sta);

/*

	����һ��TFT LCD��ʹ��ST7565����IC���豸

*/
_lcd_drv CogLcdST7565Drv = {
							.id = 0X7565,

							.init = drv_ST7565_init,
							.draw_point = drv_ST7565_drawpoint,
							.color_fill = drv_ST7565_color_fill,
							.fill = drv_ST7565_fill,
							.onoff = drv_ST7565_display_onoff,
							.prepare_display = drv_ST7565_prepare_display,
							.set_dir = drv_ST7565_scan_dir,
							.backlight = drv_ST7565_lcd_bl
							};

void drv_ST7565_lcd_bl(u8 sta)
{
	bus_seriallcd_bl(sta);
}
	
/**
 *@brief:      drv_ST7565_scan_dir
 *@details:    �����Դ�ɨ�跽�� ������Ϊ�����Ƕ�
 *@param[in]   u8 dir  
 *@param[out]  ��
 *@retval:     static
 */
static void drv_ST7565_scan_dir(u8 dir)
{
	return;
}

/**
 *@brief:      drv_ST7565_set_cp_addr
 *@details:    ���ÿ����������е�ַ��Χ
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     
 */
static s32 drv_ST7565_set_cp_addr(u16 sc, u16 ec, u16 sp, u16 ep)
{
	return 0;
}

/**
 *@brief:      drv_ST7565_refresh_gram
 *@details:       ˢ��ָ��������Ļ��
                  �����Ǻ���ģʽ����
 *@param[in]   u16 sc  
               u16 ec  
               u16 sp  
               u16 ep  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7565_refresh_gram(u16 sc, u16 ec, u16 sp, u16 ep)
{	
	struct _cog_drv_data *gram; 
	u8 i;

	//uart_printf("drv_ST7565_refresh:%d,%d,%d,%d\r\n", sc,ec,sp,ep);
	gram = (struct _cog_drv_data *)&LcdGram;
	
	bus_seriallcd_open();
    for(i=sp/8; i <= ep/8; i++)
    {
        bus_seriallcd_write_cmd (0xb0+i);    //����ҳ��ַ��0~7��
        bus_seriallcd_write_cmd (((sc>>4)&0x0f)+0x10);      //������ʾλ�á��иߵ�ַ
        bus_seriallcd_write_cmd (sc&0x0f);      //������ʾλ�á��е͵�ַ

        bus_seriallcd_write_data(&(gram->gram[i][sc]), ec-sc+1);

	}
	bus_seriallcd_close();
	
	return 0;
}

/**
 *@brief:      drv_ST7565_display_onoff
 *@details:    ��ʾ��ر�
 *@param[in]   u8 sta  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7565_display_onoff(u8 sta)
{

	bus_seriallcd_open();
	if(sta == 1)
	{
		bus_seriallcd_write_cmd(0XCF);  //DISPLAY ON
	}
	else
	{
		bus_seriallcd_write_cmd(0XCE);  //DISPLAY OFF	
	}
	bus_seriallcd_close();
	return 0;
}

/**
 *@brief:      drv_ST7565_init
 *@details:    
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_init(void)
{
	
	bus_seriallcd_init();
	bus_seriallcd_open();
	
	bus_seriallcd_write_cmd(0xe2);//��λ
	Delay(50);
	bus_seriallcd_write_cmd(0x2c);//��ѹ����1
	Delay(50);
	bus_seriallcd_write_cmd(0x2e);//��ѹ����2
	Delay(50);
	bus_seriallcd_write_cmd(0x2f);//��ѹ����3
	Delay(50);
	
	bus_seriallcd_write_cmd(0x24);//�Աȶȴֵ�����Χ0X20��0X27
	bus_seriallcd_write_cmd(0x81);//�Աȶ�΢��
	bus_seriallcd_write_cmd(0x25);//�Աȶ�΢��ֵ 0x00-0x3f
	
	bus_seriallcd_write_cmd(0xa2);// ƫѹ��
	bus_seriallcd_write_cmd(0xc8);//��ɨ�裬���ϵ���
	bus_seriallcd_write_cmd(0xa0);//��ɨ�裬������
	bus_seriallcd_write_cmd(0x40);//��ʼ�У���һ��
	bus_seriallcd_write_cmd(0xaf);//����ʾ

	bus_seriallcd_close();
	
	wjq_log(LOG_INFO, "drv_ST7565_init finish\r\n");

	/*�����Դ棬�����ͷ�*/
	memset((char*)&LcdGram, 0x00, 128*8);//Ҫ��Ϊ��̬�ж��Դ��С
	
	drv_ST7565_refresh_gram(0,127,0,63);

	return 0;
}

/**
 *@brief:      drv_ST7565_xy2cp
 *@details:    ��xy����ת��ΪCP����
 			   ����COG�ڰ�����˵��CP������Ǻ������꣬
 			   ת������CP���껹�Ǻ������꣬
 			   Ҳ����˵������ģʽ������Ҫ��XY����תCP����
 			   ��������Ҫת��
 *@param[in]   ��
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_xy2cp(u16 sx, u16 ex, u16 sy, u16 ey, u16 *sc, u16 *ec, u16 *sp, u16 *ep)
{

	return 0;
}
/**
 *@brief:      drv_ST7565_drawpoint
 *@details:    ����
 *@param[in]   u16 x      
               u16 y      
               u16 color  
 *@param[out]  ��
 *@retval:     static
 */
static s32 drv_ST7565_drawpoint( u16 x, u16 y, u16 color)
{
	u16 xtmp,ytmp;
	u16 page, colum;
	struct _strlcd_obj *lcd = &LCD;
	
	struct _cog_drv_data *gram;

	gram = (struct _cog_drv_data *)&LcdGram;

	if(x > lcd->width)
		return -1;
	if(y > lcd->height)
		return -1;

	if(lcd->dir == W_LCD)
	{
		xtmp = x;
		ytmp = y;
	}
	else//�����������XY����Դ��ӳ��Ҫ�Ե�
	{
		xtmp = y;
		ytmp = x;
	}
	
	page = ytmp/8; //ҳ��ַ
	colum = xtmp;//�е�ַ
	
	if(color == BLACK)
	{
		gram->gram[page][colum] |= (0x01<<(ytmp%8));
	}
	else
	{
		gram->gram[page][colum] &= ~(0x01<<(ytmp%8));
	}

	/*Ч�ʲ���*/
	bus_seriallcd_open();
    bus_seriallcd_write_cmd (0xb0 + page );   
    bus_seriallcd_write_cmd (((colum>>4)&0x0f)+0x10); 
    bus_seriallcd_write_cmd (colum&0x0f);    
    bus_seriallcd_write_data( &(gram->gram[page][colum]), 1);
	bus_seriallcd_close();
	return 0;
}
/**
 *@brief:      drv_ST7565_color_fill
 *@details:    ��һ�������趨Ϊĳ����ɫ
 *@param[in]   u16 sx     
               u16 sy     
               u16 ex     
               u16 ey     
               u16 color  
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_color_fill(u16 sx,u16 ex,u16 sy,u16 ey,u16 color)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 page, colum;
	struct _strlcd_obj *lcd = &LCD;
	
	struct _cog_drv_data *gram;

	//uart_printf("drv_ST7565_fill:%d,%d,%d,%d\r\n", sx,ex,sy,ey);

	gram = (struct _cog_drv_data *)&LcdGram;

	/*��ֹ�������*/
	if(sy >= lcd->height)
	{
		sy = lcd->height-1;
	}
	if(sx >= lcd->width)
	{
		sx = lcd->width-1;
	}
	
	if(ey >= lcd->height)
	{
		ey = lcd->height-1;
	}
	if(ex >= lcd->width)
	{
		ex = lcd->width-1;
	}
	
	for(j=sy;j<=ey;j++)
	{
		//uart_printf("\r\n");
		
		for(i=sx;i<=ex;i++)
		{

			if(lcd->dir == W_LCD)
			{
				xtmp = i;
				ytmp = j;
			}
			else//�����������XY����Դ��ӳ��Ҫ�Ե�
			{
				xtmp = j;
				ytmp = lcd->width-i;
			}

			page = ytmp/8; //ҳ��ַ
			colum = xtmp;//�е�ַ
			
			if(color == BLACK)
			{
				gram->gram[page][colum] |= (0x01<<(ytmp%8));
				//uart_printf("*");
			}
			else
			{
				gram->gram[page][colum] &= ~(0x01<<(ytmp%8));
				//uart_printf("-");
			}
		}
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	if(lcd->dir == W_LCD)
	{
		drv_ST7565_refresh_gram(sx,ex,sy,ey);
	}
	else
	{
		drv_ST7565_refresh_gram(sy, ey, lcd->width-ex-1, lcd->width-sx-1); 	
	}
		
	return 0;
}


/**
 *@brief:      drv_ST7565_color_fill
 *@details:    ����������
 *@param[in]   u16 sx      
               u16 sy      
               u16 ex      
               u16 ey      
               u16 *color  ÿһ�������ɫ����
 *@param[out]  ��
 *@retval:     
 */
s32 drv_ST7565_fill(u16 sx,u16 ex,u16 sy,u16 ey,u16 *color)
{
	u16 i,j;
	u16 xtmp,ytmp;
	u16 xlen,ylen;
	u16 page, colum;
	u32 index;
	
	struct _strlcd_obj *lcd = &LCD;
	struct _cog_drv_data *gram;
	
	gram = (struct _cog_drv_data *)&LcdGram;

	/*xlen��ylen������ȡ���ݵģ�������LCD*/
	xlen = ex-sx+1;//ȫ����
	ylen = ey-sy+1;

	/*��ֹ�������*/
	if(sy >= lcd->height)
	{
		sy = lcd->height-1;
	}
	if(sx >= lcd->width)
	{
		sx = lcd->width-1;
	}
	
	if(ey >= lcd->height)
	{
		ey = lcd->height-1;
	}
	if(ex >= lcd->width)
	{
		ex = lcd->width-1;
	}
	
	for(j=sy;j<=ey;j++)
	{
		//uart_printf("\r\n");
		index = (j-sy)*xlen;
		
		for(i=sx;i<=ex;i++)
		{

			if(lcd->dir == W_LCD)
			{
				xtmp = i;
				ytmp = j;
			}
			else//�����������XY����Դ��ӳ��Ҫ�Ե�
			{
				xtmp = j;
				ytmp = lcd->width-i;
			}

			page = ytmp/8; //ҳ��ַ
			colum = xtmp;//�е�ַ
			
			if(*(color+index+i-sx) == BLACK)
			{
				gram->gram[page][colum] |= (0x01<<(ytmp%8));
				//uart_printf("*");
			}
			else
			{
				gram->gram[page][colum] &= ~(0x01<<(ytmp%8));
				//uart_printf("-");
			}
		}
	}

	/*
		ֻˢ����Ҫˢ�µ�����
		���귶Χ�Ǻ���ģʽ
	*/
	if(lcd->dir == W_LCD)
	{
		drv_ST7565_refresh_gram(sx,ex,sy,ey);
	}
	else
	{

		drv_ST7565_refresh_gram(sy, ey, lcd->width-ex-1, lcd->width-sx-1); 	
	}
	//uart_printf("refresh ok\r\n");		
	return 0;
}

s32 drv_ST7565_prepare_display(u16 sx, u16 ex, u16 sy, u16 ey)
{
	return 0;
}
#endif


/*

	OLED �� COG LCD ��������
	������ʼ����һ��
	OLED������������SSD1315��SSD1615��һ���ġ�
*/

/**
 *@brief:	   drv_ssd1615_init
 *@details:    
 *@param[in]   void  
 *@param[out]  ��
 *@retval:	   
 */
s32 drv_ssd1615_init(void)
{
	bus_seriallcd_init();

	bus_seriallcd_open();

	bus_seriallcd_write_cmd(0xAE);//--turn off oled panel
	bus_seriallcd_write_cmd(0x00);//---set low column address
	bus_seriallcd_write_cmd(0x10);//---set high column address
	bus_seriallcd_write_cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	bus_seriallcd_write_cmd(0x81);//--set contrast control register
	bus_seriallcd_write_cmd(0xCF); // Set SEG Output Current Brightness
	bus_seriallcd_write_cmd(0xA1);//--Set SEG/Column Mapping	  0xa0���ҷ��� 0xa1����
	bus_seriallcd_write_cmd(0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
	bus_seriallcd_write_cmd(0xA6);//--set normal display
	bus_seriallcd_write_cmd(0xA8);//--set multiplex ratio(1 to 64)
	bus_seriallcd_write_cmd(0x3f);//--1/64 duty
	bus_seriallcd_write_cmd(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	bus_seriallcd_write_cmd(0x00);//-not offset
	bus_seriallcd_write_cmd(0xd5);//--set display clock divide ratio/oscillator frequency
	bus_seriallcd_write_cmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	bus_seriallcd_write_cmd(0xD9);//--set pre-charge period
	bus_seriallcd_write_cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	bus_seriallcd_write_cmd(0xDA);//--set com pins hardware configuration
	bus_seriallcd_write_cmd(0x12);
	bus_seriallcd_write_cmd(0xDB);//--set vcomh
	bus_seriallcd_write_cmd(0x40);//Set VCOM Deselect Level
	bus_seriallcd_write_cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	bus_seriallcd_write_cmd(0x02);//
	bus_seriallcd_write_cmd(0x8D);//--set Charge Pump enable/disable
	bus_seriallcd_write_cmd(0x14);//--set(0x10) disable
	bus_seriallcd_write_cmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
	bus_seriallcd_write_cmd(0xA6);// Disable Inverse Display On (0xa6/a7) 
	bus_seriallcd_write_cmd(0xAF);//--turn on oled panel

	bus_seriallcd_write_cmd(0xAF);//--turn on oled panel 
	bus_seriallcd_close();
	wjq_log(LOG_INFO, "dev_ssd1615_init finish\r\n");

	memset((char*)&LcdGram, 0x00, 128*8);//Ҫ��Ϊ��̬�ж��Դ��С
	
	drv_ST7565_refresh_gram(0,127,0,63);

	return 0;
}

/**
 *@brief:      drv_ssd1615_display_onoff
 *@details:    SSD1615�򿪻�ر���ʾ
 *@param[in]   DevLcd *lcd  
               u8 sta       
 *@param[out]  ��
 *@retval:     
 */
void drv_ssd1615_display_onoff(u8 sta)
{
	bus_seriallcd_open();
	if(sta == 1)
	{
    	bus_seriallcd_write_cmd(0X8D);  //SET DCDC����
    	bus_seriallcd_write_cmd(0X14);  //DCDC ON
    	bus_seriallcd_write_cmd(0XAF);  //DISPLAY ON
	}
	else
	{
		bus_seriallcd_write_cmd(0X8D);  //SET DCDC����
    	bus_seriallcd_write_cmd(0X10);  //DCDC OFF
    	bus_seriallcd_write_cmd(0XAE);  //DISPLAY OFF	
	}
	bus_seriallcd_close();
}

_lcd_drv OledLcdSSD1615rv = {
							.id = 0X1315,

							.init = drv_ssd1615_init,
							.draw_point = drv_ST7565_drawpoint,
							.color_fill = drv_ST7565_color_fill,
							.fill = drv_ST7565_fill,
							.onoff = drv_ssd1615_display_onoff,
							.prepare_display = drv_ST7565_prepare_display,
							.set_dir = drv_ST7565_scan_dir,
							.backlight = drv_ST7565_lcd_bl
							};


