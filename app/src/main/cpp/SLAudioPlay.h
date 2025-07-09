//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_SLAUDIOPLAY_H
#define SPLAYER_SLAUDIOPLAY_H

#include "IAudioPlay.h"
class SLAudioPlay :public IAudioPlay{
public:
    virtual bool StartPlay(XParameter out);
    void Close();
    void PlayCall(void *bufq);

    SLAudioPlay();
    virtual ~SLAudioPlay();
    //获取缓冲数据，如没有则阻塞
//    virtual XData GetData();
protected:
    unsigned char *buf = 0;
    std::mutex mux;
};


#endif //SPLAYER_SLAUDIOPLAY_H
