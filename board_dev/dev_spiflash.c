/**
 * @file            dev_spiflash.c
 * @brief           spi flash ��������
 * @author          test
 * @date            2017��10��26�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:      2017��10��26�� ������
 *   ��    ��:      test
 *   �޸�����:      �����ļ�
  ��Ȩ˵����
		1 Դ����ݼ�ȸ���������С�
		2 �������ڵ�������ҵ��;�����׿��������۳��⣩��������Ȩ��
		3 �ݼ�ȸ�����Ҳ��Դ��빦�����κα�֤����ʹ�������в��ԣ�����Ը���
		4 �������޸�Դ�벢�ַ���������ֱ�����۱���������������뱣��WUJIQUE��Ȩ˵����
		5 �緢��BUG�����Ż�����ӭ�������¡�����ϵ��code@wujique.com
		6 ʹ�ñ�Դ�����൱����ͬ����Ȩ˵����
		7 ���ַ����Ȩ��������ϵ��code@wujique.com
		8 һ�н���Ȩ���ݼ�ȸ���������С�
*/
#include "string.h"

#include "stm32f4xx.h"
#include "wujique_log.h"
#include "mcu_spi.h"
#include "dev_spiflash.h"

#define DEV_SPIFLASH_DEBUG

#ifdef DEV_SPIFLASH_DEBUG
#define SPIFLASH_DEBUG	wjq_log 
#else
#define SPIFLASH_DEBUG(a, ...)
#endif

/*
	SPI FLASH������Ŀ�ʼ�Ǵ�CS�½��ؿ�ʼ������ÿ�β�������Ҫ����CS����
*/

	
/*
	���õ�SPI FLASH ������Ϣ
*/
_strSpiFlash SpiFlashPraList[]=
{
	{"MX25L3206E", 0XC22016, 0XC215, 1024, 4096, 4194304},
	{"W25Q64JVSI", 0Xef4017, 0Xef16, 2048, 4096, 8388608}
};
	
/*
	�豸������
*/
#define DEV_SPI_FLASH_C 2//�ܹ�����ƬSPI FLASH

DevSpiFlash DevSpiFlashList[DEV_SPI_FLASH_C]=
{
	/*��һ������board_spiflash��SPI FLASH����DEV_SPI_3_2�ϣ��ͺ�δ֪*/
	{"board_spiflash", DEV_SPI_3_2, NULL},
	/*��һ������board_spiflash��SPI FLASH����DEV_SPI_3_1�ϣ��ͺ�δ֪*/
	{"core_spiflash",  DEV_SPI_3_1, NULL},
};


/* spi flash ����*/
#define SPIFLASH_WRITE      0x02  /* Write to Memory instruction  Page Program */
#define SPIFLASH_WRSR       0x01  /* Write Status Register instruction */
#define SPIFLASH_WREN       0x06  /* Write enable instruction */
#define SPIFLASH_WRDIS      0x04  /* Write disable instruction */
#define SPIFLASH_READ       0x03  /* Read from Memory instruction */
#define SPIFLASH_FREAD      0x0B  /* Fast Read from Memory instruction */
#define SPIFLASH_RDSR       0x05  /* Read Status Register instruction  */
#define SPIFLASH_SE         0x20  /* Sector Erase instruction */
#define SPIFLASH_BE         0xD8  /* Bulk Erase instruction */
#define SPIFLASH_CE         0xC7  /* Chip Erase instruction */
#define SPIFLASH_PD         0xB9  /* Power down enable */
#define SPIFLASH_RPD        0xAB  /* Release from Power down mode */
#define SPIFLASH_RDMID      0x90  /* Read Device identification */
#define SPIFLASH_RDJID      0x9F  /* Read JEDEC identification */
#define SPIFLASH_DUMMY_BYTE 0xA5

/**
 *@brief:      dev_spiflash_writeen
 *@details:    FLASH дʹ��:����FLASH�ڵļĴ�����
               ����д/������ÿ��д/����֮ǰ����Ҫ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 dev_spiflash_writeen(DevSpiFlash *dev)  
{
    s32 len = 1;
    u8 command = SPIFLASH_WREN;
	mcu_spi_cs(dev->spi, 0);
	mcu_spi_transfer(dev->spi, &command, NULL, len); //���ݴ���
	mcu_spi_cs(dev->spi, 1);
    return 0;
}
/**
 *@brief:      dev_spiflash_waitwriteend
 *@details:    ��ѯFLASH״̬���ȴ�д��������
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static s32 dev_spiflash_waitwriteend(DevSpiFlash *dev)
{
    u8 flash_status = 0;
    s32 len = 1;
    u8 command = SPIFLASH_RDSR;
	
	mcu_spi_cs(dev->spi, 0);
    mcu_spi_transfer(dev->spi, &command, NULL, len);
    do
    {
        mcu_spi_transfer(dev->spi, NULL, &flash_status, len);
    }
    while ((flash_status & 0x01) != 0); 
	mcu_spi_cs(dev->spi, 1);
		
		return 0;
}
/**
 *@brief:      dev_spiflash_readmorebyte
 *@details:    �����ⳤ��FLASH����
 *@param[in]   u32 addr  
               u8 *dst  
               u32 len      
 *@param[out]  ��
 *@retval:     
 */
static s32 dev_spiflash_readmorebyte(DevSpiFlash *dev, u32 addr, u8 *dst, u32 rlen)
{
    
    s32 len = 4;
    u8 command[4];

    if(rlen == 0)return 0;

    command[0] = SPIFLASH_READ;
    command[1] = (u8)(addr>>16);
    command[2] = (u8)(addr>>8);
    command[3] = (u8)(addr);
	mcu_spi_cs(dev->spi, 0);
    mcu_spi_transfer(dev->spi, command, NULL, len); 
    mcu_spi_transfer(dev->spi, NULL, dst, rlen);
	mcu_spi_cs(dev->spi, 1);
    return(0);
}
/**
 *@brief:      dev_spiflash_write
 *@details:    д���ݵ�FLASH
 *@param[in]   u8* pbuffer     
               u32 addr  
               u16 wlen        
 *@param[out]  ��
 *@retval:     
 */

static s32 dev_spiflash_write(DevSpiFlash *dev, u8* pbuffer, u32 addr, u16 wlen)
{
    s32 len;
    u8 command[4];

    while (wlen)
    {    
		dev_spiflash_writeen(dev);
	
        command[0] = SPIFLASH_WRITE;
        command[1] = (u8)(addr>>16);
        command[2] = (u8)(addr>>8);
        command[3] = (u8)(addr);

        len = 4;
		mcu_spi_cs(dev->spi, 0);
        mcu_spi_transfer(dev->spi, command, NULL, len);
		
        len = 256 - (addr & 0xff);
        if(len < wlen)
        {
            mcu_spi_transfer(dev->spi, pbuffer, NULL, len);
            wlen -= len;
            pbuffer += len;
            addr += len;
        }
        else
        {
            mcu_spi_transfer(dev->spi, pbuffer, NULL, wlen);
            wlen = 0;
            addr += wlen;
            pbuffer += wlen;
        }
		
		mcu_spi_cs(dev->spi, 1);
		
        dev_spiflash_waitwriteend(dev);       
    }
		
	return 0;
}

/**
 *@brief:      dev_spiflash_sector_erase
 *@details:    ����һ��sector
 *@param[in]   u32 sector_addr  sector�ڵ�ַ
 *@param[out]  ��
 *@retval:     
 */
s32 dev_spiflash_sector_erase(DevSpiFlash *dev, u32 sector)
{
    s32 len = 4;
    u8 command[4];
	u32 addr;

	if(sector >= dev->pra->sectornum)
		return -1;
	
	addr = sector*dev->pra->sectorsize;
	
    command[0] = SPIFLASH_SE;
    command[1] = (u8)(addr>>16);
    command[2] = (u8)(addr>>8);
    command[3] = (u8)(addr);
    
    dev_spiflash_writeen(dev);
	
	mcu_spi_cs(dev->spi, 0);
    mcu_spi_transfer(dev->spi, command, NULL, len);
	mcu_spi_cs(dev->spi, 1);

    dev_spiflash_waitwriteend(dev);   

return 0;	
}

/**
 *@brief:      dev_spiflash_sector_read
 *@details:    ��ȡһ������
 *@param[in]   DevSpiFlash *dev  
               u32 sector  ������
               u8 *dst           
 *@param[out]  ��
 *@retval:     
 */
s32 dev_spiflash_sector_read(DevSpiFlash *dev, u32 sector, u8 *dst)	
{
	if(sector >= dev->pra->sectornum)
		return -1;
	
	return dev_spiflash_readmorebyte(dev, sector*dev->pra->sectorsize, dst, dev->pra->sectorsize);
}
/**
 *@brief:      dev_spiflash_sector_write
 *@details:    дһ������
 *@param[in]   DevSpiFlash *dev  
               u32 sector        
               u8 *src           
 *@param[out]  ��
 *@retval:     
 */
s32 dev_spiflash_sector_write(DevSpiFlash *dev, u32 sector, u8 *src)
{
	u16 sector_size;

	if(sector >= dev->pra->sectornum)
		return -1;
	
	sector_size = dev->pra->sectorsize;
	dev_spiflash_write(dev, src, sector*sector_size, sector_size);
	return 0;
}
/**
 *@brief:      dev_spiflash_readMTD
 *@details:    ��FLASH MID��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static u32 dev_spiflash_readMTD(DevSpiFlash *dev)
{
    u32 MID;
    s32 len = 4;
    u8 command[4];
    u8 data[2];

    command[0] = SPIFLASH_RDMID;
    command[1] = 0;
    command[2] = 0;
    command[3] = 0;
	mcu_spi_cs(dev->spi, 0);
    mcu_spi_transfer(dev->spi, command, NULL, len);
    len = 2;
    mcu_spi_transfer(dev->spi, NULL, data, len);
	mcu_spi_cs(dev->spi, 1);
    MID = data[0];
    MID = (MID<<8) + data[1];
         
    return MID;
}
/**
 *@brief:      dev_spiflash_readJTD
 *@details:    ��FLASH JTD��
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
static u32 dev_spiflash_readJTD(DevSpiFlash *dev)
{
    u32 JID;
    s32 len = 1;
    u8 command = SPIFLASH_RDJID;
    u8 data[3];

	mcu_spi_cs(dev->spi, 0);
    len = 1;
    mcu_spi_transfer(dev->spi, &command, NULL, len);
    len = 3;
    mcu_spi_transfer(dev->spi, NULL, data, len);
	mcu_spi_cs(dev->spi, 1);
	
    JID = data[0];
    JID = (JID<<8) + data[1];
    JID = (JID<<8) + data[2];
    
    return JID;
}

/**
 *@brief:      SpiFlashOpen
 *@details:    ��SPI FLASH
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_spiflash_open(DevSpiFlash *dev, char* name)
{
	s32 res;
	SPI_DEV spidev = DEV_SPI_NULL;
	u8 i=0;

	/*��������name�����豸*/
	while(1)
	{
		if(0 == strcmp(name, DevSpiFlashList[i].name))
		{
			spidev = DevSpiFlashList[i].spi;
			break;
		}
		
		i++;
		if(i>= DEV_SPI_FLASH_C)
		{
			SPIFLASH_DEBUG(LOG_DEBUG, "open spi flash err\r\n");
			res = -1;
			break;	
		}		
	}

	SPIFLASH_DEBUG(LOG_DEBUG, "spi flash type:%s\r\n", DevSpiFlashList[i].pra->name);
	
	if(res != -1)
	{
		/*���ݲ��ҵ�����Ϣ��SPI*/
    	res = mcu_spi_open(spidev, SPI_MODE_3, SPI_BaudRatePrescaler_4); //��spi
		if(res == -1)
		{
			SPIFLASH_DEBUG(LOG_DEBUG, "open spi err\r\n");
		}
	}
	
	if(res!= -1)
	{
		/*SPI �򿪳ɹ����豸����*/
		dev->name = DevSpiFlashList[i].name;
		dev->spi = DevSpiFlashList[i].spi;
		dev->pra = DevSpiFlashList[i].pra;
	}
	else
	{
		/*��ʧ�ܣ��壬��ֹ�Ҹ�*/
		dev->name = NULL;
		dev->spi = DEV_SPI_NULL;
		dev->pra = NULL;	
	}
	return 0;    
}
/**
 *@brief:      dev_spiflash_close
 *@details:       �ر�SPI FLASH�豸
 *@param[in]  DevSpiFlash *dev  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_spiflash_close(DevSpiFlash *dev)
{
	s32 res;
	res = mcu_spi_close(dev->spi); 
	if(res == 0)
	{
		dev->name = NULL;
		dev->pra = NULL;
		dev->spi = DEV_SPI_NULL;
		return 0;
	}
	return -1;
}
/**
 *@brief:      dev_spiflash_init
 *@details:    ��ʼ��spiflash
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
s32 dev_spiflash_init(void)
{
	/*���SPI FALSH�����ҳ�ʼ���豸��Ϣ*/
	u8 i=0;
	u8 index = 0;
	SPI_DEV spidev;
	s32 res;
	u32 JID = 0;
	u32 MID = 0;
	
	while(1)
	{
		spidev = DevSpiFlashList[i].spi;			
		
		res = mcu_spi_open(spidev, SPI_MODE_3, SPI_BaudRatePrescaler_4); //��spi
		if(res == 0)
		{
			JID = dev_spiflash_readJTD(&DevSpiFlashList[i]);
			SPIFLASH_DEBUG(LOG_DEBUG, "%s jid:0x%x\r\n", DevSpiFlashList[i].name, JID);
			
			MID  = dev_spiflash_readMTD(&DevSpiFlashList[i]);
			SPIFLASH_DEBUG(LOG_DEBUG, "%s mid:0x%x\r\n", DevSpiFlashList[i].name, MID);
			
			/*����JID�����豸��Ϣ*/
			for(index = 0; index<(sizeof(SpiFlashPraList)/sizeof(_strSpiFlash));index++)
			{
				if((SpiFlashPraList[index].JID == JID)
					&&(SpiFlashPraList[index].MID == MID))
				{
					DevSpiFlashList[i].pra = &(SpiFlashPraList[index]);
					break;
				}
			}
			
			mcu_spi_close(spidev);
		}

		i++;
		if(i>= DEV_SPI_FLASH_C)
		{
			
			break;	
		}		
	}
	
    return 0;    
}


#include "alloc.h"

/**
 *@brief:      dev_spiflash_test_fun
 *@details:    ����FLASH,����д��������ʱʹ�ã����������ݺ�Ҫ����
 *@param[in]   void  
 *@param[out]  ��
 *@retval:     
 */
void dev_spiflash_test_fun(char *name)
{
    u32 addr;
    u16 tmp;
    u8 i = 1;
    u8 rbuf[4096];
    u8 wbuf[4096];
    u8 err_flag = 0;

	DevSpiFlash dev;
	
	s32 res;
	
    wjq_log(LOG_FUN, ">:-------dev_spiflash_test-------\r\n");
    res = dev_spiflash_open(&dev, name);
	wjq_log(LOG_FUN, ">:-------%s-------\r\n", dev.name);
	if(res == -1)
	{
		wjq_log(LOG_FUN, "open spi flash ERR\r\n");
		while(1);
	}
    i = 0;
    for(tmp = 0; tmp < 4096; tmp++)
    {
        wbuf[tmp] = i;
        i++;
    }
    //sector 1 ���в�����Ȼ��д��У�顣
    wjq_log(LOG_FUN, ">:-------test sector erase-------\r\n", addr);
    
    addr = 0;
    dev_spiflash_sector_erase(&dev, addr);
    wjq_log(LOG_FUN, "erase...");

    dev_spiflash_sector_read(&dev, addr, rbuf);;//��һҳ����
    wjq_log(LOG_FUN, "read...");
    
    for(tmp = 0; tmp < dev.pra->sectorsize; tmp++)
    {
        if(rbuf[tmp] != 0xff)//������ȫ������0xff
        {
            wjq_log(LOG_FUN, "%x=%02X ", tmp, rbuf[tmp]);//�����󲻵���0XFF,����    
            err_flag = 1;
        }
    }

    dev_spiflash_sector_write(&dev, addr, wbuf);
    wjq_log(LOG_FUN, "write...");
    
    dev_spiflash_sector_read(&dev, addr, rbuf);
    wjq_log(LOG_FUN, "read...");
    
    wjq_log(LOG_FUN, "\r\n>:test wr..\r\n");
    
    for(tmp = 0; tmp < dev.pra->sectorsize; tmp++)
    {
        if(rbuf[tmp] != wbuf[tmp])
        {
            wjq_log(LOG_FUN, "%x ", tmp);//�������ĸ�д��ȥ�Ĳ���� 
            err_flag = 1;
        }
    }

    if(err_flag == 1)
        wjq_log(LOG_FUN, "bad sector\r\n");
    else
        wjq_log(LOG_FUN, "OK sector\r\n");

	dev_spiflash_close(&dev);
}
/*
	�����ƬFLASH
*/
void dev_spiflash_test_chipcheck(char *name)
{
    u32 addr;
	u16 sector;
    u16 tmp;
    u8 i = 1;
    u8 *rbuf;
    u8 *wbuf;
    u8 err_flag = 0;

	DevSpiFlash dev;
	
	s32 res;
	
    wjq_log(LOG_FUN, ">:-------dev_spiflash_test-------\r\n");
    res = dev_spiflash_open(&dev, name);
	wjq_log(LOG_FUN, ">:-------%s-------\r\n", dev.name);
	if(res == -1)
	{
		wjq_log(LOG_FUN, "open spi flash ERR\r\n");
		while(1);
	}

	rbuf = (u8*)wjq_malloc(dev.pra->sectorsize);
	wbuf = (u8*)wjq_malloc(dev.pra->sectorsize);

	for(sector = 0; sector < dev.pra->sectornum; sector++)
	{
	    i = sector%0xff;
		
	    for(tmp = 0; tmp < dev.pra->sectorsize; tmp++)
	    {
	        wbuf[tmp] = i;
	        i++;
	    }
	    
	    addr = sector * (dev.pra->sectorsize);
		
		wjq_log(LOG_FUN, ">:sector:%d, addr:0x%08x,", sector, addr);
	    dev_spiflash_sector_erase(&dev, sector);
	    wjq_log(LOG_FUN, "erase...");

	    dev_spiflash_sector_read(&dev, sector, rbuf);;//��һҳ����
	    wjq_log(LOG_FUN, "read...");
	    
	    for(tmp = 0; tmp < dev.pra->sectorsize; tmp++)
	    {
	        if(rbuf[tmp] != 0xff)//������ȫ������0xff
	        {
	            //wjq_log(LOG_FUN, "%x=%02X ", tmp, rbuf[tmp]);//�����󲻵���0XFF,����    
	            err_flag = 1;
	        }
	    }

	    dev_spiflash_sector_write(&dev, sector, wbuf);
	    wjq_log(LOG_FUN, "write...");
	    
	    dev_spiflash_sector_read(&dev, sector, rbuf);
	    wjq_log(LOG_FUN, "read...");
	    
	    for(tmp = 0; tmp < dev.pra->sectorsize; tmp++)
	    {
	        if(rbuf[tmp] != wbuf[tmp])
	        {
	            //wjq_log(LOG_FUN, "%x ", tmp);//�������ĸ�д��ȥ�Ĳ���� 
	            err_flag = 1;
	        }
	    }

	    if(err_flag == 1)
	        wjq_log(LOG_FUN, "bad sector\r\n");
	    else
	        wjq_log(LOG_FUN, "OK sector\r\n");
	}
	dev_spiflash_close(&dev);

	wjq_free(rbuf);
	wjq_free(wbuf);
}

s32 dev_spiflash_test(void)
{
	dev_spiflash_test_fun("board_spiflash");
	dev_spiflash_test_fun("core_spiflash");
	return 0;
}

