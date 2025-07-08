//
// Created by jack on 2025-07-08.
//

#include "IAudioPlay.h"

XData IAudioPlay::GetData() {
    XData d;
    while(!isExit){
        framesMutex.lock();
        if(!frames.empty()){
            d = frames.front();
            frames.pop_front();
            framesMutex.unlock();
            return d;
        }
        framesMutex.unlock();
        XSleep(1);
    }
    return d;
}

void IAudioPlay::Update(XData data) {
    //压入缓冲逻辑
    if (data.size <= 0 || !data.data) return;

    while (!isExit) {
        framesMutex.lock();
        if (frames.size() > maxFrame) {
            framesMutex.unlock();
            XSleep(1);
//            break; //todo
            continue;
        }
        frames.push_back(data);
        framesMutex.unlock();
        break;
    }
}