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

//在IPlayerBuilder::BuilderPlayer中做了设置  IVideoView是 视频解码 的观察者
//只要 视频编码 那里有动静 就会触发 IVideoView的Update，而在IVideoView的Update中会调用Render
void GLVideoView::Render(XData data) {
    //写错的代码
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