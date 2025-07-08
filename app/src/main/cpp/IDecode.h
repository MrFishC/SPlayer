//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_IDECODE_H
#define SPLAYER_IDECODE_H

#include "XParameter.h"
#include "IObserver.h"
#include <list>

//解码接口，支持硬解码
class IDecode :public IObserver{
public:
    //打开解码器
    virtual bool Open(XParameter para,bool isHard = false) = 0;

    //future模型（跟生产者消费者模型一样） 发送数据到线程解码
    virtual bool SendPacket(XData pkt) = 0;

    //从线程中获取解码结果
    virtual XData RecvFrame() = 0;

    //由主体notify的数据
    virtual void Update(XData pkt);

    bool isAudio = false;

    //最大的队列缓冲   如果是1s25帧，则可以缓冲4s
    int maxList = 100;

    //同步时间，再次打开文件要清理
    int synPts = 0;
    int pts = 0;

protected:
    virtual void Main();//解码工作就在该线程来做
    //读取缓冲
    std::list<XData> packs;
    std::mutex packsMutex;
};

#endif //SPLAYER_IDECODE_H
