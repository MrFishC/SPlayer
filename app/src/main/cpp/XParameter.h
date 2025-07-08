//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XPARAMETER_H
#define SPLAYER_XPARAMETER_H

struct AVCodecParameters;
class XParameter {
public:
    AVCodecParameters *para = 0;
    int channels = 2;
    int sample_rate = 44100;
};


#endif //SPLAYER_XPARAMETER_H
