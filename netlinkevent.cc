
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "netlinkevent.h"

const int NetlinkEvent::NlActionUnknown = 0;
const int NetlinkEvent::NlActionAdd = 1;
const int NetlinkEvent::NlActionRemove = 2;
const int NetlinkEvent::NlActionMove = 3;
const int NetlinkEvent::NlActionChange = 4;

NetlinkEvent::NetlinkEvent()
{
    seqnum = 0;
    action = NetlinkEvent::NlActionUnknown;
    devpath = NULL;
    subsystem = NULL;
    devname = NULL;
    devtype = NULL;
    interface = NULL;
    major = -1;
    minor = -1;
    idVendor = 0;
    idProduct = 0;
}

NetlinkEvent::~NetlinkEvent()
{
    if (devpath)
        free((void *)devpath);
    if (subsystem)
        free((void *)subsystem);
    if (devname)
        free((void *)devname);
    if (devtype)
        free((void *)devtype);
    if (interface)
        free((void *)interface);
}

NetlinkEvent *NetlinkEvent::decodeFromMessage(const char *msg)
{
    NetlinkEvent *evt = new NetlinkEvent();

    while (*msg) {
        if (!strncmp(msg, "SEQNUM=", 7)) {
            msg += 7;
            evt->seqnum = atoi(msg);
        } else if (!strncmp(msg, "ACTION=", 7)) {
            msg += 7;
            if (!strncmp(msg, "add", 3)) {
                evt->action = NetlinkEvent::NlActionAdd;
            } else if (!strncmp(msg, "remove", 6)) {
                evt->action = NetlinkEvent::NlActionRemove;
            } else if (!strncmp(msg, "move", 4)) {
                evt->action = NetlinkEvent::NlActionMove;
            } else if (!strncmp(msg, "change", 6)) {
                evt->action = NetlinkEvent::NlActionChange;
            }
        } else if (!strncmp(msg, "DEVPATH=", 8)) {
            msg += 8;
            evt->devpath = strdup(msg);
        } else if (!strncmp(msg, "SUBSYSTEM=", 10)) {
            msg += 10;
            evt->subsystem = strdup(msg);
        } else if (!strncmp(msg, "DEVNAME=", 8)) {
            msg += 8;
            evt->devname = strdup(msg);
        } else if (!strncmp(msg, "DEVTYPE=", 8)) {
            msg += 8;
            evt->devtype = strdup(msg);
        } else if (!strncmp(msg, "INTERFACE=", 10)) {
            msg += 10;
            evt->interface = strdup(msg);
        } else if (!strncmp(msg, "MAJOR=", 6)) {
            msg += 6;
            evt->major = atoi(msg);
        } else if (!strncmp(msg, "MINOR=", 6)) {
            msg += 6;
            evt->minor = atoi(msg);
        }

        while (*msg++);
    }

    return evt;
}

int NetlinkEvent::getProperty(const char *prop_name, char *property, int length)
{
    char *p;

    if (devpath != NULL) {
        char path[256], full_path[256];

        strncpy(path, devpath, sizeof(path));

        path[sizeof(path)-1] = 0;

        while ((p = strrchr(path, '/'))) {
            *p = 0;

            snprintf(full_path, sizeof(full_path), "/sys/%s/%s", path, prop_name);

            if (access(full_path, R_OK))
                continue;

            FILE *fp = fopen(full_path, "r");
            if (!fp)
                continue;

            size_t n = fread(property, 1, length, fp);

            fclose(fp);

            if (!n)
                continue;

            property[n-1] = 0;

            return n;
        }
    }

    return 0;
}

const char *NetlinkEvent::getPropertyAsString(const char *prop_name)
{
    char property[64];
    if (getProperty(prop_name, property, sizeof(property)) > 0)
        return strdup(property);
    return NULL;
}

int NetlinkEvent::getPropertyAsInteger(const char *prop_name)
{
    char property[64];
    if (getProperty(prop_name, property, sizeof(property)) > 0)
        return strtoul(property, NULL, 16);
    return 0;
}

const char *NetlinkEvent::getInterface()
{
    char property[64];
    if (interface == NULL) {
        if (getProperty("interface", property, sizeof(property)) > 0)
            interface = strdup(property);
    }
    return interface ? interface : "";
}

int NetlinkEvent::getIdVendor()
{
    if (!idVendor)
        idVendor = getPropertyAsInteger("idVendor");
    return idVendor;
}

int NetlinkEvent::getIdProduct()
{
    if (!idProduct)
        idProduct = getPropertyAsInteger("idProduct");
    return idProduct;
}
