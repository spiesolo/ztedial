
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ZTEDIAL_H
#define __ZTEDIAL_H

#include "ztemodemmanager.h"

class ZteDial
{
private:
    ZteModemManager* manager;

    int             run();

                    ZteDial(ZteModemManager* _manager);
public:
    virtual         ~ZteDial();

    static bool     dial(const char* device);
};

#endif
