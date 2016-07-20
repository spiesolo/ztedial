
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __MUTEX_H
#define __MUTEX_H

#include <pthread.h>

class Mutex
{
private:
    pthread_mutex_t         mutex;
    pthread_cond_t          cond;
public:
                            Mutex();
    virtual                 ~Mutex();

    bool                    wait(int timeout = 0);
    void                    signal();
    void                    broadcast();
    void                    lock();
    void                    unlock();
    bool                    trylock();
};

#endif
