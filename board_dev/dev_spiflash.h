#ifndef __DEV_SPIFLASH_H_
#define __DEV_SPIFLASH_H_

/*SPI FLASH ��Ϣ*/
typedef struct
{
	char *name;
	u32 JID;
	u32 MID;
	/*���������������С����Ϣ*/
	u32 sectornum;//�ܿ���
	u32 sectorsize;//���С
	u32 structure;//������
	
}_strSpiFlash;


/*SPI FLASH�豸����*/
typedef struct
{
	char *name;//�豸����
	SPI_DEV spi;//����������SPI����
	_strSpiFlash *pra;//�豸��Ϣ
}DevSpiFlash;


extern s32 dev_spiflash_sector_erase(DevSpiFlash *dev, u32 sector_addr);
extern s32 dev_spiflash_sector_read(DevSpiFlash *dev, u32 sector, u8 *dst);	
extern s32 dev_spiflash_sector_write(DevSpiFlash *dev, u32 sector, u8 *src);
extern s32 dev_spiflash_init(void);
extern s32 dev_spiflash_open(DevSpiFlash *dev, char* name);
extern s32 dev_spiflash_test(void);

#endif

