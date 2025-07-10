//
// Created by jack on 2025-07-07.
//

#include "XShader.h"
#include "XLog.h"
#include <GLES2/gl2.h>

//这个宏的作用是将输入的参数x转换为字符串字面量。在预处理阶段，任何传递给GET_STR的参数都会被转换成带双引号的字符串。例如：GET_STR(hello)会被展开为"hello"。
//1.使用了字符串化运算符（#）的功能
#define GET_STR(x) #x
//顶点着色器glsl                       确定位置
//以下场景，顶点着色器 的模板几乎是固定的(暂时不研究细节)
//✅ 在屏幕上渲染一张2D纹理（如图片/视频）。
//✅ 处理坐标系差异（如YUV数据）。
//✅ 简单的全屏绘制。
static const char *vertexShader = GET_STR(
//输入：接收CPU传递的顶点和UV坐标。
//处理：翻转Y轴解决坐标系差异问题。
//输出：传递插值后的UV坐标给片元着色器，确保纹理采样正确。
//attribute 的作用：接收从CPU端（C++代码）传递的逐顶点数据·
//aPosition ：定义顶点在3D空间中的位置（代码中Z值固定为0，实际是2D渲染）。
//vTexCoord 用途：定义每个顶点对应的纹理UV坐标（范围0.0~1.0） // 输入：纹理UV坐标
        attribute vec4 aPosition; //顶点坐标              输入：顶点坐标（3D空间+齐次坐标）
        attribute vec2 aTexCoord; //材质顶点坐标           输出：处理后的纹理坐标（传递给片元着色器）
        varying vec2 vTexCoord;   //输出的材质坐标         输出：处理后的纹理坐标（传递给片元着色器）
        void main() {
            // 翻转Y坐标        翻转的原因：图像方向正确。
            vTexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
            //简单投影：代码中顶点坐标已经是裁剪空间坐标（范围[-1,1]），直接赋值即可全屏渲染。例如：顶点坐标(1.0, -1.0, 0.0)对应屏幕右下角。
            gl_Position = aPosition;
        }
);

//片元着色器,软解码和部分x86硬解码
//这段代码是YUV渲染的黄金标准实现（暂不研究具体细节），直接复用于大多数视频解码场景，但需确保输入数据格式（YUV420P）和纹理绑定方式完全匹配。
//这段片元着色器代码是专门为YUV420P格式视频数据转RGB渲染设计的
static const char *fragYUV420P = GET_STR(
        precision mediump float;    //中等精度浮点数 作用：平衡性能和精度，适合移动端GPU（OpenGL ES要求必须声明精度）。
        varying vec2 vTexCoord;     //顶点着色器传递的坐标  // 插值后的纹理坐标（来自顶点着色器）
        //YUV420P数据存储为三个独立平面（Y全分辨率，U/V半分辨率），需分别采样。
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            //FFmpeg等解码器输出的YUV数据默认以0.5为色度中点，必须匹配此约定。
            yuv.g = texture2D(uTexture, vTexCoord).r - 0.5;//漏掉U/V减0.5	颜色严重偏绿或紫（色度信号未归一化）
            yuv.b = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;//这里如果用错   用错转换矩阵系数	颜色失真（如人脸发蓝）
            //输出像素颜色  输出颜色
            gl_FragColor = vec4(rgb, 1.0);// 不透明RGB
        }
);

//片元着色器,软解码和部分x86硬解码
static const char *fragNV12 = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uvTexture, vTexCoord).r - 0.5;
            yuv.b = texture2D(uvTexture, vTexCoord).a - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);
        }
);

//片元着色器,软解码和部分x86硬解码   fragNV21和fragNV12 的区别就在于 yuv.g 和 yuv.b的赋值
static const char *fragNV21 = GET_STR(
        precision mediump float;    //精度
        varying vec2 vTexCoord;     //顶点着色器传递的坐标
        uniform sampler2D yTexture; //输入的材质（不透明灰度，单像素）
        uniform sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.r = texture2D(yTexture, vTexCoord).r;
            yuv.g = texture2D(uvTexture, vTexCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTexCoord).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                       0.0, -0.39465, 2.03211,
                       1.13983, -0.58060, 0.0) * yuv;
            //输出像素颜色
            gl_FragColor = vec4(rgb, 1.0);
        }
);

static GLuint InitShader(const char *code, GLint type) {
    //创建shader
    GLuint sh = glCreateShader(type);
    if (sh == 0) {
        XLOGE("glCreateShader %d failed!", type);
        return 0;
    }
    //加载shader
    glShaderSource(sh,
                   1,    //shader数量
                   &code, //shader代码
                   0);   //代码长度
    //编译shader
    glCompileShader(sh);

    //获取编译情况
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        XLOGE("glCompileShader failed!");
        return 0;
    }
    XLOGE("glCompileShader success!");
    return sh;
}

void XShader::Close() {
    mux.lock();
    //释放shader
    if (program)
        glDeleteProgram(program);
    if (fsh)
        glDeleteShader(fsh);
    if (vsh)
        glDeleteShader(vsh);

    //释放材质
    for (int i = 0; i < sizeof(texts) / sizeof(unsigned int); i++) {
        if (texts[i]) {
            //glDeleteTextures：是 OpenGL 中的一个函数，用于删除一个或多个纹理对象
            //参数1：要删除的纹理数量。
            //参数2：指向一个包含纹理对象名称（ID）的数组的指针。
            glDeleteTextures(1, &texts[i]);
        }
        texts[i] = 0;
    }

    mux.unlock();
}

bool XShader::Init(XShaderType type) {
    Close();

    //顶点和片元shader初始化
    //顶点shader初始化
    mux.lock();
    vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    if (vsh == 0) {
        mux.unlock();
        XLOGE("InitShader GL_VERTEX_SHADER failed!");
        return false;
    }
    XLOGE("InitShader GL_VERTEX_SHADER success!");

    switch (type) {
        case XSHADER_YUV420P:
            fsh = InitShader(fragYUV420P, GL_FRAGMENT_SHADER);
            break;
        case XSHADER_NV12:
            fsh = InitShader(fragNV12, GL_FRAGMENT_SHADER);
            break;
        case XSHADER_NV21:
            fsh = InitShader(fragNV21, GL_FRAGMENT_SHADER);
            break;
        default:
            mux.unlock();
            XLOGE("SHADER format is error");
            return false;
    }

    if (fsh == 0) {
        mux.unlock();
        XLOGE("InitShader GL_FRAGMENT_SHADER failed!");
        return false;
    }
    XLOGE("InitShader GL_FRAGMENT_SHADER success!");

    //创建渲染程序
    program = glCreateProgram();
    if (program == 0) {
        mux.unlock();
        XLOGE("glCreateProgram failed!");
        return false;
    }
    //渲染程序中加入着色器代码
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    //链接程序
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        mux.unlock();
        XLOGE("glLinkProgram failed!");
        return false;
    }
    glUseProgram(program);// 使用程序
    XLOGE("glLinkProgram success!");

    //加入三维顶点数据 两个三角形组成正方形
    static float vers[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f,
    };
    // 获取并启用顶点属性
    GLuint apos = (GLuint) glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(apos);
    //传递顶点    // 设置顶点指针
    //OpenGL ES 中用于指定顶点属性数据格式和位置
    //参数2：每个顶点的分量数（1-4）
    //参数3：// 数据类型（如GL_FLOAT）
    //参数4：//  相邻顶点间的字节偏移
    glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 12, vers);

    //加入材质坐标数据
    static float txts[] = {
            1.0f, 0.0f, //右下
            0.0f, 0.0f,
            1.0f, 1.0f,
            0.0, 1.0
    };
    GLuint atex = (GLuint) glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(atex);
    glVertexAttribPointer(atex, 2, GL_FLOAT, GL_FALSE, 8, txts);

    glUniform1i(glGetUniformLocation(program, "yTexture"), 0); //对于纹理第1层
    switch (type) {
        case XSHADER_YUV420P:
            glUniform1i(glGetUniformLocation(program, "uTexture"), 1); //对于纹理第2层
            glUniform1i(glGetUniformLocation(program, "vTexture"), 2); //对于纹理第3层
            break;
        case XSHADER_NV21:
        case XSHADER_NV12:
            glUniform1i(glGetUniformLocation(program, "uvTexture"), 1); //对于纹理第2层
            break;
    }

    XLOGE("初始化Shader成功！");
    mux.unlock();
    return true;
}

void XShader::Draw() {
    mux.lock();
    if (!program) {
        mux.unlock();
        return;
    }
    //三维绘制          注意：顶点数据加了static 对这里的意义
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    mux.unlock();
}

void XShader::GetTexture(unsigned int index, int width, int height, unsigned char *buf, bool isa) {

    unsigned int format = GL_LUMINANCE;//默认是灰度图
    if (isa) {
        format = GL_LUMINANCE_ALPHA;
    }
    mux.lock();
    if (texts[index] == 0) {//== 0 材质没有被初始化过
        //创建1个纹理
        //材质初始化     参数1：创建1个
        glGenTextures(1, &texts[index]);

        //设置纹理属性
        glBindTexture(GL_TEXTURE_2D, texts[index]);
        //缩小的过滤器
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //设置纹理的格式和大小
        glTexImage2D(GL_TEXTURE_2D,
                     0,                    //细节基本 0默认
                     format,        //gpu内部格式 亮度，灰度图
                     width, height,              //拉升到全屏
                     0,                   //边框
                     format,                     //数据的像素格式 亮度，灰度图 要与上面一致
                     GL_UNSIGNED_BYTE,      //像素的数据类型
                     NULL                  //纹理的数据
        );
    }

    //激活第1层纹理,绑定到创建的opengl纹理
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texts[index]);
    //替换纹理内容
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, buf);
    mux.unlock();
}