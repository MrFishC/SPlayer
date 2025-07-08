//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_FFDECODE_H
#define SPLAYER_FFDECODE_H
#include "XParameter.h"
#include "IDecode.h"
struct AVCodecContext;
struct AVFrame;

class FFDecode:public IDecode {
public:
    static void InitHard(void *vm);
    virtual bool Open(XParameter para,bool isHard = false);

    virtual bool SendPacket(XData pkt);
    //从线程中获取解码结果   再次调用会复用上次空间（todo 暂时不理解），线程不安全
    virtual XData RecvFrame();
protected:
    AVCodecContext *codec = 0;
    AVFrame *frame = 0;
//    std::mutex mux;
};


#endif //SPLAYER_FFDECODE_H
