
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ZTENETLINK_H
#define __ZTENETLINK_H

#include "netlinkobserver.h"
#include "netlinkevent.h"

class ZteNetlink : public NetlinkObserver
{
public:
                ZteNetlink(int sock);
    virtual     ~ZteNetlink();

    virtual int on_event(NetlinkEvent* evt);

    static ZteNetlink* open();
};

#endif
