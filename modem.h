
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __MODEM_H
#define __MODEM_H

#include "wvmodem.h"

class Modem : public WvModem
{
private:
        bool        inited;
public:
                    Modem(const char *device);
        virtual     ~Modem();

        bool        is_inited() { return inited; }

        int         getfd() { return WvModem::getrfd(); }

        int         send(const char *msg);
        int         sendln(const char *msg);
        int         recv(char *buffer, int length, int timeout);
        //int         select();
};

#endif
