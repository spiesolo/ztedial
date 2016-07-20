
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __SIGNALLISTENER_H
#define __SIGNALLISTENER_H

#include <stdio.h>
#include <list>

typedef std::list<int>   SignalContainers;

class SignalListener
{
private:
    static void     signal_handler(int sig);

    SignalContainers* signals;
    
public:
                    SignalListener();
    virtual         ~SignalListener();

    void            watch(int sig);
    void            clean();
};

#endif
