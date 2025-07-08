//
// Created by jack on 2025-07-07.
//

#include "FFDemux.h"
#include "XLog.h"

bool FFDemux::Open(const char *url) {
    XLOGI("Open file %s begin",url);
    int re = avformat_open_input(&ic,url,0,0);
    if(re != 0 )
    {
//        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof(buf));
        XLOGE("FFDemux open %s failed!",url);
        return false;
    }
    XLOGI("FFDemux open %s success!",url);

    //读取文件信息
    re = avformat_find_stream_info(ic,0);
    if(re != 0 )
    {
//        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof(buf));
        XLOGE("avformat_find_stream_info %s failed!",url);
        return false;
    }

    this->totalMs = ic->duration/(AV_TIME_BASE/1000);

//    mux.unlock();
    XLOGI("total ms = %d!",totalMs);
    GetVPara();
    GetAPara();
    return true;
}

//获取视频参数
XParameter FFDemux::GetVPara(){
    if(!ic){
        return XParameter();
    }

    //获取了视频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (re < 0) {
//        mux.unlock();
        XLOGE("av_find_best_stream failed!");
        return XParameter();
    }
    videoStream = re;
    XParameter para;
    para.para = ic->streams[re]->codecpar;
    return para;
}

//获取音频参数
XParameter FFDemux::GetAPara(){
    if(!ic){
        return XParameter();
    }

    //获取了视频流索引
    int re = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (re < 0) {
//        mux.unlock();
        XLOGE("av_find_best_stream failed!");
        return XParameter();
    }
    audioStream = re;
    XParameter para;
    para.para = ic->streams[re]->codecpar;

    para.channels = ic->streams[re]->codecpar->channels;
    para.sample_rate = ic->streams[re]->codecpar->sample_rate;

    return para;
}

XData FFDemux::Read() {
    if(!ic)return XData();

    XData d;
    AVPacket *pkt = av_packet_alloc();
    int re = av_read_frame(ic,pkt);
    if(re != 0)
    {
//        mux.unlock();
        av_packet_free(&pkt);//细节点
        return XData();
    }
//    XLOGI("pack size if %d ptss %lld",pkt->size,pkt->pts);//日志信息 暂时注释
    d.data = (unsigned char*)pkt;
    d.size = pkt->size;
    if(pkt->stream_index == audioStream){
        d.isAudio = true;
    }else if(pkt->stream_index == videoStream){
        d.isAudio = false;
    }else{
        av_packet_free(&pkt);
        return XData();
    }
    return d;
}

FFDemux::FFDemux(){
    static bool isFirst = true;
    if(isFirst){
        isFirst = false;

        //这里暂未考虑线程安全的问题

        //注册所有的封装器
        av_register_all();
        //注册所有的解码器
        avcodec_register_all();
        //初始化网络
        avformat_network_init();
        XLOGI("register ffmpeg");
    }
}