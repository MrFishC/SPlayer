//
// Created by jack on 2025-07-07.
//
#include <android/native_window_jni.h>
#include "XEGL.h"
#include <EGL/egl.h>
#include <mutex>
#include "XLog.h"
class CXEGL:public XEGL{
public:
    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;
//    std::mutex mux;

    virtual void Draw(){
//        mux.lock();
        if(display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE)
        {
//            mux.unlock();
            return;
        }
        //窗口显示    交换缓冲区
        eglSwapBuffers(display,surface);
//        mux.unlock();
    }

    virtual bool Init(void *win)
    {
        //获取原始窗口        从Surface获取原生窗口
        ANativeWindow *nwin = (ANativeWindow*)win;

        //1.初始化EGL
        //获取EGLDisplay对象 显示设备 获取默认显示
        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        //2.初始化Display
        if(EGL_TRUE != eglInitialize(display,0,0))
        {
//            mux.unlock();
            XLOGE("eglInitialize failed!");
            return false;
        }
        XLOGE("eglInitialize success!");

        //3 获取配置并创建surface
        EGLint configSpec [] = {
                EGL_RED_SIZE,8,
                EGL_GREEN_SIZE,8,
                EGL_BLUE_SIZE,8,
                EGL_SURFACE_TYPE,EGL_WINDOW_BIT,
                EGL_NONE
        };
        //输出配置
        EGLConfig config = 0;
        EGLint numConfigs = 0;
        // 选择配置
        if(EGL_TRUE != eglChooseConfig(display,configSpec,&config,1,&numConfigs))
        {
//            mux.unlock();
            XLOGE("eglChooseConfig failed!");
            return false;
        }
        XLOGE("eglChooseConfig success!");
        surface = eglCreateWindowSurface(display,config,nwin,NULL);
        //4.创建并打开EGL上下文
        const EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION ,2, EGL_NONE};
        context = eglCreateContext(display,config,EGL_NO_CONTEXT,ctxAttr);
        if(context == EGL_NO_CONTEXT)
        {
//            mux.unlock();
            XLOGE("eglCreateContext failed!");
            return false;
        }
        XLOGE("eglCreateContext success!");
        //用于将 OpenGL ES 渲染上下文绑定到当前线程的 绘制表面（Surface） 和 读取表面（Read Surface）
        if(EGL_TRUE != eglMakeCurrent(display,surface,surface,context))
        {
//            mux.unlock();
            XLOGE("eglMakeCurrent failed!");
            return false;
        }
        XLOGE("eglMakeCurrent success!");
//        mux.unlock();
        return true;
    }
};

XEGL *XEGL::Get()
{
    static CXEGL egl;
    return &egl;
}