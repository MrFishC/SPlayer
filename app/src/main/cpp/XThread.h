//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XTHREAD_H
#define SPLAYER_XTHREAD_H

#include "XLog.h"

//sleep 毫秒
void XSleep(int mis);

//多态的本质是函数的多种实现形态
//如果基类与派生类中有同名成员函数，根据类型兼容规则，当使用基类指针或基类引用操作派生类对象时，只能调用基类的同名函数。
//如果想要使用基类指针或基类引用调用派生类中的成员函数，就需要虚函数解决，虚函数是实现多态的基础
//*****若类中声明了虚函数，并且派生类重新定义了虚函数，当使用基类指针或基类引用操作派生类对象调用函数时，系统会自动调用派生类中的虚函数代替基类虚函数*****
class XThread {
public:
    //启动线程
    virtual bool Start();
    //通过控制isExit安全停止线程（不一定成功）
    virtual void Stop();

    virtual void SetPause(bool isP);

    //是否是暂停状态
    virtual bool IsPause()
    {
        isPausing = isPause;
        return isPause;
    }

    //入口主函数
    virtual void Main(){

    };
protected:
    bool isExit = false;
    bool isRuning = false;
    bool isPause = false;
    bool isPausing = false;

private:
    void ThreadMain();
};

#endif //SPLAYER_XTHREAD_H
