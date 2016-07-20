
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __NETLINKEVENT_H
#define __NETLINKEVENT_H

class NetlinkEvent
{
private:
    int         seqnum;
    int         action;
    const char *devpath;
    const char *subsystem;
    const char *devname;
    const char *devtype;
    const char *interface;
    int         major;
    int         minor;
    int         idVendor;
    int         idProduct;

                NetlinkEvent();

    int         getProperty(const char *prop_name, char *property, int length);
    int         getPropertyAsInteger(const char *prop_name);
    const char *getPropertyAsString(const char *prop_name);
public:
    virtual     ~NetlinkEvent();

    int         getAction() { return action; }
    const char *getDevpath() { return devpath ? devpath : ""; }
    const char *getSubsystem() { return subsystem ? subsystem : ""; }
    const char *getDevname() { return devname ? devname : ""; }
    const char *getDevtype() { return devtype ? devtype : ""; }
    const char *getInterface();
    int         getIdVendor();
    int         getIdProduct();

    static const int NlActionUnknown;
    static const int NlActionAdd;
    static const int NlActionRemove;
    static const int NlActionMove;
    static const int NlActionChange;

    static NetlinkEvent *decodeFromMessage(const char *msg);
};

#endif
