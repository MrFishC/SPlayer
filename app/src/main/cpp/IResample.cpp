//
// Created by jack on 2025-07-08.
//

#include "IResample.h"

//IDecode::Main() 中调用了notify，会触发 IResample::Update
void IResample::Update(XData data){
    XData d = this->Resample(data);

    XLOGE("IResample Update 触发 %d",data.size);
    if(d.size > 0){
        this->Notify(d);//触发音频播放的Update， 即 IAudioPlay::Update
    }
}