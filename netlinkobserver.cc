
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include "netlinkobserver.h"
#include "uevent.h"

NetlinkObserver::NetlinkObserver(int sock)
    : Listener(sock), sockfd(sock)
{
}

NetlinkObserver::~NetlinkObserver()
{
}

#define UEVENT_MSG_LEN 1024

void NetlinkObserver::handle_uevent()
{
    char msg[UEVENT_MSG_LEN+2];
    ssize_t count;

    while ((count = uevent_kernel_multicast_recv(sockfd, msg, UEVENT_MSG_LEN)) > 0) {
        if (count >= UEVENT_MSG_LEN)
            continue;

        msg[count] = msg[count+1] = '\0';

        NetlinkEvent *evt = NetlinkEvent::decodeFromMessage((const char *)msg);

        on_event(evt);

        delete evt;
    }
}

bool NetlinkObserver::on_data_available()
{
    handle_uevent();
    return true;
}
