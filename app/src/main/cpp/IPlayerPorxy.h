//
// Created by jack on 2025-07-08.
//

#ifndef SPLAYER_IPLAYERPORXY_H
#define SPLAYER_IPLAYERPORXY_H

#include "IPlayer.h"
#include <mutex>

//即使多路视频播放，由一个IPlayerPorxy来进行控制即可

class IPlayerPorxy : public IPlayer {
public:
    static IPlayerPorxy *Get() {
        static IPlayerPorxy px;
        return &px;
    }

    void Init(void *vm = 0);

    virtual bool Open(const char *path);
    virtual void Close();

    virtual bool Seek(double pos);

    //获取当前的播放进度 0.0 ~ 1.0
    virtual double PlayPos();

//    virtual bool Seek(double pos);

    virtual bool Start();

    virtual void InitView(void *win);

    virtual void SetPause(bool isP);

    virtual bool IsPause();

protected:
    IPlayerPorxy() {}

    IPlayer *player = 0;
    std::mutex mux;
};


#endif //SPLAYER_IPLAYERPORXY_H
