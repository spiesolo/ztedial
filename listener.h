
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __LISTENER_H
#define __LISTENER_H

#include <pthread.h>

class Listener
{
private:
    int             fd;
    int             idle_time;
    int             ctrlfd[2];
    int             started;
    pthread_t       thread;

    static void*    thread_main(void*);
protected:
    virtual void    run();

    virtual bool    on_data_available() { return false; }
    virtual bool    on_error() { fd = -1; return true; }
    virtual bool    on_idle() { return true; }

    void            setfd(int _fd);
    void            set_idle_time(int _time);
public:
                    Listener(int _fd = -1);
    virtual         ~Listener();

    bool            start();
    bool            stop();
};

#endif
