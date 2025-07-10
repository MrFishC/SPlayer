//
// Created by jack on 2025-07-07.
//

#include "FFDemux.h"
#include "XLog.h"

//音视频同步 使用 分数转为浮点数
static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}

bool FFDemux::Open(const char *url) {
    Close();//FFDemux中定义的返回值类型 为bool，但是没有给返回值导致的问题。实际要给void的即可 //每一次Open之前都做一次Close
    XLOGI("Open file %s begin", url);

    mux.lock();
    int re = avformat_open_input(&ic, url, 0, 0);
    if (re != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("FFDemux open %s failed!", url);
        return false;
    }
    XLOGI("FFDemux open %s success!", url);

    //读取文件信息
    re = avformat_find_stream_info(ic, 0);
    if (re != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        XLOGE("avformat_find_stream_info %s failed!", url);
        return false;
    }

    this->totalMs = ic->duration / (AV_TIME_BASE / 1000);

    mux.unlock(); //这里不能写在 GetVPara()和GetAPara()的后面，因为这两个方法的内部可能还有锁，可能会导致死锁的产生
    XLOGI("total ms = %d!", totalMs);
    GetVPara();
    GetAPara();
    return true;
}

//对ic进行释放
void FFDemux::Close() {
    mux.lock();
    if (ic)
        avformat_close_input(&ic);
    mux.unlock();
}

//获取视频参数
XParameter FFDemux::GetVPara() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        return XParameter();
    }

    //获取了视频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (re < 0) {
        mux.unlock();
        XLOGE("av_find_best_stream failed!");
        return XParameter();
    }
    videoStream = re;
    XParameter para;
    para.para = ic->streams[re]->codecpar;
    mux.unlock();
    return para;
}

//获取音频参数
XParameter FFDemux::GetAPara() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        return XParameter();
    }

    //获取了视频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0) {
        mux.unlock();
        XLOGE("av_find_best_stream failed!");
        return XParameter();
    }
    audioStream = re;
    XParameter para;
    para.para = ic->streams[re]->codecpar;

//    para.channels = ic->streams[re]->codecpar->channels;
    para.channels = ic->streams[re]->codecpar->ch_layout.nb_channels;
    para.sample_rate = ic->streams[re]->codecpar->sample_rate;
    mux.unlock();
    return para;
}

XData FFDemux::Read() {
    mux.lock();
    if (!ic) {
        mux.unlock();
        return XData();
    }

    XData d;
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic, pkt);
    if (re != 0) {
        mux.unlock();//先释放锁 再释放pkt
        av_packet_free(&pkt);//细节点
        return XData();
    }
//    XLOGI("pack size if %d ptss %lld",pkt->size,pkt->pts);//日志信息 暂时注释
    d.data = (unsigned char *) pkt;
    d.size = pkt->size;
    if (pkt->stream_index == audioStream) {
        d.isAudio = true;
    } else if (pkt->stream_index == videoStream) {
        d.isAudio = false;
    } else {
        mux.unlock();
        av_packet_free(&pkt);
        return XData();
    }

    //音视频同步  转换pts  todo   退出：这种方案可能有一定的精度损失（后续对比零声学院的视频课程的计算方案）
    //r2d(ic->streams[pkt->stream_index]->time_base)  计算出来的是秒 需要 * 1000
    pkt->pts = pkt->pts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    pkt->dts = pkt->dts * (1000 * r2d(ic->streams[pkt->stream_index]->time_base));
    d.pts = (int) pkt->pts;//得到的是毫秒

    XLOGE("demux pts %d", d.pts);
    mux.unlock();
    return d;
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;

        //这里暂未考虑线程安全的问题

//        从 FFmpeg 4.0 开始：编解码器和复用器/解复用器会在首次使用时自动注册
//        //注册所有的封装器
//        av_register_all();
//        //注册所有的解码器
//        avcodec_register_all();
        //初始化网络
        avformat_network_init();
        XLOGI("register ffmpeg");
    }
}

bool FFDemux::Seek(double pos){
    if(pos<0 || pos > 1)
    {
        XLOGE("Seek value must 0.0~1.0");
        return false;
    }
    bool re = false;
    mux.lock();
    if(!ic)
    {
        mux.unlock();
        return false;
    }
    //清理读取的缓冲
    avformat_flush(ic);//不做清理的话，可能会造成少量帧的错误，错误可能不太明显，但是还是需要避免
    long long seekPts = 0;
    seekPts = ic->streams[videoStream]->duration*pos;

    //往后跳转到关键帧
    re = av_seek_frame(ic,videoStream,seekPts,AVSEEK_FLAG_FRAME|AVSEEK_FLAG_BACKWARD);
    mux.unlock();
    return re;
}