//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_IPLAYER_H
#define SPLAYER_IPLAYER_H


#include <mutex>
#include "XThread.h"
#include "XParameter.h"
class IDemux;
class IAudioPlay;
class IVideoView;
class IResample;
class IDecode;

class IPlayer :public XThread{
public:
    static IPlayer *Get(unsigned char index = 0);
    virtual bool Open(const char *path);
    virtual bool Start();
    virtual void Close();
    virtual void InitView(void *win);

    //获取当前的播放进度 0.0 ~ 1.0
    virtual double PlayPos();

    virtual bool Seek(double pos);

    virtual void SetPause(bool isP);

    //音频输出参数配置
    XParameter outPara;

    //是否视频硬解码
    bool isHardDecode = true;

    IDemux *demux = 0;
    IDecode *vdecode = 0;
    IDecode *adecode = 0;
    IResample *resample = 0;
    IVideoView *videoView = 0;
    IAudioPlay *audioPlay = 0;

protected:
    //用作音视频同步
    void Main();
    std::mutex mux;//需要做线程互斥控制

    IPlayer(){};
};


#endif //SPLAYER_IPLAYER_H
