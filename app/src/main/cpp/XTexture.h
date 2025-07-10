//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XTEXTURE_H
#define SPLAYER_XTEXTURE_H

enum XTextureType
{
    XTEXTURE_YUV420P = 0,  // Y 4  u 1 v 1
    XTEXTURE_NV12 = 25,    // Y 4   uv 1
    XTEXTURE_NV21 = 26     // Y 4   vu 1

};

class XTexture {
public:
    static XTexture *Create();
    virtual bool Init(void *win,XTextureType type=XTEXTURE_YUV420P) = 0;
    virtual void Draw(unsigned char*data[],int width,int height) = 0;
    virtual void Drop() = 0;
    virtual ~XTexture(){};//这里的析构函数一定要加上 virtual
protected:
//    XTexture();//会存在报错  所有调用 XTexture 构造函数的地方（如派生类构造、new CXTexture）都会引用一个不存在的符号
//    XTexture(){};//可以注释掉    都是细节
};

#endif //SPLAYER_XTEXTURE_H
