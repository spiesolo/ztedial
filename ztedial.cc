
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include "dispatchmanager.h"
#include "ztedial.h"

ZteDial::ZteDial(ZteModemManager* _manager)
    : manager(_manager)
{
}

ZteDial::~ZteDial()
{
    if (manager)
        delete manager;
}

int ZteDial::run()
{
    for (;;) {
        char message[1024];
        int n = DispatchManager::Get(DispatchManager::main_id, message, sizeof(message));
        if (n < 0)
            break;
        if (!n)
            continue;
        char *p = message;
        if (!strncmp(p, "ACTION=", 7)) {
            p += 7;
            if (!strncmp(p, "exit", 4))
                break;
        }
    }

    return 0;
}

bool ZteDial::dial(const char* device)
{
    ZteModem* modem = ZteModem::fromDevice(device);
    if (!modem)
        return false;

    ZteModemManager* manager = new ZteModemManager(modem);
    if (!manager->ready()) {
        delete modem;
        return false;
    }

    ZteDial* dialer = new ZteDial(manager);
    dialer->run();
    delete dialer;

    return true;
}

