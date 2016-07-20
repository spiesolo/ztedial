
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "log.h"
#include "syslog.h"

Log* Log::logger = NULL;

void Log::Instantiate(const char* logmeth, int level, const char* prefix)
{
    if (!Log::logger) {
        if (!logmeth)
            logmeth = "syslog";

        if (!strncmp(logmeth, "syslog", 6))
            Log::logger = new Syslog(level, prefix);
        else
            Log::logger = new Log(level, prefix);
    }
}

Log::Log(int level, const char* prefix)
    : prio(level), tag(prefix)
{
}

Log::~Log()
{
}

void Log::SetPriority(int priority)
{
    prio = priority;
}

void Log::log(int priority, const char* tag, const char* buffer)
{
    fprintf(stdout, "%s: %s\n", tag, buffer);
}

void Log::log(int priority, const char* format, va_list ap) 
{
    char buffer[256];
    if (priority > prio)
        return;
    vsnprintf(buffer, sizeof(buffer), format, ap);
    log(priority, tag, buffer);
}

void Log::log(int priority, const char* format, ...)
{
    char buffer[256];

    if (priority > prio)
        return;

    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    log(priority, tag, buffer);
}

void Log::Error(const char* format, ...)
{
    if (Log::logger) {
        va_list ap;
        va_start(ap, format);
        Log::logger->log(LOG_PRIO_ERROR, format, ap);
        va_end(ap);
    }
}

void Log::Info(const char* format, ...)
{
    if (Log::logger) {
        va_list ap;
        va_start(ap, format);
        Log::logger->log(LOG_PRIO_ERROR, format, ap);
        va_end(ap);
    }
}

void Log::Debug(const char* format, ...)
{
    if (Log::logger) {
        va_list ap;
        va_start(ap, format);
        Log::logger->log(LOG_PRIO_ERROR, format, ap);
        va_end(ap);
    }
}

void Log::Trace(const char* format, ...)
{
    if (Log::logger) {
        va_list ap;
        va_start(ap, format);
        Log::logger->log(LOG_PRIO_ERROR, format, ap);
        va_end(ap);
    }
}

void Log::Dispose()
{
    if (Log::logger) {
        delete Log::logger;
        Log::logger = NULL;
    }
}
