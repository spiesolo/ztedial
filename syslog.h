
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __SYSLOG_H
#define __SYSLOG_H

#include "log.h"

class Syslog: public Log
{
protected:
    virtual void    log(int piority, const char* format, const char* buffer);
public:
                    Syslog(int level, const char* prefix);
    virtual         ~Syslog();
};

#endif
