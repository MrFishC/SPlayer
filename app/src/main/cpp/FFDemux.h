//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_FFDEMUX_H
#define SPLAYER_FFDEMUX_H

#include "IDemux.h"
extern "C"{
#include <libavformat/avformat.h>
}
struct AVFormatContext;//使用该策略，可以不用引入头文件

class FFDemux :public IDemux{
public:
    //打开文件，或者流媒体 rmtp http rtsp
    virtual bool Open(const char *url);

    //读取一帧数据，数据由调用者清理
    virtual XData Read();

    FFDemux();
private:
    AVFormatContext *ic = 0;
};


#endif //SPLAYER_FFDEMUX_H
