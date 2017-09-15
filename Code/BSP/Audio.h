#ifndef __AUDIO_H
#define __AUDIO_H
 

/*------------------------------API---------------------------------------------*/

//初始化声音
void Audio_Init(void);
//
void Period_task_500us(void);
//播放第idx段语音
UINT8 PlayOneSong(UINT8 idx);
//循环播放第idx段语音
void LoopPlayOneSong(UINT8 idx);
//设置音量
void SetAudioVol(UINT8 isize);
//打开功放
void OpenPA(void);
//关闭功放
void ClosePA(void);
//停止当前的播放
void StopPlay(void);

void AudioSta(void);

#endif
