
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uevent.h"
#include "ztenetlink.h"
#include "dispatchmanager.h"

ZteNetlink* ZteNetlink::open()
{
    int sock = uevent_open_socket(0, true);
    if (sock >= 0)
            return new ZteNetlink(sock);
    return NULL;
}

ZteNetlink::ZteNetlink(int sock)
    : NetlinkObserver(sock)
{
}

ZteNetlink::~ZteNetlink()
{
}

int ZteNetlink::on_event(NetlinkEvent *evt)
{
    if (evt->getAction() == NetlinkEvent::NlActionAdd) {
        const char *subsystem = evt->getSubsystem();

        if (strncmp(subsystem, "tty", 3))
            return 0;

        const char *devname = evt->getDevname();
        if (strncmp(devname, "tty", 3) || isdigit(devname[3]))
            return 0;

        int idVendor = evt->getIdVendor();
        int idProduct = evt->getIdProduct();

        if (idVendor != 0x19d2 && idProduct != 0x0199)
            return 0;

        const char *interface = evt->getInterface();
        if (!strncmp(interface, "USB-AT", 6)) {
            char message[64];
            snprintf(message, sizeof(message), "ACTION=add, DEVNODE=/dev/%s", devname);
            DispatchManager::Post(DispatchManager::main_id, message);
        }
    }

    return 0;
}
