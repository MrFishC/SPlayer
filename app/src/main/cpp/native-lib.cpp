#include <jni.h>
#include <string>
#include "XLog.h"
//#include "XEGL.h"
//#include "XShader.h"
#include <android/native_window_jni.h>
//#include "IPlayer.h"
#include "IPlayerPorxy.h"

//class TestObs:public IObserver
//{
//public:
//    void update(XData d){
//        XLOGI("TestObs updatea size is %d",d.size);//todo 日志未被触发，但是整个流程又没有查出哪里存在问题
//    }
//};

//IVideoView *view = NULL;

//static IPlayer *player = NULL;

//试着将 void改成string，会出现ndk层报错，  可以尝试如何去排查ndk层的问题 总结方法
extern "C" JNIEXPORT void JNICALL
Java_com_jack_splayer_SPlay_InitView(
        JNIEnv *env,
        jobject instance, jobject surface) {
    //从Surface获取原生窗口
    ANativeWindow *win = ANativeWindow_fromSurface(env, surface);
//    view->SetRender(win);
//    if (player)
    IPlayerPorxy::Get()->InitView(win);

    //测试
//    XEGL::Get()->Init(win);//XEGL 不对外开放
//    XShader shader;
//    shader.Init();

//    IPlayerPorxy::Get()->InitView(win);
}

extern "C"
JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *res) {
//    FFDecode::InitHard(vm);

    IPlayerPorxy::Get()->Init(vm);
//    FFPlayerBuilder::InitHard(vm);

//    IPlayerPorxy::Get()->Init(vm);

    /*IPlayerPorxy::Get()->Open("/sdcard/v1080.mp4");
    IPlayerPorxy::Get()->Start();


    IPlayerPorxy::Get()->Open("/sdcard/1080.mp4");
    IPlayerPorxy::Get()->Start();*/

    //测试代码
//    player = FFPlayerBuilder::Get()->BuilderPlayer();

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

//    IDemux *de = new FFDemux();

//    IDecode *vdecode = new FFDecode();
//    IDecode *adecode = new FFDecode();
//    de->AddObs(vdecode);
//    de->AddObs(adecode);
//    view = new GLVideoView();
//    vdecode->AddObs(view);
//    IResample *resample = new FFResample();
//    adecode->AddObs(resample);
//
//    IAudioPlay *audioPlay = new SLAudioPlay();
//    resample->AddObs(audioPlay);
//
//    IPlayer::Get()->demux = de;
//    IPlayer::Get()->adecode = adecode;
//    IPlayer::Get()->vdecode = vdecode;
//    IPlayer::Get()->videoView = view;
//    IPlayer::Get()->resample = resample;
//    IPlayer::Get()->audioPlay = audioPlay;

//    IPlayerPorxy::Get()->Open("/sdcard/1080.mp4");
//    IPlayerPorxy::Get()->Start();

    //测试seek
//    IPlayerPorxy::Get()->Seek(0.5);
    return JNI_VERSION_1_4;
}

//extern "C" JNIEXPORT jstring JNICALL
//Java_com_jack_splayer_MainActivity_stringFromJNI(
//        JNIEnv *env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//
//    //Android8.0存在重音的问题，有可能是下方的代码执行了两次，更换调用位置
////    //测试代码
//////    TestObs *tobs = new TestObs();
////    IDemux *de = new FFDemux();
//////    de->AddObs(tobs);
////
////    de->Open("/sdcard/1080.mp4");
////
////    IDecode *vdecode = new FFDecode();
////    vdecode->Open(de->GetVPara(),true);
////
////    IDecode *adecode = new FFDecode();
////    adecode->Open(de->GetAPara());
////
////    de->AddObs(vdecode);
////    de->AddObs(adecode);
////
////    view = new GLVideoView();
////    vdecode->AddObs(view);
////
////    IResample *resample = new FFResample();
////    XParameter outPara = de->GetAPara();
////    resample->Open(de->GetAPara(),outPara);
////    adecode->AddObs(resample);
////
////    IAudioPlay *audioPlay = new SLAudioPlay();
////    audioPlay->StartPlay(outPara);
////    resample->AddObs(audioPlay);
////
////    de->Start();
////    vdecode->Start();
////    adecode->Start();
//
////    XSleep(3000);
////    de->Stop();
//
//    //测试在线程中进行读取
////    for(;;){
////        XData d = de->Read();
////        XLOGI("Read data size is %d",d.size);
////    }
//
//    return env->NewStringUTF(hello.c_str());
//}

extern "C"
JNIEXPORT void JNICALL
Java_com_jack_splayer_MainActivity_Seek(JNIEnv *env, jobject thiz, jdouble pos) {
    IPlayerPorxy::Get()->Seek(pos);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jack_splayer_SPlay_PlayOrPause(JNIEnv *env, jobject thiz) {
    IPlayerPorxy::Get()->SetPause(!IPlayerPorxy::Get()->IsPause());
}
extern "C"
JNIEXPORT jdouble JNICALL
Java_com_jack_splayer_MainActivity_PlayPos(JNIEnv *env, jobject thiz) {
    return IPlayerPorxy::Get()->PlayPos();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jack_splayer_OpenUrl_Open(JNIEnv *env, jobject thiz, jstring url_) {
    const char *url = env->GetStringUTFChars(url_, 0);
    IPlayerPorxy::Get()->Open(url);
    IPlayerPorxy::Get()->Start();
    env->ReleaseStringUTFChars(url_, url);
}