//
// Created by jack on 2025-07-07.
//

#ifndef SPLAYER_XEGL_H
#define SPLAYER_XEGL_H

class XEGL {
public:
    virtual bool Init(void *win) = 0;
    virtual void Close() = 0;
    virtual void Draw() = 0;
    static XEGL *Get();

protected:
    XEGL(){

    }
};


#endif //SPLAYER_XEGL_H
