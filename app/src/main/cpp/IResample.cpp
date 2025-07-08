//
// Created by jack on 2025-07-08.
//

#include "IResample.h"
void IResample::Update(XData data){
    XData d = this->Resample(data);

    XLOGE("IResample Update 触发 %d",data.size);
    if(d.size > 0){
//        Notify(d);//todo
        this->Notify(d);
    }
}