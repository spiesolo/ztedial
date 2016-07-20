
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ATLISTENER_H
#define __ATLISTENER_H

#include "listener.h"
#include "modem.h"
#include "atevent.h"

class AtListener : public Listener
{
private:
    Modem*          modem;
    char            buffer[1024];

protected:
    virtual bool    on_data_available();

public:
                    AtListener(Modem*);
    virtual         ~AtListener();

    virtual bool    on_event(AtEvent *evt) = 0;
};

#endif

