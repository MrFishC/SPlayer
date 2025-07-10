//
// Created by jack on 2025-07-07.
//

#include "IDecode.h"

//消费者
void IDecode::Main() {
    while (!isExit) {
        if(IsPause()){
            XSleep(2);
            continue;
        }

        packsMutex.lock();

        //音视频同步   在业务逻辑这里添加
        //要知道音频的播放时间   需要将其进行存储

//        if(!isAudio && synPts > 0){
        if (isAudio && synPts > 0) {
            //开始音视频同步的逻辑
            if (synPts < pts) {
                //此时，表示音频播放的比视频慢，则视频需要等一等
                packsMutex.unlock();
                XSleep(1);
                continue;
            }
        }

        if (packs.empty()) {
            packsMutex.unlock();
            //假sleep的意义，一旦packs为空，循环就不断地在运行。如此会把一个核心的CPU全部耗尽，所有涉及到CPU耗尽的地方一定要加sleep
            XSleep(1);
            continue;
        }
        //取出packet      消费者
        XData pack = packs.front();
        packs.pop_front();//从链表中删除

        //发送数据到解码线程  一个数据包 可能包含多个结果
        if (this->SendPacket(pack)) {
            while (!isExit) {
                XData frame = RecvFrame();

                if (!frame.data)break;

                XLOGE("RecvFrame %d", frame.size);

                //在执行了RecvFrame之后对pts（用来表示当前播放的位置）进行赋值
                pts = frame.pts;

                //发送数据给观察者
                this->Notify(frame);
            }
        }

        pack.Drop();
        packsMutex.unlock();
    }
}

//音、视频解码器 观察 解封装，而 IDemux::Main() 中会去调用 Notify
//生产者
void IDecode::Update(XData pkt) {
    if (pkt.isAudio != isAudio) {
        return;
    }

    while (!isExit) {
        packsMutex.lock();
        if (packs.size() < maxList) {
            //生产者
            packs.push_back(pkt);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        XSleep(1);
    }
}

//清理的时候并不一定调用关闭的方法  但是FFDecode关闭的时候一定要调用清理的工作       seek的时候也要调用
void IDecode::Clear() {
    packsMutex.lock();
    while(!packs.empty())
    {
        packs.front().Drop();
        packs.pop_front();
    }
    pts = 0;
    synPts = 0;
    packsMutex.unlock();
}