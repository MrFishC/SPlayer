//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_GLVIDEOVIEW_H
#define SPLAYER_GLVIDEOVIEW_H


#include "XData.h"
#include "XTexture.h"
#include "IVideoView.h"

class GLVideoView : public IVideoView {
public:
    virtual void SetRender(void *win);
    virtual void Render(XData data);

protected:
    void *view = 0;
    XTexture *txt = 0;
};


#endif //SPLAYER_GLVIDEOVIEW_H
