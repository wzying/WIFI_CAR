#ifndef __AUDIO_H
#define __AUDIO_H
 

/*------------------------------API---------------------------------------------*/

//��ʼ������
void Audio_Init(void);
//
void Period_task_500us(void);
//���ŵ�idx������
UINT8 PlayOneSong(UINT8 idx);
//ѭ�����ŵ�idx������
void LoopPlayOneSong(UINT8 idx);
//��������
void SetAudioVol(UINT8 isize);
//�򿪹���
void OpenPA(void);
//�رչ���
void ClosePA(void);
//ֹͣ��ǰ�Ĳ���
void StopPlay(void);

void AudioSta(void);

#endif
