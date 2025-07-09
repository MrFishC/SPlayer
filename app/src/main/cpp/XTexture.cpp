//
// Created by jack on 2025-07-07.
//

#include "XTexture.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"

class CXTexture : public XTexture {
public:
    XShader sh;//由XTexture来调用
    XTextureType type;
    std::mutex mux;

    virtual bool Init(void *win, XTextureType type) {
        mux.lock();
        XEGL::Get()->Close();
        sh.Close();

        this->type = type;
        if (!win) {
            mux.unlock();
            XLOGE("XTexture Init failed win is NULL");
            return false;
        }

//        if (XEGL::Get()->Init(win)) {//todo
        if (!XEGL::Get()->Init(win)) {
            mux.unlock();
            return false;
        }
        sh.Init((XShaderType)type);
        mux.unlock();
        return true;
    }

    virtual void Draw(unsigned char *data[], int width, int height) {
        mux.lock();
        //数据的宽度和高度都是给XShader来进行显示的
        sh.GetTexture(0, width, height, data[0]);
        if (type == XTEXTURE_YUV420P) {
            sh.GetTexture(1, width / 2, height / 2, data[1]);//U
            sh.GetTexture(2, width / 2, height / 2, data[2]);//V
        }else{
            sh.GetTexture(1, width / 2, height / 2, data[1],true);//UV
        }
        sh.Draw();

        XEGL::Get()->Draw();
        mux.unlock();
    }

    virtual void Drop(){
        mux.lock();
        XEGL::Get()->Close();
        sh.Close();
        mux.unlock();
        delete this;//
    }
};

XTexture *XTexture::Create() {
    return new CXTexture();
}