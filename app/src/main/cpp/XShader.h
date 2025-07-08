//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XSHADER_H
#define SPLAYER_XSHADER_H

enum XShaderType
{
    XSHADER_YUV420P = 0,    //软解码和虚拟机
    XSHADER_NV12 = 25,      //手机
    XSHADER_NV21 = 26
};

class XShader {
public:
    virtual bool Init(XShaderType type=XSHADER_YUV420P);

    //获取材质 & 映射到内存(buf中)
    void GetTexture(unsigned int index,int width,int height,unsigned char *buf,bool isa = false);//isa:是否有透明通道
    void Draw();
protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    unsigned int texts[100] = {0};
};


#endif //SPLAYER_XSHADER_H
