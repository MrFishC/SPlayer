//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_IDEMUX_H
#define SPLAYER_IDEMUX_H

#include "XData.h"
#include "IObserver.h"
#include "XParameter.h"

//解封装接口
class IDemux:public IObserver{
public:
    //打开文件，或者流媒体 rmtp http rtsp
    virtual bool Open(const char *url) = 0;
    virtual void Close() = 0;
    //获取视频参数
    virtual XParameter GetVPara() = 0;
    //获取音频参数
    virtual XParameter GetAPara() = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData Read() = 0;

    //seek 位置 pos 0.0~1.0
    virtual bool Seek(double pos) = 0;

    //总时长（毫秒）
    int totalMs = 0;

protected:
    virtual void Main();
};


#endif //SPLAYER_IDEMUX_H
