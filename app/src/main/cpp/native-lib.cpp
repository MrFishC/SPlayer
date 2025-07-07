#include <jni.h>
#include <string>
#include "FFDemux.h"

#include "XLog.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_jack_splayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    //测试代码
    IDemux *de = new FFDemux();
    de->Open("/sdcard/1080.mp4");
    for(;;){
        XData d = de->Read();
        XLOGI("Read data size is %d",d.size);
    }

    return env->NewStringUTF(hello.c_str());
}