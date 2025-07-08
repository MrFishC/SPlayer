//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_IRESAMPLE_H
#define SPLAYER_IRESAMPLE_H

#include "IObserver.h"
#include "XParameter.h"
class IResample :public IObserver{
public:
    //要知道输入和输出的元素
    //in:从解封装器中获取  out：封装的
//    virtual bool Open(XParameter in,XParameter out) = 0;//todo
    virtual bool Open(XParameter in,XParameter out=XParameter()) = 0;

    virtual XData Resample(XData data) = 0;

    virtual void Update(XData data);

    int outChannels = 2;
    int outFormat = 1;
};


#endif //SPLAYER_IRESAMPLE_H
