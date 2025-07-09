//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_FFRESAMPLE_H
#define SPLAYER_FFRESAMPLE_H

#include "IResample.h"
#include <mutex>

struct SwrContext;

//Open 和 Resample 有可能不在一个线程之中
class FFResample :public IResample{
public:
    virtual bool Open(XParameter in,XParameter out);
    virtual XData Resample(XData indata);
    virtual void Close();
protected:
    SwrContext *actx = 0;
    std::mutex mux;
};

#endif //SPLAYER_FFRESAMPLE_H
