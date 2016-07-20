
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __SYSLOG_PRIO_H
#define __SYSLOG_PRIO_H

#include <stdarg.h>

#define LOG_PRIO_EMERG   0
#define LOG_PRIO_ALERT   1
#define LOG_PRIO_CRIT    2
#define LOG_PRIO_ERROR   3
#define LOG_PRIO_INFO    4
#define LOG_PRIO_DEBUG   5
#define LOG_PRIO_TRACE   6
#define LOG_PRIO_VERBOSE 7

class Log
{
private:
    static Log*     logger;

    int             prio;
    const char*     tag;

protected:
    virtual void    log(int piority, const char* tag, const char* buffer);
    virtual void    log(int piority, const char* format, ...);
    virtual void    log(int piority, const char* format, va_list ap);

public:
                    Log(int level, const char* prefix);
    virtual         ~Log();

    void            SetPriority(int priority);

    static void     Error(const char* format, ...);
    static void     Info(const char* format, ...);
    static void     Debug(const char* format, ...);
    static void     Trace(const char* format, ...);

    static void     Instantiate(const char* logmeth, int level = LOG_PRIO_INFO, const char* prefix = "");
    static void     Dispose();
};

#endif
