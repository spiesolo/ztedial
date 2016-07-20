
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __NETLINKOBSERVER_H
#define __NETLINKOBSERVER_H

#include <pthread.h>
#include "listener.h"
#include "netlinkevent.h"

class NetlinkObserver : public Listener
{
private:
    int             sockfd;

    void            handle_uevent();
public:
                    NetlinkObserver(int sock);
    virtual         ~NetlinkObserver();

    virtual bool    on_data_available();
    virtual int     on_event(NetlinkEvent *evt) = 0;
};

#endif
