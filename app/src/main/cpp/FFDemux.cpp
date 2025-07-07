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

    return true;
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
    XLOGI("pack size if %d ptss %lld",pkt->size,pkt->pts);
    d.data = (unsigned char*)pkt;
    d.size = pkt->size;

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