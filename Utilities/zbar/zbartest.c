/*
	��ά�����ӿ�

	http://zbar.sourceforge.net/ ������ҳ���ȶ��İ汾��0.10
	���뵽�����ַ�Ĳֿ���
	http://sourceforge.net/projects/zbar
	��github��Ҳ��һ������ֿ�
	https://github.com/ZBar/ZBar Ҳ���߰���û�˸����ˡ�

	ZBar is licensed under the GNU LGPL 2.1 to enable development of both open source and commercial projects.
	ZBarʹ��GNU LGPL 2.1��ȨЭ�飬����ZBarԴ����޸ĺ��Ż������뿪Դ
	
*/
#include "stm32f4xx.h"
#include "wujique_log.h"
#include "zbar.h"
#include "image.h"
/*
	raw��ͼ�����ݣ��Ҷ�����
	width��ͼ����
	height��ͼ��߶�
*/
int Zbar_Test(void* raw, int width, int height)
{
	zbar_image_scanner_t *scanner = NULL;
	
    /* create a reader */
    scanner = zbar_image_scanner_create();
    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int*)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);
	
    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
	
    printf("n = %d\r\n", n);
    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);

    for(; symbol; symbol = zbar_symbol_next(symbol)) 
	{
        /* do something useful with results */
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        const char *data = zbar_symbol_get_data(symbol);
	
        printf("\r\n--------------decoded %s symbol \"%s\"\r\n", zbar_get_symbol_name(typ), data);
        printf("len = %d\r\n",strlen(data));
		
    }

    /* clean up */
    zbar_image_destroy(image);

    zbar_image_scanner_destroy(scanner);

    return(0);
}   



