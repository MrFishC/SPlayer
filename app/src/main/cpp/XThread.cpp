//
// Created by jack on 2025-07-07.
//

#include "XThread.h"
#include <thread>

using namespace std;

void XSleep(int mis) {
    chrono::milliseconds du(mis);
    this_thread::sleep_for(du);
}

bool XThread::Start() {
    isExit = false;
    isPause = false;
    // 创建新线程，执行ThreadMain方法
    //thread 是 C++11 标准库提供的线程类，用于创建和管理线程
    //参数3，由于 ThreadMain 是一个成员函数，它必须绑定到一个对象才能调用
    thread th(&XThread::ThreadMain, this);
    th.detach();//当前线程放弃对新线程的控制  分离线程(让线程独立运行)
    //不放弃的话：可能会造成 当对象被清空的时候，相关资源可能造成线程出错
    return true;
}

void XThread::SetPause(bool isP)
{
    isPause = isP;
    //等待100毫秒           等待的意义 暂时不太理解
    for(int i = 0; i < 10; i++)
    {
        if(isPausing == isP)
        {
            break;
        }
        XSleep(10);
    }

}

//通过控制isExit安全停止线程（不一定成功）
void XThread::Stop() {
    isExit = true;
    for (int i = 0; i < 200; i++) {
        if (!isRuning){
            XLOGI("Stop 停止线程成功!");
            return;
        }
        XSleep(1);
    }
    XLOGI("Stop 停止线程超时!");
}

void XThread::ThreadMain() {
    isRuning = true;
    XLOGI("线程函数既进入");
    Main();
    XLOGI("线程函数既退出");
    isRuning = false;
}