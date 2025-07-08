//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XTHREAD_H
#define SPLAYER_XTHREAD_H

#include "XLog.h"

//sleep 毫秒
void XSleep(int mis);

class XThread {
public:
    //启动线程
    virtual void Start();
    //通过控制isExit安全停止线程（不一定成功）
    virtual void Stop();
    //入口主函数
    virtual void Main(){

    };
protected:
    bool isExit = false;
    bool isRuning = false;

private:
    void ThreadMain();
};

#endif //SPLAYER_XTHREAD_H
