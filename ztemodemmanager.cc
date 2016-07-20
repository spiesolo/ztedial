
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <signal.h>
#include "log.h"
#include "ztemodemmanager.h"
#include "dispatchmanager.h"

const int ZteModemManager::ModemStateDown = 0;
const int ZteModemManager::ModemStateUp = 1;

const int ZteModemManager::NetworkStateUnregister = 0;
const int ZteModemManager::NetworkStateRegistered = 1;
const int ZteModemManager::NetworkStateSearching = 2;
const int ZteModemManager::NetworkStateOthers = 3;

const int ZteModemManager::ConnStateDisconnect = 0;
const int ZteModemManager::ConnStateConnected = 1;

ZteModemManager::ZteModemManager(ZteModem* _modem)
    : Listener(), MessageDispatch()
{
    modem_state = ZteModemManager::ModemStateDown;
    nw_state = ZteModemManager::NetworkStateUnregister;
    gprs_state = ZteModemManager::NetworkStateUnregister;
    eps_state = ZteModemManager::NetworkStateUnregister;
    conn_state = ZteModemManager::ConnStateDisconnect;

    modem = _modem;
    if (MessageDispatch::ready()) {
        Listener::setfd(MessageDispatch::fd());
        Listener::set_idle_time(10000); // wakeup every 10s if no activity
        Listener::start();
        DispatchManager::Register(DispatchManager::modemmanager_id, this);

        inited = modem->init_modem();
    } else
        inited = false;
}

ZteModemManager::~ZteModemManager()
{
    DispatchManager::Unregister(DispatchManager::modemmanager_id);
    Listener::stop();
    delete modem;
}

bool ZteModemManager::on_data_available()
{
    char message[1024];

    int n = MessageDispatch::read(message, sizeof(message));
    if (n < 0) {
        // something horrible wrong
        return false;
    } else if (!n) {
        // timeout
        return true;
    }

    char *p = message;

    Log::Info("MESSAGE: %s", message);

    if (!strncmp(p, "STATE=", 6)) {
        p += 6;
        if (atoi(p))
            modem_state = ZteModemManager::ModemStateUp;
        else
            modem_state = ZteModemManager::ModemStateDown;
    } else if (!strncmp(p, "NETWORK=", 8)) { 
        p += 8;
        nw_state = atoi(p);
    } else if (!strncmp(p, "CONNECTION=", 11)) {
        p += 11;
        conn_state = atoi(p);
    } else if (!strncmp(p, "EPS=", 4)) {
        p += 4;
        eps_state = atoi(p);
    } else if (!strncmp(p, "GPRS=", 5)) {
        p += 5;
        gprs_state = atoi(p);
    } else if (!strncmp(p, "ACTION=", 7)) {
        p += 7;
        if (!strncmp(p, "remove", 6))
            kill(getpid(), SIGHUP);
    } else if (!strncmp(p, "USIMSLOT=", 9)) {
        p += 9;
        // in case of removal of SIM, force modem state to Down
        // instead of resetting the modem, as resetting the modem
        // would cause modem to report the SIM availability, and
        // that can cause endless loop handling the SIM events
        // idle task will check this flag and reset the modem
        // accordingly
        if (atoi(p) == 0) {
            modem_state = ZteModemManager::ModemStateDown;
            nw_state = ZteModemManager::NetworkStateUnregister;
            gprs_state = ZteModemManager::NetworkStateUnregister;
            eps_state = ZteModemManager::NetworkStateUnregister;
            conn_state = ZteModemManager::ConnStateDisconnect;
        } else if (modem_state == ZteModemManager::ModemStateDown) {
            // in case of SIM insert, query the CIMI again and enable
            // the modem
            modem->soft_init();
        }
    }

    Log::Info("modem_state = %d, eps_state = %d, conn_state = %d", modem_state, eps_state, conn_state);
    // check if modem is ready
    if (modem_state != ZteModemManager::ModemStateUp) {
        goto out;
    }

    // check EPS network status
    if (eps_state != ZteModemManager::NetworkStateRegistered)
        goto out;

    // check data connection status
    if (conn_state == ZteModemManager::ConnStateDisconnect) {
        Log::Info("dialing...");
        modem->dial();
    }
out:
    return true;
}

bool ZteModemManager::on_idle()
{
    int cimi = 0;

    if (modem_state == ZteModemManager::ModemStateDown) {
        modem->get("CIMI", &cimi);
        if (cimi == 0) {
            // reset the modem
            modem->reset();
        }
    }
    return true;
}
