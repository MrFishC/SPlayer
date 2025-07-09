//
// Created by jack on 2025-07-07.
//

#include "GLVideoView.h"

void GLVideoView::SetRender(void *win) {
    view = win;
}

void GLVideoView::Close() {
    mux.lock();
    if(txt)
    {
        txt->Drop();
        txt = 0;
    }
    mux.unlock();
}

void GLVideoView::Render(XData data) {
    //todo 错误点
//    if(!view){
//        txt = XTexture::Create();
//        txt->Init(view);
//    }

    if (!view)return;
    if (!txt) {
        txt = XTexture::Create();
        txt->Init(view, (XTextureType) data.format);
    }

    txt->Draw(data.datas, data.width, data.height);
}