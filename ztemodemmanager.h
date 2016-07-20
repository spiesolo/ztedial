
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ZTEMODEMMANAGER_H
#define __ZTEMODEMMANAGER_H

#include "ztemodem.h"
#include "messagedispatch.h"

class ZteModemManager : public Listener, public MessageDispatch
{
private:
    bool                inited;

    ZteModem*           modem;

    int                 modem_state;
    int                 nw_state;
    int                 gprs_state;
    int                 eps_state;
    int                 conn_state;

    virtual bool        on_data_available();
    virtual bool        on_idle();
public:
                        ZteModemManager(ZteModem* modem);
    virtual             ~ZteModemManager();

    bool                ready() { return inited; }

    //TODO: provide APIs for information query, e.g. signal strength
    //      or system information

    static const int    ModemStateDown;
    static const int    ModemStateUp;

    static const int    NetworkStateUnregister;
    static const int    NetworkStateRegistered;
    static const int    NetworkStateSearching;
    static const int    NetworkStateOthers;

    static const int    ConnStateDisconnect;
    static const int    ConnStateConnected;
};

#endif
