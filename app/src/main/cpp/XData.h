//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XDATA_H
#define SPLAYER_XDATA_H

enum XDataType {
    AVPACKET_TYPE = 0,
    UCHAR_TYPE = 1
};

struct XData {
    int type = 0;
    unsigned char *data = 0;
    //datas:存放的是解码后的数据
    unsigned char *datas[8] = {0};
    int size = 0;
    bool isAudio = false;
    //视频数据包含宽高
    int width = 0;
    int height = 0;

    int format = 0;//赋值的时机 需要清楚     FFDecode::RecvFrame解码成功之后知道其格式

    bool Alloc(int size, const char *data = 0);

    void Drop();
};


#endif //SPLAYER_XDATA_H
