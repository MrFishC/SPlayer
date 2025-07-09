//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_IVIDEOVIEW_H
#define SPLAYER_IVIDEOVIEW_H

#include "XData.h"
#include "IObserver.h"

class IVideoView :public IObserver{
public:
    virtual void SetRender(void *win) = 0;
    virtual void Render(XData data) = 0;//Render显示
    virtual void Update(XData data);//若添加了 = 0，则子类必须要实现
    virtual void Close() = 0;
};


#endif //SPLAYER_IVIDEOVIEW_H
