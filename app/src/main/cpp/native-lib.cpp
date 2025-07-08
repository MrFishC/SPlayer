#include <jni.h>
#include <string>
#include "FFDemux.h"
#include "XLog.h"
#include "IObserver.h"
#include "IDecode.h"
#include "FFDecode.h"
#include "XEGL.h"
#include "XShader.h"
#include "IVideoView.h"
#include "GLVideoView.h"
#include "FFResample.h"
#include "SLAudioPlay.h"
#include <android/native_window_jni.h>

class TestObs:public IObserver
{
public:
    void update(XData d){
        XLOGI("TestObs updatea size is %d",d.size);//todo 日志未被触发，但是整个流程又没有查出哪里存在问题
    }
};

IVideoView *view = NULL;

//试着将 void改成string，会出现ndk层报错，  可以尝试如何去排查ndk层的问题 总结方法
extern "C" JNIEXPORT void JNICALL
Java_com_jack_splayer_SPlay_InitView(
        JNIEnv* env,
        jobject instance,jobject surface) {
    //从Surface获取原生窗口
    ANativeWindow *win = ANativeWindow_fromSurface(env,surface);
    view->SetRender(win);

    //测试
//    XEGL::Get()->Init(win);//XEGL 不对外开放
//    XShader shader;
//    shader.Init();

//    IPlayerPorxy::Get()->InitView(win);
}

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm,void *res)
{
    FFDecode::InitHard(vm);

//    IPlayerPorxy::Get()->Init(vm);

    /*IPlayerPorxy::Get()->Open("/sdcard/v1080.mp4");
    IPlayerPorxy::Get()->Start();


    IPlayerPorxy::Get()->Open("/sdcard/1080.mp4");
    IPlayerPorxy::Get()->Start();*/

    //测试代码
//    TestObs *tobs = new TestObs();
    IDemux *de = new FFDemux();
//    de->AddObs(tobs);

    de->Open("/sdcard/1080.mp4");

    IDecode *vdecode = new FFDecode();
    vdecode->Open(de->GetVPara(),true);

    IDecode *adecode = new FFDecode();
    adecode->Open(de->GetAPara());

    de->AddObs(vdecode);
    de->AddObs(adecode);

    view = new GLVideoView();
    vdecode->AddObs(view);

    IResample *resample = new FFResample();
    XParameter outPara = de->GetAPara();
    resample->Open(de->GetAPara(),outPara);
    adecode->AddObs(resample);

    IAudioPlay *audioPlay = new SLAudioPlay();
    audioPlay->StartPlay(outPara);
    resample->AddObs(audioPlay);

    de->Start();
    vdecode->Start();
    adecode->Start();

    return JNI_VERSION_1_4;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_jack_splayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    //Android8.0存在重音的问题，有可能是下方的代码执行了两次，更换调用位置
//    //测试代码
////    TestObs *tobs = new TestObs();
//    IDemux *de = new FFDemux();
////    de->AddObs(tobs);
//
//    de->Open("/sdcard/1080.mp4");
//
//    IDecode *vdecode = new FFDecode();
//    vdecode->Open(de->GetVPara(),true);
//
//    IDecode *adecode = new FFDecode();
//    adecode->Open(de->GetAPara());
//
//    de->AddObs(vdecode);
//    de->AddObs(adecode);
//
//    view = new GLVideoView();
//    vdecode->AddObs(view);
//
//    IResample *resample = new FFResample();
//    XParameter outPara = de->GetAPara();
//    resample->Open(de->GetAPara(),outPara);
//    adecode->AddObs(resample);
//
//    IAudioPlay *audioPlay = new SLAudioPlay();
//    audioPlay->StartPlay(outPara);
//    resample->AddObs(audioPlay);
//
//    de->Start();
//    vdecode->Start();
//    adecode->Start();

//    XSleep(3000);
//    de->Stop();

    //测试在线程中进行读取
//    for(;;){
//        XData d = de->Read();
//        XLOGI("Read data size is %d",d.size);
//    }

    return env->NewStringUTF(hello.c_str());
}