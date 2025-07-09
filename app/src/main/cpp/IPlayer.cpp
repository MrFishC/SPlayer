//
// Created by jack on 2025-07-08.
//

#include "IPlayer.h"
#include "IDemux.h"
#include "IDecode.h"
#include "IAudioPlay.h"
#include "IVideoView.h"
#include "IResample.h"
#include "XLog.h"

IPlayer *IPlayer::Get(unsigned char index) {
    static IPlayer p[256];
    return &p[index];
}

void IPlayer::Close() {
    mux.lock();
    //先关闭主题线程，再清理观察者
    //同步线程
    //解封装线程
    //音、视频 解码线程
    //音频缓冲（没有做线程，不用处理）
    //清理缓冲队列    前面先关闭线程，可以保证缓冲队列不会增加
    //清理资源

    //2 先关闭主体线程，再清理观察者
    //同步线程
    XThread::Stop();
    //解封装线程
    if (demux)
        demux->Stop();
    //音、视频 解码线程
    if (vdecode)
        vdecode->Stop();
    if (adecode)
        adecode->Stop();
    if (audioPlay)
        audioPlay->Stop();

    //2 清理缓冲队列
    if (vdecode)
        vdecode->Clear();
    if (adecode)
        adecode->Clear();
    if (audioPlay)
        audioPlay->Clear();

    //3 清理资源
    if (audioPlay)
        audioPlay->Close();
    if (videoView)
        videoView->Close();
    if (vdecode)
        vdecode->Close();
    if (adecode)
        adecode->Close();
    if (demux)
        demux->Close();
    mux.unlock();
}

bool IPlayer::Open(const char *path) {
    Close();//todo

    mux.lock();

    if (!demux || !demux->Open(path)) {
//        mux.unlock();
        XLOGE("demux->Open %s failed!", path);
        mux.unlock();
        return false;
    }

    //解码 解码可能不需要，如果是解封之后就是原始数据
    if (!vdecode || !vdecode->Open(demux->GetVPara(), isHardDecode)) {
        XLOGE("vdecode->Open %s failed!", path);
//        return false;
    }
    if (!adecode || !adecode->Open(demux->GetAPara())) {
        XLOGE("adecode->Open %s failed!", path);
//        return false;
    }

    //重采样 有可能不需要，解码后或者解封后可能是直接能播放的数据
//    if (outPara.sample_rate <= 0)//若仅仅提供默认值，该if语句不会执行的。在播放rtmp时，该if下的语句没有执行，会导致有杂音问题 todo
    outPara = demux->GetAPara();
    if (!resample || !resample->Open(demux->GetAPara(), outPara)) {
        XLOGE("resample->Open %s failed!", path);
    }
    mux.unlock();
    return true;
}

bool IPlayer::Start() {
//    mux.lock();
//    if (!demux || !demux->Start()) {
//        XLOGE("demux->Start failed!");
//        mux.unlock();
//        return false;
//    }
//    if (adecode)
//        adecode->Start();
//    if (audioPlay)
//        audioPlay->StartPlay(outPara);
//    if (vdecode)
//        vdecode->Start();
//
//    XThread::Start();//启动 IPlayer::Main()
//
//    mux.unlock();
//    return true;

//画面一开始是黑屏/白屏  过一会儿 显示正常
//在IDecode::Update方法当中 线程没有启动时 将数据给丢掉了
//简单的处理方案  更换一下启动的顺序
/** 先启动 音、视频解码线程，而音频解码线程启动需要audioPlay先运行正常 **/

//    void IDecode::Update(XData pkt) {
//        if (pkt.isAudio != isAudio) {
//            return;
//        }
//
//        while (!isExit) {
//            packsMutex.lock();
//            if (packs.size() < maxList) {
//                //生产者
//                packs.push_back(pkt);
//                packsMutex.unlock();
//                break;
//            }
//            packsMutex.unlock();
//            XSleep(1);
//        }
//    }

    mux.lock();
    if (audioPlay)
        audioPlay->StartPlay(outPara);
    //启动解码线程
    if (adecode)
        adecode->Start();
    //启动解码线程
    if (vdecode)
        vdecode->Start();
    //启动封装器
    if (!demux || !demux->Start()) {
        XLOGE("demux->Start failed!");
        mux.unlock();
        return false;
    }
    //启动同步线程
    XThread::Start();//启动 IPlayer::Main()

    mux.unlock();
    return true;
}

//窗口创建之后调用该方法
void IPlayer::InitView(void *win) {
    if (videoView) {
        //先清理 再通过SetRender来传递 void *view = 0;  自己总结的（后续功底上来再验证）
        videoView->Close();//不在这里增加该行代码会导致  重复打开视频时， 间歇性的出现 播放不成功的问题。主要是因为多线程相关
        videoView->SetRender(win);
    }
}

void IPlayer::Main() {
    while (!isExit) {
        mux.lock();

        //同步的核心逻辑
        if (!audioPlay || !vdecode)//这两者有一个不存在，则没必要进行同步
        {
            mux.unlock();
            XSleep(2);
            continue;
        }
        //同步的核心逻辑
        //******************** 获取音频的pts 告诉视频 *************************
        //现在的音频和视频的 时间基数是不一致的  要换成统一的时间单位
        int apts = audioPlay->pts;
        XLOGE("apts = %d", apts);
        vdecode->synPts = apts;

        mux.unlock();
        XSleep(2);
    }
}

double IPlayer::PlayPos() {
    double pos = 0.0;
    mux.lock();

    int total = 0;
    if (demux)
        total = demux->totalMs;
    if (total > 0) {
        if (vdecode) {
            pos = (double) vdecode->pts / (double) total;
        }
    }
    mux.unlock();
    return pos;
}

//seek的整体测试，发现还是存在一定的时间消耗
bool IPlayer::Seek(double pos) {
    bool re = false;
    if (!demux) return false;

    //暂停所有线程 （结束之后要恢复暂停）  seek的第1步
    SetPause(true);
    mux.lock();
    //清理缓冲 seek的第2步
    //2 清理缓冲队列          补充：对于解码器的缓冲 一定要把之前解码的内容给清除掉，若不做清除，则可能会出现花屏  Clear方法在FFDecode中要进一步实现一下
    if (vdecode)
        vdecode->Clear(); //清理缓冲队列，清理ffmpeg的缓冲
    if (adecode)
        adecode->Clear();
    if (audioPlay)
        audioPlay->Clear();

    //缓冲清理完 开始seek
    re = demux->Seek(pos); //seek跳转到关键帧（关键帧不一定是我们seek操作时想要的位置）
    //OpenCV的做法也是类似 但是我们这里更加复杂，因为涉及到了多线程
    if (!vdecode) {
        mux.unlock();
        SetPause(false);
        return re;
    }
    //解码到实际需要显示的帧         pos*demux->totalMs = 实际想要显示的位置
    int seekPts = pos * demux->totalMs;
    while (!isExit)//下方代码需要好好再消化消化 大体思路能理解
    {
        XData pkt = demux->Read();
        if (pkt.size <= 0)break;
        if (pkt.isAudio) {
            if (pkt.pts < seekPts) {
                pkt.Drop();
                continue;
            }
            //（需要播放的音频）写入缓冲队列
            demux->Notify(pkt);//通知监听者adecode
            continue;
        }

        //解码需要显示的帧之前的数据     只解码不显示 当满足 data.pts >= seekPts 时再显示
        vdecode->SendPacket(pkt);
        pkt.Drop();
        XData data = vdecode->RecvFrame();
        if (data.size <= 0) {
            continue;
        }
        if (data.pts >= seekPts) {
            //猜测：可能跟视频中讲的报错原因不一致，后续跟进升级该项目
            //不注释，报错：call to OpenGL ES API with no current context (logged once per thread)"
            //vdecode->Notify(data);//不注释 会存在报错。  手机中可能不会出现该错误，  限制了只能在一个线程中处理
            break;
        }
    }
    mux.unlock();

    SetPause(false);//恢复暂停    注意位置，SetPause内部也加了锁   要避免死锁的发生
    return re;
}

void IPlayer::SetPause(bool isP) {
    mux.lock();
    XThread::SetPause(isP);
    if (demux)
        demux->SetPause(isP);
    if (vdecode)
        vdecode->SetPause(isP);
    if (adecode)
        adecode->SetPause(isP);
    if (audioPlay)
        audioPlay->SetPause(isP);
    mux.unlock();
}