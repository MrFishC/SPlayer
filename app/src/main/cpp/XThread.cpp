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

void XThread::Start() {
    isExit = false;
    thread th(&XThread::ThreadMain, this);
    th.detach();//当前线程放弃对新线程的控制
    //不放弃的话：可能会造成 当对象被清空的时候，相关资源可能造成线程出错
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