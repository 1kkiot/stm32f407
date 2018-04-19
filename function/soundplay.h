/**
 * @file            soundplay.h
 * @brief           �������Ŷ��ⶨ��
 * @author          wujique
 * @date            2018��1��7�� ������
 * @version         ����
 * @par             ��Ȩ���� (C), 2013-2023
 * @par History:
 * 1.��    ��:        2018��1��7�� ������
 *   ��    ��:         wujique
 *   �޸�����:   �����ļ�
*/
#ifndef _SOUNDPLAY_H_
#define _SOUNDPLAY_H_

typedef enum {
  SOUND_IDLE =0,
  SOUND_BUSY,
  SOUND_PLAY,
  SOUND_PAUSE
}
SOUND_State;

typedef enum{
	SOUND_DEV_NULL = 0,	
	SOUND_DEV_1CH,
	SOUND_DEV_2CH
}SOUND_DEV_TYPE;

#endif
