//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_FFDEMUX_H
#define SPLAYER_FFDEMUX_H

#include "IDemux.h"
extern "C"{
#include <libavformat/avformat.h>
}
#include <mutex>

struct AVFormatContext;//使用该策略，可以不用引入头文件

/**
 * Open 和 Read 很容易不在一个线程当中
 * Open：是用户的操作
 * Read：是在IDemux当中来做的
 *
 * 这两者需要共同的访问ic
 */
class FFDemux :public IDemux{
public:
    //打开文件，或者流媒体 rmtp http rtsp
    virtual bool Open(const char *url);
    virtual void Close();

    //seek 位置 pos 0.0~1.0
    virtual bool Seek(double pos);

    //读取一帧数据，数据由调用者清理
    virtual XData Read();

    //获取视频参数
    virtual XParameter GetVPara();
    //获取音频参数
    virtual XParameter GetAPara();

    FFDemux();
private:
    AVFormatContext *ic = 0;
    int audioStream = 1;
    int videoStream = 0;
    std::mutex mux;
};


#endif //SPLAYER_FFDEMUX_H
