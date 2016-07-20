
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "mutex.h"

Mutex::Mutex()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

bool Mutex::wait(int timeout /* in ms */)
{
    int ret;

    if (timeout > 0) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * 1000000;
        if (ts.tv_nsec > 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }

        ret = pthread_cond_timedwait(&cond, &mutex, &ts);
    } else
        ret = pthread_cond_wait(&cond, &mutex);

    return (ret == 0);
}

void Mutex::signal()
{
    pthread_cond_signal(&cond);
}

void Mutex::broadcast()
{
    pthread_cond_broadcast(&cond);
}

void Mutex::lock()
{
    pthread_mutex_lock(&mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&mutex);
}

bool Mutex::trylock()
{
    return (pthread_mutex_trylock(&mutex) == 0);
}
