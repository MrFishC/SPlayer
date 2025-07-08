//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_IAUDIOPLAY_H
#define SPLAYER_IAUDIOPLAY_H

#include "IObserver.h"
#include "XParameter.h"
#include "list"

class IAudioPlay :public IObserver{
public:
    //缓冲满了，则该函数会处于阻塞的状态
    virtual void Update(XData data);
    virtual bool StartPlay(XParameter out) = 0;

    //获取缓冲数据，如没有则阻塞
    virtual XData GetData();

    //最大缓冲
    int maxFrame = 100;
protected:
    std::list<XData> frames;
    std::mutex framesMutex;
};

#endif //SPLAYER_IAUDIOPLAY_H
