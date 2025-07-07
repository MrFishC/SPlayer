//
// Created by jack on 2025-07-07.
//

#include "XData.h"

extern "C" {
#include <libavformat/avformat.h>
}

void XData::Drop() {
    if(!data) return;
    av_packet_free((AVPacket **)&data);
    data = 0;
    size = 0;
}