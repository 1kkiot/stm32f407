/**
 * @file            mcu_spi.c
 * @brief           Ƭ��SPI����������
 * @author          test
 * @date            2017��10��26�� ������
 * @version         ����
 * @par             
 * @par History:
 * 1.��    ��:      2017��10��26�� ������
 *   ��    ��:      test
 *   �޸�����:      �����ļ�
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
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "mcu_spi.h"

#define MCU_SPI_DEBUG

#ifdef MCU_SPI_DEBUG
#define SPI_DEBUG	wjq_log 
#else
#define SPI_DEBUG(a, ...)
#endif


#define MCU_SPI_WAIT_TIMEOUT 0x40000
/*
	Ӳ��SPIʹ�ÿ�����SPI3
*/
#define SPI_DEVICE SPI3
/*
	��λ���ã�һ������ģʽ
*/
typedef struct
{
	u16 CPOL;
	u16 CPHA;	
}_strSpiModeSet;

const _strSpiModeSet SpiModeSet[SPI_MODE_MAX]=
	{
		{SPI_CPOL_Low, SPI_CPHA_1Edge},
		{SPI_CPOL_Low, SPI_CPHA_2Edge},
		{SPI_CPOL_High, SPI_CPHA_1Edge},
		{SPI_CPOL_High, SPI_CPHA_2Edge}
	};

/*SPI�����豸����ÿ��Ӳ�����������붨��һ�����˴�����SPI3һ��Ӳ��SPI������*/
s32 DevSpi3Gd = -2;

/**
 *@brief:      mcu_spi_init
 *@details:    ��ʼ��SPI������������ʼ������CS��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStruct;

    //��ʼ��Ƭѡ��ϵͳ��ʱ�趨Ϊ3��SPI��ȫ��ʹ��SPI3
    //DEV_SPI_3_1, ���İ��ϵ�SPI FLASH
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
    //DEV_SPI_3_2, �װ��SPI FLASH
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_15);

	//DEV_SPI_3_3, ���İ�����SPI
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
	GPIO_SetBits(GPIOG,GPIO_Pin_6);
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//---PB3~5
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//---���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//---�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//---100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//---����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//---��ʼ��

    //�������Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3); //PB3 ����Ϊ SPI3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3); //PB4 ����Ϊ SPI3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3); //PB5 ����Ϊ SPI3

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);// ---ʹ�� SPI3 ʱ��
    // ��λSPIģ��
    SPI_I2S_DeInit(SPI_DEVICE);

    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//---˫��˫��ȫ˫��
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//---��ģʽ
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//---8bit֡�ṹ
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//----����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//---���ݲ����ڵ�1��ʱ����
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; //---SPI_NSS_Hard; Ƭѡ��Ӳ������SPI������������
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //---Ԥ��Ƶ
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//---���ݴ���� MSB λ��ʼ
    SPI_InitStruct.SPI_CRCPolynomial = 7;//---CRC ֵ����Ķ���ʽ

    SPI_Init(SPI_DEVICE, &SPI_InitStruct);
    
    //SPI_SSOutputCmd(SPI_DEVICE, DISABLE); 
    DevSpi3Gd = -1;
    return 0;
}

/**
 *@brief:      mcu_spi_open
 *@details:       ��SPI
 *@param[in]   SPI_DEV dev  ��SPI��
               u8 mode      ģʽ
               u16 pre      Ԥ��Ƶ
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_open(SPI_DEV dev, SPI_MODE mode, u16 pre)
{
	SPI_InitTypeDef SPI_InitStruct;

	if(DevSpi3Gd != -1)
		return -1;

	if(mode >= SPI_MODE_MAX)
		return -1;

	SPI_I2S_DeInit(SPI_DEVICE);
	
	SPI_Cmd(SPI1, DISABLE); 
	
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//---˫��˫��ȫ˫��
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//---��ģʽ
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//---8bit֡�ṹ
    SPI_InitStruct.SPI_CPOL = SpiModeSet[mode].CPOL;
    SPI_InitStruct.SPI_CPHA = SpiModeSet[mode].CPHA;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft; //---SPI_NSS_Hard; Ƭѡ��Ӳ������SPI������������
    SPI_InitStruct.SPI_BaudRatePrescaler = pre;  //---Ԥ��Ƶ
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//---���ݴ���� MSB λ��ʼ
    SPI_InitStruct.SPI_CRCPolynomial = 7;//---CRC ֵ����Ķ���ʽ

    SPI_Init(SPI_DEVICE, &SPI_InitStruct);
	
	/*
		Ҫ��ʹ��SPI����ʹ��CS
	*/
	SPI_Cmd(SPI_DEVICE, ENABLE);
	
	if(dev == DEV_SPI_3_1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_14);	
	}
	else if(dev == DEV_SPI_3_2)
	{
		GPIO_ResetBits(GPIOG,GPIO_Pin_15);	
	}
	else if(dev == DEV_SPI_3_3)
	{
		GPIO_ResetBits(GPIOG,GPIO_Pin_6);
	}
	else
	{
		return -1;
	}

	DevSpi3Gd = dev;
		
    
    return 0;
}
/**
 *@brief:      mcu_spi_close
 *@details:    �ر�SPI ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_close(SPI_DEV dev)
{
	if(DevSpi3Gd != dev)
	{
		SPI_DEBUG(LOG_DEBUG, "spi dev err\r\n");
		return -1;
	}
	
	if(dev == DEV_SPI_3_1)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_14);	
	}
	else if(dev == DEV_SPI_3_2)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_15);	
	}
	else if(dev == DEV_SPI_3_3)
	{
		GPIO_SetBits(GPIOG,GPIO_Pin_6);
	}
	else
	{
		return -1;
	}
	
    SPI_Cmd(SPI_DEVICE, DISABLE);
	DevSpi3Gd = -1;
    return 0;
}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI ����
 *@param[in]   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_transfer(SPI_DEV dev, u8 *snd, u8 *rsv, s32 len)
{
    s32 i = 0;
    s32 pos = 0;
    u32 time_out = 0;
    u16 ch;

	if(dev != DevSpi3Gd)
	{
		SPI_DEBUG(LOG_DEBUG, "spi dev err\r\n");
		return -1;
	}
	
    if( ((snd == NULL) && (rsv == NULL)) || (len < 0) )
    {
        return -1;
    }
    
    /* æ�ȴ� */
    time_out = 0;
    while(SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_BSY) == SET)
    {
        if(time_out++ > MCU_SPI_WAIT_TIMEOUT)
        {
            return(-1);
        }
    }

    /* ���SPI�������ݣ���ֹ�����ϴδ������������� */
    time_out = 0;
    while(SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_RXNE) == SET)
    {
        SPI_I2S_ReceiveData(SPI_DEVICE);
        if(time_out++ > 2)
        {
            return(-1);
        }
    }

    /* ��ʼ���� */
    for(i=0; i < len; )
    {
        // д����
        if(snd == NULL)/*����ָ��ΪNULL��˵�������Ƕ����� */
        {
            SPI_I2S_SendData(SPI_DEVICE, 0xff);
        }
        else
        {
            ch = (u16)snd[i];
            SPI_I2S_SendData(SPI_DEVICE, ch);
        }
        i++;
        
        // �ȴ����ս���
        time_out = 0;
        while(SPI_I2S_GetFlagStatus(SPI_DEVICE, SPI_I2S_FLAG_RXNE) == RESET)
        {
            time_out++;
            if(time_out > MCU_SPI_WAIT_TIMEOUT)
            {
                return -1;
            }    
        }
        // ������
        if(rsv == NULL)/* ����ָ��Ϊ�գ������ݺ��� */
        {
            SPI_I2S_ReceiveData(SPI_DEVICE);
        }
        else
        {
            ch = SPI_I2S_ReceiveData(SPI_DEVICE);
            rsv[pos] = (u8)ch;
        } 
        pos++;

    }

    return i;
}
/**
 *@brief:      mcu_spi_cs
 *@details:    �ٿض�ӦSPI��CS
 *@param[in]   SPI_DEV dev  
               u8 sta       
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_hspi_cs(SPI_DEV dev, u8 sta)
{
	if(DevSpi3Gd != dev)
	{
		SPI_DEBUG(LOG_DEBUG, "spi dev err\r\n");
		return -1;
	}

	if(sta == 1)
	{
		switch(dev)
		{
			case DEV_SPI_3_1:
				GPIO_SetBits(GPIOB,GPIO_Pin_14);
				break;
			case DEV_SPI_3_2:
				GPIO_SetBits(GPIOG,GPIO_Pin_15);
				break;
			case DEV_SPI_3_3:
				GPIO_SetBits(GPIOG,GPIO_Pin_6);
				break;
			default:
				break;
		}
	}
	else
	{
		switch(dev)
		{
			case DEV_SPI_3_1:
				GPIO_ResetBits(GPIOB,GPIO_Pin_14);
				break;
			case DEV_SPI_3_2:
				GPIO_ResetBits(GPIOG,GPIO_Pin_15);
				break;
			case DEV_SPI_3_3:
				GPIO_ResetBits(GPIOG,GPIO_Pin_6);
				break;
			default:
				break;
		}
	}
	
	return 0;
}


/*
	VSPI1��ʹ�ô��������߽ӿ�ģ��SPI������XPT2046�������������ɶ���д��
*/

/*

	������SPI��Ӳ�������������
	��ע�⣬���Ƕ�SPI�ӿڵĳ���
	*
	ֱ�Ӵ���IO�ڻ��Ǵ��뺯����
	���뺯����1 ��ͬ��IC�������ܲ�һ����
			2 ͬһ��IC����ͬ��IO��˵����IO�ڵ�����Ҳ��һ����
	����IO�����ã��򵥣����׸Ķ�
	*
*/
typedef struct
{
	char *name;
	SPI_DEV dev;
	s32 gd;
	
	u32 clkrcc;
	GPIO_TypeDef *clkport;
	u16 clkpin;

	u32 mosircc;
	GPIO_TypeDef *mosiport;
	u16 mosipin;

	u32 misorcc;
	GPIO_TypeDef *misoport;
	u16 misopin;

	u32 csrcc;
	GPIO_TypeDef *csport;
	u16 cspin;
}DevVspiIO;

/*

	�����ӿ�ģ��SPI���ɽ�OLED LCD��COG LCD

*/
#define VSPI1_CS_PORT GPIOB
#define VSPI1_CS_PIN GPIO_Pin_1
	
#define VSPI1_CLK_PORT GPIOB
#define VSPI1_CLK_PIN GPIO_Pin_0
	
#define VSPI1_MOSI_PORT GPIOD
#define VSPI1_MOSI_PIN GPIO_Pin_11
	
#define VSPI1_MISO_PORT GPIOD
#define VSPI1_MISO_PIN GPIO_Pin_12
	
#define VSPI1_RCC RCC_AHB1Periph_GPIOB
#define VSPI1_RCC2 RCC_AHB1Periph_GPIOD	

DevVspiIO DevVspi1IO={
		"VSPI1",
		DEV_VSPI_1,
		-2,//δ��ʼ��
		
		VSPI1_RCC,
		VSPI1_CLK_PORT,
		VSPI1_CLK_PIN,
		
		VSPI1_RCC2,
		VSPI1_MOSI_PORT,
		VSPI1_MOSI_PIN,

		VSPI1_RCC2,
		VSPI1_MISO_PORT,
		VSPI1_MISO_PIN,

		VSPI1_RCC,
		VSPI1_CS_PORT,
		VSPI1_CS_PIN,
	};
		
#define VSPI2_CS_PORT GPIOF
#define VSPI2_CS_PIN GPIO_Pin_12
		
#define VSPI2_CLK_PORT GPIOF
#define VSPI2_CLK_PIN GPIO_Pin_11
		
#define VSPI2_MOSI_PORT GPIOF
#define VSPI2_MOSI_PIN GPIO_Pin_10
		
#define VSPI2_MISO_PORT GPIOF
#define VSPI2_MISO_PIN GPIO_Pin_9
		
#define VSPI2_RCC RCC_AHB1Periph_GPIOF
		
DevVspiIO DevVspi2IO={
		"VSPI2",
		DEV_VSPI_2,
		-2,//δ��ʼ��
		
		VSPI2_RCC,
		VSPI2_CLK_PORT,
		VSPI2_CLK_PIN,
		
		VSPI2_RCC,
		VSPI2_MOSI_PORT,
		VSPI2_MOSI_PIN,

		VSPI2_RCC,
		VSPI2_MISO_PORT,
		VSPI2_MISO_PIN,

		VSPI2_RCC,
		VSPI2_CS_PORT,
		VSPI2_CS_PIN,
	};

/*���õ�����SPI�豸��ռλ��*/		
DevVspiIO DevVspiNULL={
		"VSPI0",
		DEV_VSPI_0,
		-2,//δ��ʼ��;
		};

DevVspiIO *DevVspiIOList[]={
	&DevVspiNULL,
	
	#ifdef SYS_USE_VSPI1
	&DevVspi1IO,
	#endif

	#ifdef SYS_USE_VSPI2
	&DevVspi2IO,
	#endif
	};

/**
 *@brief:      mcu_vspi_init
 *@details:       ��ʼ������SPI
 *@param[in]  void  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	u8 i;
	/* ���ݴ����dev�ҵ�IO������*/
	DevVspiIO *vspi;

	i = 1;
	while(1)
	{
		if(i >= sizeof(DevVspiIOList)/sizeof(DevVspiIO *))
			break;
		
		vspi = DevVspiIOList[i];
		
		wjq_log(LOG_INFO, "bus_vspi_init %s\r\n", vspi->name);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		RCC_AHB1PeriphClockCmd(vspi->csrcc, ENABLE);
		GPIO_InitStructure.GPIO_Pin = vspi->cspin;
		GPIO_Init(vspi->csport, &GPIO_InitStructure);
		GPIO_SetBits(vspi->csport, vspi->cspin);

		RCC_AHB1PeriphClockCmd(vspi->clkrcc, ENABLE);
		GPIO_InitStructure.GPIO_Pin = vspi->clkpin;	
		GPIO_Init(vspi->clkport, &GPIO_InitStructure);
		GPIO_SetBits(vspi->clkport,vspi->clkpin);

		RCC_AHB1PeriphClockCmd(vspi->mosircc, ENABLE);
		GPIO_InitStructure.GPIO_Pin = vspi->mosipin;
		GPIO_Init(vspi->mosiport, &GPIO_InitStructure);
		GPIO_SetBits(vspi->mosiport, vspi->mosipin);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		RCC_AHB1PeriphClockCmd(vspi->misorcc, ENABLE);
		GPIO_InitStructure.GPIO_Pin = vspi->misopin;
		GPIO_Init(vspi->misoport, &GPIO_InitStructure);
		GPIO_SetBits(vspi->misoport, vspi->misopin);

		vspi->gd = -1;

		i++;
			
	}
	wjq_log(LOG_INFO, "vspi init finish!\r\n");
	
	return 0;
}


static DevVspiIO *mcu_vspi_find_io(SPI_DEV dev)
{
	u8 i;
	/* ���ݴ����dev�ҵ�IO������*/
	DevVspiIO *vspi;

	i = 0;
	while(1)
	{
		if(i >= sizeof(DevVspiIOList)/sizeof(DevVspiIO *))
			break;
		
		vspi = DevVspiIOList[i];
		if(vspi->dev == dev)
		{
			//uart_printf("vspi find io\r\n");
			return vspi;
		}
		i++;
	}
	
	return NULL;
	
}
/**
 *@brief:      vspi_delay
 *@details:    ����SPIʱ����ʱ
 *@param[in]   u32 delay  
 *@param[out]  ��
 *@retval:     
 */
void vspi_delay(u32 delay)
{
	volatile u32 i=delay;

	while(i>0)
	{
		i--;	
	}

}
/**
 *@brief:      mcu_vspi_open
 *@details:    ������SPI
 *@param[in]   SPI_DEV dev    
               SPI_MODE mode  
               u16 pre        
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_open(SPI_DEV dev, SPI_MODE mode, u16 pre)
{
	DevVspiIO *vspi;

	vspi = mcu_vspi_find_io(dev);

	if(vspi == NULL)
		return -1;
	
	if(vspi->gd != -1)
		return -1;

	GPIO_ResetBits(vspi->csport, vspi->cspin);	

	vspi->gd = dev;
		
    return 0;
}
/**
 *@brief:      mcu_vspi_close
 *@details:       �ر�����SPI
 *@param[in]  SPI_DEV dev  
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_close(SPI_DEV dev)
{
	DevVspiIO *vspi;

	vspi = mcu_vspi_find_io(dev);

	if(vspi == NULL)
		return -1;
	
	if(vspi->gd != dev)
	{
		SPI_DEBUG(LOG_DEBUG, "vspi dev err\r\n");
		return -1;
	}
	
	GPIO_SetBits(vspi->csport, vspi->cspin);	

	vspi->gd = -1;
    return 0;
}
/**
 *@brief:      mcu_vspi_transfer
 *@details:       ����SPIͨ��
 *@param[in]   SPI_DEV dev  
               u8 *snd      
               u8 *rsv      
               s32 len      
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_transfer(SPI_DEV dev, u8 *snd, u8 *rsv, s32 len)
{
	u8 i;
	u8 data;
	s32 slen;
	u8 misosta;
	DevVspiIO *vspi;

	vspi = mcu_vspi_find_io(dev);

	if(vspi == NULL)
		return -1;

	if(dev != vspi->gd)
	{
		SPI_DEBUG(LOG_DEBUG, "vspi dev err\r\n");
		return -1;
	}
	
    if( ((snd == NULL) && (rsv == NULL)) || (len < 0) )
    {
        return -1;
    }

	slen = 0;

	while(1)
	{
		if(slen >= len)
			break;

		if(snd == NULL)
			data = 0xff;
		else
			data = *(snd+slen);
		
		for(i=0; i<8; i++)
		{
			GPIO_ResetBits(vspi->clkport, vspi->clkpin);
			vspi_delay(10);
			
			if(data&0x80)
				GPIO_SetBits(vspi->mosiport, vspi->mosipin);
			else
				GPIO_ResetBits(vspi->mosiport, vspi->mosipin);
			
			vspi_delay(10);
			data<<=1;
			GPIO_SetBits(vspi->clkport, vspi->clkpin);
			
			misosta = GPIO_ReadInputDataBit(vspi->misoport, vspi->misopin);
			if(misosta == Bit_SET)
			{
				data |=0x01;
			}
			else
			{
				data &=0xfe;
			}
			vspi_delay(10);
			
		}

		if(rsv != NULL)
			*(rsv+slen) = data;
		
		slen++;
	}

	return slen;
}
/**
 *@brief:      mcu_vspi_cs
 *@details:    ��������SPI��ʹ�ܽ�
 *@param[in]   SPI_DEV dev  
               u8 sta       
 *@param[out]  ��
 *@retval:     
 */
static s32 mcu_vspi_cs(SPI_DEV dev, u8 sta)
{
	DevVspiIO *vspi;

	vspi = mcu_vspi_find_io(dev);

	if(vspi == NULL)
		return -1;

	if(dev != vspi->gd)
	{
		SPI_DEBUG(LOG_DEBUG, "vspi dev err\r\n");
		return -1;
	}

	if(sta == 1)
	{
		GPIO_SetBits(vspi->csport, vspi->cspin);
	}
	else
	{
		GPIO_ResetBits(vspi->csport, vspi->cspin);
	}
	
	return 0;
}

/*


	����SPIͳһ����ӿ�


*/
s32 mcu_spi_init(void)
{
	mcu_hspi_init();
	mcu_vspi_init();
	return 0;
}

/**
 *@brief:      mcu_spi_open
 *@details:       ��SPI
 *@param[in]   SPI_DEV dev  ��SPI��
               u8 mode      ģʽ
               u16 pre      Ԥ��Ƶ
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_open(SPI_DEV dev, SPI_MODE mode, u16 pre)
{
	switch(dev)
	{
		case DEV_SPI_3_1:
		case DEV_SPI_3_2:
		case DEV_SPI_3_3:
			return mcu_hspi_open(dev, mode, pre);

		case DEV_VSPI_1:
		case DEV_VSPI_2:
			return mcu_vspi_open(dev, mode, pre);
		
		default:
			return -1;
		
	}

}
/**
 *@brief:      mcu_spi_close
 *@details:    �ر�SPI ������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_close(SPI_DEV dev)
{
	switch(dev)
	{
		case DEV_SPI_3_1:
		case DEV_SPI_3_2:
		case DEV_SPI_3_3:
			return mcu_hspi_close(dev);

		case DEV_VSPI_1:
		case DEV_VSPI_2:
			return mcu_vspi_close(dev);

		default:
			return -1;
	}

}
/**
 *@brief:      mcu_spi_transfer
 *@details:    SPI ����
 *@param[in]   u8 *snd  
               u8 *rsv  
               s32 len  
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_transfer(SPI_DEV dev, u8 *snd, u8 *rsv, s32 len)
{
	switch(dev)
	{
		case DEV_SPI_3_1:
		case DEV_SPI_3_2:
		case DEV_SPI_3_3:
			return mcu_hspi_transfer(dev, snd, rsv, len);
		
		case DEV_VSPI_1:
		case DEV_VSPI_2:
			return mcu_vspi_transfer(dev, snd, rsv, len);

		default:
			return -1;
	}   
}
/**
 *@brief:      mcu_spi_cs
 *@details:    �ٿض�ӦSPI��CS
 *@param[in]   SPI_DEV dev  
               u8 sta       
 *@param[out]  ��
 *@retval:     
 */
s32 mcu_spi_cs(SPI_DEV dev, u8 sta)
{
	switch(dev)
	{
		case DEV_SPI_3_1:
		case DEV_SPI_3_2:
		case DEV_SPI_3_3:
			return mcu_hspi_cs(dev, sta);

		case DEV_VSPI_1:
		case DEV_VSPI_2:
			return mcu_vspi_cs( dev, sta);
		
		default:
			return -1;

	}
	
}



