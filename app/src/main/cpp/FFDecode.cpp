//
// Created by jack on 2025-07-07.
//

#include "FFDecode.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/jni.h>
#include "libavutil/imgutils.h"
}

void FFDecode::InitHard(void *vm) {
    av_jni_set_java_vm(vm, 0);
}

bool FFDecode::Open(XParameter para, bool isHard) {
    Close();

    if (!para.para) return false;
    AVCodecParameters *p = para.para;

    //1.查找编码器
    const AVCodec *cd = avcodec_find_decoder(p->codec_id);

    if (isHard) {
        cd = avcodec_find_decoder_by_name("h264_mediacodec");
    }

    if (!cd) {
        XLOGE("avcodec_find_decoder %d failed %d!", p->codec_id, isHard);
        return false;
    }
    XLOGI("avcodec_find_decoder success %d!", isHard);

    mux.lock();
    //2 创建解码上下文，并复制参数
    codec = avcodec_alloc_context3(cd);
    avcodec_parameters_to_context(codec, p);

    codec->thread_count = 8;

    //3 打开解码器
    int re = avcodec_open2(codec, 0, 0);
    if (re != 0) {
        mux.unlock();
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        XLOGE("%s", buf);
        return false;
    }

    if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        this->isAudio = false;
    } else {
        this->isAudio = true;
    }

    mux.unlock();
    XLOGI("avcodec_open2 success!");
    return true;
}

void FFDecode::Close() {
    //关闭的时候一定要调用清理的工作
    IDecode::Clear();

    mux.lock();
    pts = 0;
    if (frame)
        av_frame_free(&frame);
    if (codec) {
        avcodec_close(codec);
        avcodec_free_context(&codec);
    }
    mux.unlock();
}

//seek时 对于解码器的缓冲 一定要把之前解码的内容给清除掉，若不做清除，则可能会出现花屏  Clear方法在FFDecode中要进一步实现一下
void FFDecode::Clear() {
    IDecode::Clear();
    mux.lock();
    if (codec)
        avcodec_flush_buffers(codec);
    mux.unlock();
}

bool FFDecode::SendPacket(XData pkt) {
    if (pkt.size <= 0 || !pkt.data)return false;
    mux.lock();
    if (!codec) {
        mux.unlock();
        return false;
    }
    int re = avcodec_send_packet(codec, (AVPacket *) pkt.data);
    mux.unlock();
    if (re != 0) {
        return false;
    }
    return true;
}

//从线程中获取解码结果
XData FFDecode::RecvFrame() {
    mux.lock();
    if (!codec) {
        mux.unlock();
        return XData();
    }
    if (!frame) {
        frame = av_frame_alloc();
    }
    int re = avcodec_receive_frame(codec, frame);
    if (re != 0) {
        mux.unlock();
        return XData();
    }
    XData d;
    d.data = (unsigned char *) frame;
    if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        //大小的计算
//        d.size = (frame->linesize[0] + frame->linesize[1] + frame->linesize[2]) * frame->height;//猜测：应该是仅做粗略的计算

        //使用FFmpeg工具函数   使用ffmpeg的函数可能更合适
        d.size = av_image_get_buffer_size(
                (AVPixelFormat) frame->format,
                frame->width,
                frame->height,
                1 // 行对齐
        );

        //linesize 的含义
        //frame->linesize[i] 表示第 i 个平面（Y、U、V分量）中一行像素的字节数。由于内存对齐（如16/32字节对齐），linesize 可能略大于实际的图像宽度。

        //YUV格式的存储结构
        //Y分量（亮度）：完整分辨率（width × height），对应 frame->data[0]。
        //U/V分量（色度）：通常为半分辨率（如 width/2 × height/2），对应 frame->data[1] 和 frame->data[2]（假设是YUV420P格式）。

        //linesize[0] * height：Y分量的总大小（行字节数 × 行数）。
        //linesize[1] * (height/2)：U分量的总大小（色度行字节数 × 半行数）。
        //linesize[2] * (height/2)：V分量的总大小（同上）。
        //代码中的写法：
        //原代码简化为 (linesize[0] + linesize[1] + linesize[2]) * height，这实际上是近似计算，可能存在误差（尤其在非YUV420P格式时）。

        //原代码通过累加三个平面的 linesize 并乘以高度的方式，快速估算了YUV420P视频帧的数据大小。
        //虽然存在对齐冗余和格式假设的局限性，但在大多数场景下能正常工作。精确实现需结合像素格式或调用FFmpeg的辅助函数

        d.width = frame->width;
        d.height = frame->height;
    } else {
        //样本字节数 * 单通道样本数 * 通道数
        d.size = av_get_bytes_per_sample((AVSampleFormat) frame->format) * frame->nb_samples * 2;
    }

    d.format = frame->format;

//    if (!isAudio)
//        XLOGE("data format is %d", frame->format);
    //将解码后的数据放入到d.datas中
    memcpy(d.datas, frame->data, sizeof(d.datas));

    //音视频同步  d.pts需要被赋值
    d.pts = frame->pts;

    pts = d.pts;
    mux.unlock();
    return d;
}