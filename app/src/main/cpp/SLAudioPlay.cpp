//
// Created by jack on 2025-07-08.
//

#include "SLAudioPlay.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "XLog.h"

//static 修饰，仅在本cpp文件有效
static SLObjectItf engineSL = NULL;
static SLEngineItf eng = NULL;
static SLObjectItf mix = NULL;

static SLObjectItf player = NULL;                      // 播放器对象（未初始化）  音频播放器的基础对象，通过 CreateAudioPlayer 创建，后续需要 Realize 初始化
static SLPlayItf iplayer = NULL;                       // 播放控制接口（未初始化） 播放控制接口（SLPlayItf），用于控制播放状态（如播放/暂停/停止）。
static SLAndroidSimpleBufferQueueItf pcmQue = NULL;    // 缓冲队列接口（未初始化） Android 专用的缓冲队列接口（SLAndroidSimpleBufferQueueItf），用于动态填充 PCM 数据

SLAudioPlay::SLAudioPlay() {
    buf = new unsigned char[1024 * 1024];
}

SLAudioPlay::~SLAudioPlay() {
    delete buf;
    buf = 0;
}

static SLEngineItf CreateSL() {
    SLresult re;// 用于接收 OpenSL ES 操作结果
    SLEngineItf en;// 引擎接口
    // slCreateEngine：创建引擎对象。是 OpenSL ES 的入口点，必须先调用它才能使用其他 OpenSL ES 功能
    //参数1：pEngine (输出参数)，指向 SLObjectItf 指针的指针，用于接收创建的引擎对象。如果创建成功，*pEngine 会被赋值为新引擎对象的引用。
    //参数2：numOptions (输入参数)，指定 pEngineOptions 数组中的选项数量。如果为 0，表示不使用任何选项。
    //参数3：pEngineOptions (输入参数)，指向 SLEngineOption 结构体数组的指针，用于配置引擎。
    //每个 SLEngineOption 包含一个 SLuint32 类型的键值对（例如性能模式、线程优先级等）
    //如果 numOptions 为 0，此参数应为 NULL
    //参数4:numInterfaces (输入参数)
    //指定 pInterfaceIds 数组中请求的接口数量。如果为 0，表示不立即请求任何接口。
    //参数5：pInterfaceIds (输入参数)
    //指向 SLInterfaceID 数组的指针，列出需要从引擎对象获取的接口（如 SL_IID_ENGINE）。
    //如果 numInterfaces 为 0，此参数应为 NULL。
    //参数6：pInterfaceRequired (输入参数)
    //指向 SLboolean 数组的指针，标记每个请求的接口是否是必需的（SL_BOOLEAN_TRUE/SL_BOOLEAN_FALSE）。
    //如果 numInterfaces 为 0，此参数应为 NULL。
    re = slCreateEngine(&engineSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) return NULL;
    // 实现（初始化）引擎对象  Realize：初始化一个 OpenSL ES 对象
    re = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) return NULL;
    // 获取引擎接口 GetInterface：用于从已初始化的对象中获取特定功能的接口
    // 通过它，可以访问对象提供的各种音频操作功能（如播放控制、音量调节等）。
    re = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &en);
    if (re != SL_RESULT_SUCCESS) return NULL;
    return en;
}

void SLAudioPlay::PlayCall(void *bufq) {
    if (!bufq)return;

//    SLAndroidSimpleBufferQueueItf bf = (SLAndroidSimpleBufferQueueItf) bufq;
    //XLOGE("SLAudioPlay::PlayCall");
    //阻塞
    XData d = GetData();//GetData 有可能锁在这里
    if (d.size <= 0) {
        XLOGE("GetData() size is 0");
        return;
    }
    if (!buf)
        return;
    memcpy(buf, d.data, d.size);
    mux.lock();
    //如果pcmQue已经被清理掉了，仍执行(*bf)->Enqueue(bf,buf,d.size) 则会出现问题
    if (pcmQue && (*pcmQue))
        (*pcmQue)->Enqueue(pcmQue, buf, d.size);// 将数据加入播放队列
//    (*bf)->Enqueue(bf,buf,d.size);//注释掉  修复835cpu的手机中测试并修正了音频播放Close的 bug
    mux.unlock();
    d.Drop();
}

//PCM 数据回调函数
static void PcmCall(SLAndroidSimpleBufferQueueItf bf, void *contex) {
    SLAudioPlay *ap = (SLAudioPlay *) contex;
    if (!ap) {
        XLOGE("PcmCall failed contex is null!");
        return;
    }
    ap->PlayCall((void *) bf);
}

void SLAudioPlay::Close() {

    IAudioPlay::Clear();

    mux.lock();

    //停止播放
    if (iplayer && (*iplayer)) {
        (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);
    }
    //清理播放队列
    if (pcmQue && (*pcmQue)) {
        (*pcmQue)->Clear(pcmQue);
    }
    //销毁player对象                下方的销毁  可能 存在无法销毁的情况         如果PcmCall线程没有被终止掉的话
    if (player && (*player)) {
        (*player)->Destroy(player);
    }
    //销毁混音器
    if (mix && (*mix)) {
        (*mix)->Destroy(mix);
    }

    //销毁播放引擎
    if (engineSL && (*engineSL)) {
        (*engineSL)->Destroy(engineSL);
    }

    engineSL = NULL;
    eng = NULL;
    mix = NULL;
    player = NULL;
    iplayer = NULL;
    pcmQue = NULL;

    mux.unlock();
}

bool SLAudioPlay::StartPlay(XParameter out) {
    Close();

    mux.lock();
    //1 创建引擎
    eng = CreateSL();
    if (eng) {
        XLOGI("CreateSL success！ ");
    } else {
        mux.unlock();
        XLOGE("CreateSL failed！ ");
        return false;
    }

    //2 创建混音器
    SLresult re = 0;
    //CreateOutputMix：是 OpenSL ES 中用于创建音频输出混音器（Output Mix）的函数，属于 SLEngineItf 接口。
    //参数1：引擎接口
    //参数2：返回的混音器对象
    //参数3：请求的混音器接口数量     不请求接口
    //参数4：请求的接口ID数组        无接口ID数组
    //参数5：无接口必需标记          接口是否必需的数组
    re = (*eng)->CreateOutputMix(eng, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        mux.unlock();
        XLOGE("SL_RESULT_SUCCESS failed!");
        return false;
    }
    //(*mix)->Realize：初始化混音器对象（Output Mix）的关键函数。创建混音器后，必须调用 Realize 才能使用它
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        mux.unlock();
        XLOGE("(*mix)->Realize failed!");
        return false;
    }
    //定义了一个 OpenSL ES 的音频数据定位器（Data Locator），用于指定音频数据的输出目标是一个混音器（Output Mix）对象。
    //它是音频数据流（如播放器的输出）和物理音频设备（如扬声器）之间的桥梁。
    //参数1：定位器类型（固定为 SL_DATALOCATOR_OUTPUTMIX，常量：表示数据定位器的类型是“输出混音器”（即音频数据的目标是混音器））
    //参数2：指向混音器对象的指针
    SLDataLocator_OutputMix outmix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    //数据最终输出到混音器
    //这行代码定义了一个 OpenSL ES 的数据接收端（Data Sink），用于指定音频数据的最终输出目标。
    //参数2：可选的格式信息（通常设为 NULL 或 0）
    SLDataSink audioSink = {&outmix, 0};

    //3 配置音频信息
    //缓冲队列          定义数据来源（PCM 缓冲队列）
    //定义了一个 OpenSL ES 的 Android 专用缓冲队列定位器。用于指定音频数据的来源是一个内存中的 PCM 缓冲队列。它是实现音频流式播放（如实时解码网络音频或播放 PCM 数据）的核心组件。
    //参数1：固定值，表示这是一个 Android 专用的缓冲队列定位器。
    SLDataLocator_AndroidSimpleBufferQueue que = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                  10};// 队列缓冲区数量
    //音频格式
    //SLDataFormat_PCM 结构体：用于明确音频数据的存储格式和参数     定义了一个 PCM 音频格式描述符
    //音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放PCM格式的数据  // 数据类型（固定为SL_DATAFORMAT_PCM）
            (SLuint32) out.channels,//    声道数
            (SLuint32) out.sample_rate * 1000,// 采样率 44.1kHz
            SL_PCMSAMPLEFORMAT_FIXED_16,// 每个采样位数 16bit
            SL_PCMSAMPLEFORMAT_FIXED_16,//容器大小（通常等于bitsPerSample）//bitsPerSample 是 PCM 音频的“分辨率”，决定每个采样值的精度。16-bit 是通用选择，兼顾音质和效率。
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// 声道布局
            SL_BYTEORDER_LITTLEENDIAN//字节序，小端（Intel架构常用）           其它：大端（网络传输常用）
    };

    //组合了数据来源定位器和数据格式，构成完整的音频输入源描述。
    //参数1：数据定位器（如缓冲队列）
    //参数2：数据格式（如PCM描述）
    //为什么需要这两者？
    //缓冲队列定位器 (que)：解决数据从哪里来的问题（内存实时填充）。
    //PCM格式描述 (pcm)：解决数据如何解析的问题（避免乱码或杂音）。
    SLDataSource ds = {&que, &pcm};// 数据源
    //音频数据流逻辑       [RAW PCM数据] → [缓冲队列(que)] → [格式解析(pcm)] → [音频播放器]

    //4 创建播放器
    const SLInterfaceID ids[] = {
            SL_IID_BUFFERQUEUE};// 需要的接口 ID        指定播放器需要支持的接口类型，此处仅请求 SL_IID_BUFFERQUEUE（缓冲队列接口）。
    const SLboolean req[] = {
            SL_BOOLEAN_TRUE};// 是否必须                   标记接口是否强制需要（SL_BOOLEAN_TRUE 表示必需，若无法获取则播放器创建失败）。
    //创建播放器时传入数据源
        //参数1：引擎接口（SLEngineItf），用于创建播放器。
        //参数2：输出参数，接收创建的播放器对象。
        //参数3：数据源（SLDataSource），指定音频数据的来源（如缓冲队列 + PCM 格式）。
        //参数4：数据接收端（SLDataSink），指定音频输出目标（如混音器）。
        //参数5：计算接口数量（此处为1）。
        //参数6：需要的接口 ID 数组（此处为 SL_IID_BUFFERQUEUE）。
    re = (*eng)->CreateAudioPlayer(eng, &player, &ds, &audioSink,
                                   sizeof(ids) / sizeof(SLInterfaceID), ids, req);
    if (re != SL_RESULT_SUCCESS) {
        mux.unlock();
        XLOGE("CreateAudioPlayer failed!");
        return false;
    } else {
        XLOGI("CreateAudioPlayer success!");
    }
    (*player)->Realize(player, SL_BOOLEAN_FALSE);// 初始化播放器
    //通过 GetInterface 获取具体功能接口
    //获取播放控制接口
    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        mux.unlock();
        XLOGE("GetInterface SL_IID_PLAY failed!");
        return false;
    }
    // 获取缓冲队列接口
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQue);
    if (re != SL_RESULT_SUCCESS) {
        mux.unlock();
        XLOGE("GetInterface SL_IID_BUFFERQUEUE failed!");
        return false;
    }

    //设置回调函数，播放队列空调用
    //5.设置回调函数从文件读取 PCM 数据

    //注册回调函数（当队列需要数据时触发）
    //参数1：缓冲队列接口指针（通过 GetInterface 获取）
    //参数2：回调函数指针，原型为 void callback(SLAndroidSimpleBufferQueueItf bq, void *context)。
    //参数3：传递给回调函数的用户上下文。
    //回调时机：当缓冲队列为空或即将耗尽时，系统调用 PcmCall 请求新数据。
    (*pcmQue)->RegisterCallback(pcmQue, PcmCall, this);

    //设置为播放状态
    //6.启动播放流程
    //启动播放
    //参数1：播放控制接口指针
    //参数2：设置为播放状态。其他可选值：
    //- SL_PLAYSTATE_PAUSED（暂停）
    //- SL_PLAYSTATE_STOPPED（停止）
    //关键点之一，无数据时行为：若缓冲队列为空，播放可能静音或卡顿（需提前或通过回调填充数据）。
    //异步操作，无阻塞。
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);

    //启动队列回调
    // 手动触发第一次数据填充 启动队列回调（传入空数据触发第一次回调）
    //作用：主动向缓冲队列提交一个空数据块，强制触发回调函数 PcmCall。
    //参数1：缓冲队列接口指针。
    //参数2：空数据指针（此处无实际意义，仅用于触发回调）
    //参数3：数据大小（字节数）。此处传 1 仅满足参数要求，实际无效。 仅触发回调，不提供有效数据
    //替代方案：也可直接填充有效数据（如首帧 PCM）
    //安全注意：空数据不会导致崩溃，但后续回调中必须提交有效数据。
    (*pcmQue)->Enqueue(pcmQue, "", 1);
    isExit = false; //todo
    mux.unlock();
    XLOGI("SLAudioPlay::StartPlay success!");
    return true;
}