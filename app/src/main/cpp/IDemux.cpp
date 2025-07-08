//
// Created by jack on 2025-07-07.
//

#include "IDemux.h"
#include "XLog.h"

void IDemux::Main(){
    while(!isExit){
        XData d = Read();
        if(d.size > 0){
            //先要做缓冲
            Notify(d);
        }
//        XLOGI("IDemux Read %d",d.size);//日志信息过多，该处暂时注释
//        if(d.size<=0)break;
    }
}