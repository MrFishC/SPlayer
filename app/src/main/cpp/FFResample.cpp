//
// Created by jack on 2025-07-08.
//

#include "FFResample.h"

extern "C"
{
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
}

#include "XLog.h"
#include <libavcodec/avcodec.h>

bool FFResample::Open(XParameter in, XParameter out) {
    Close();

    mux.lock();

    //音频重采样的初始化
    actx = swr_alloc();
    //在 FFmpeg 5.0+ 版本中，av_get_default_channel_layout() 已被移除，改用 av_channel_layout_default() 来获取默认的声道布局（AVChannelLayout）
    // 设置输入和输出的声道布局
    AVChannelLayout out_ch_layout;
    AVChannelLayout in_ch_layout;
    // 设置输出声道布局（如立体声）
    av_channel_layout_default(&out_ch_layout,
                              out.channels);  // out.channels=2 → AV_CHANNEL_LAYOUT_STEREO
    // 设置输入声道布局
    av_channel_layout_default(&in_ch_layout, in.para->ch_layout.nb_channels);

//    int ret = swr_alloc_set_opts2(
//            &actx,
//            &out_ch_layout,            // 输出声道布局
//            AV_SAMPLE_FMT_S16,         // 输出采样格式
//            ac->sample_rate,           // 输出采样率
//            &in_ch_layout,             // 输入声道布局
//            ac->sample_fmt,            // 输入采样格式
//            ac->sample_rate,           // 输入采样率
//            0,                         // 日志偏移
//            NULL                       // 日志上下文
//    );

//    actx = swr_alloc_set_opts(actx,
    int re = swr_alloc_set_opts2(&actx, &out_ch_layout,
//                              av_get_default_channel_layout(out.channels),
                                 AV_SAMPLE_FMT_S16, out.sample_rate,
//                              av_get_default_channel_layout(in.para->channels),
                                 &in_ch_layout,
                                 (AVSampleFormat) in.para->format, in.para->sample_rate,
                                 0, 0);
    if (re < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(re, errbuf, sizeof(errbuf));
        XLOGE("swr_alloc_set_opts2 failed: %s", errbuf);
        return false;
    }

//    int re = swr_init(actx);
    re = swr_init(actx);
    if (re != 0) {
        mux.unlock();
        XLOGE("swr_init failed!");
        return false;
    } else {
        XLOGI("swr_init success!");
    }

//    outChannels = in.para->channels;
    outChannels = in.para->ch_layout.nb_channels;
    outFormat = AV_SAMPLE_FMT_S16;

    mux.unlock();
    return true;
}

void FFResample::Close() {
    mux.lock();
    if (actx) {
        swr_free(&actx);
    }
    mux.unlock();
}

XData FFResample::Resample(XData indata) {
//    XLOGE("indata size is %d",indata.size);
    if (indata.size <= 0 || !indata.data) return XData();
    mux.lock();

    if (!actx) {
        mux.unlock();
        return XData();
    }

    AVFrame *frame = (AVFrame *) indata.data;

    //输出空间的分配
    XData out;
    int outsize =
            outChannels * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat) outFormat);
    if (outsize <= 0)return XData();
    out.Alloc(outsize);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.data;
    int len = swr_convert(actx, outArr, frame->nb_samples, (const uint8_t **) frame->data,
                          frame->nb_samples);
    if (len <= 0) {
        mux.unlock();
        out.Drop();
        return XData();
    }

    //音视频同步     out是直接new出来的，其pts需要被赋值
    out.pts = indata.pts;
    mux.unlock();
    return out;
}