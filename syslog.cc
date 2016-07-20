
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <syslog.h>
#include "syslog.h"

Syslog::Syslog(int level, const char* prefix)
    : Log(level, prefix)
{
    openlog(prefix, LOG_PID, LOG_DAEMON);
}

Syslog::~Syslog()
{
}

void Syslog::log(int priority, const char* tag, const char* buffer)
{
    static int __priorities[] = {
        LOG_EMERG,
        LOG_ALERT,
        LOG_CRIT,
        LOG_ERR,
        LOG_WARNING,
        LOG_NOTICE,
        LOG_INFO,
        LOG_DEBUG,
    };
    syslog(__priorities[priority & 0x07], "%s\n", buffer);
}
