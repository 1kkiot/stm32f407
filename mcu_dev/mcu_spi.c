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

//#define MCU_SPI_DEBUG

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
s32 mcu_spi_init(void)
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
s32 mcu_spi_open(SPI_DEV dev, SPI_MODE mode, u16 pre)
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
s32 mcu_spi_close(SPI_DEV dev)
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
s32 mcu_spi_transfer(SPI_DEV dev, u8 *snd, u8 *rsv, s32 len)
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
s32 mcu_spi_cs(SPI_DEV dev, u8 sta)
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

