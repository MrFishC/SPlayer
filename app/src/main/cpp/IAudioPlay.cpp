//
// Created by jack on 2025-07-08.
//

#include "IAudioPlay.h"

XData IAudioPlay::GetData() {
    XData d;

    isRuning = true;//todo

    while(!isExit){//这个循环没有退出的情况下，下一次是进不来的
        if(IsPause()){
            XSleep(2);
            continue;
        }

        framesMutex.lock();
        if(!frames.empty()){
            //有数据
            d = frames.front();
            frames.pop_front();
            framesMutex.unlock();
            //音视频同步
            pts = d.pts;
            return d;
        }
        framesMutex.unlock();
        XSleep(1);
    }
    isRuning = false;//todo
    //没有数据
    return d;
}

//SLAudioPlay::Close 调用时需要调用 清理方法
void IAudioPlay::Clear()
{
    framesMutex.lock();
    while(!frames.empty())
    {
        frames.front().Drop();
        frames.pop_front();
    }
    framesMutex.unlock();
}

//IAudioPlay::Update(XData data)  这里的data是由 IResample::Update 传递过来的
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