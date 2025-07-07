//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XLOG_H
#define SPLAYER_XLOG_H

class XLog {

};

#include <android/log.h>
#define XLOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"SPlay",__VA_ARGS__)
#define XLOGI(...) __android_log_print(ANDROID_LOG_INFO,"SPlay",__VA_ARGS__)
#define XLOGE(...) __android_log_print(ANDROID_LOG_ERROR,"SPlay",__VA_ARGS__)
#else
#define XLOGD(...) printf("SPlay",__VA_ARGS__)
#define XLOGI(...) printf("SPlay",__VA_ARGS__)
#define XLOGE(...) printf("SPlay",__VA_ARGS__)

#endif //SPLAYER_XLOG_H
