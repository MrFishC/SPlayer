//
// Created by jack on 2025-07-07.
//

#include "IDecode.h"

//消费者
void IDecode::Main() {
    while (!isExit) {
        packsMutex.lock();
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
            while(!isExit) {
                XData frame = RecvFrame();

                if (!frame.data)break;

                XLOGE("RecvFrame %d", frame.size);

                //发送数据给观察者
                this->Notify(frame);
            }
        }

        pack.Drop();
        packsMutex.unlock();
    }
}

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