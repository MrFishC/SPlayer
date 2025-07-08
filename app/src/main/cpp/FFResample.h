//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_FFRESAMPLE_H
#define SPLAYER_FFRESAMPLE_H

#include "IResample.h"
struct SwrContext;

class FFResample :public IResample{
public:
    virtual bool Open(XParameter in,XParameter out);
    virtual XData Resample(XData indata);
protected:
    SwrContext *actx = 0;
};

#endif //SPLAYER_FFRESAMPLE_H
