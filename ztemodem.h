
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ZTEMODEM_H
#define __ZTEMODEM_H

#include "modem.h"
#include "mutex.h"
#include "atevent.h"
#include "atlistener.h"

class ZteModem: public AtListener, public AtProperties
{
private:
    //AtProperties*   properties;
    Modem*          modem;
    bool            inited;

    Mutex           at_mutex;
    int             at_wait_command(int timeout = 1000);
    int             at_raw_command(const char* cmd);
    int             at_set_command(const char* name, int value);
    int             at_set_command(const char* name, const char* value);
    int             at_query_command(const char* name, char* data, int len);
    int             at_query_command(const char* name, int* value = NULL);

    const char*     network_status(int state);
    const char*     connection_status(int state);
public:
                    ZteModem(Modem* _modem);
    virtual         ~ZteModem();

    bool            init_modem();
    bool            soft_init();
    bool            dial();
    bool            disconnect();
    bool            hang_up();
    bool            reset();

    virtual bool    on_event(AtEvent* evt);
    virtual bool    on_error();

    static ZteModem* fromDevice(const char* device);
};

#endif
