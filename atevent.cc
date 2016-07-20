
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"
#include "atevent.h"

const int AtEvent::AtEventUnknown = 0;
const int AtEvent::AtEventOk = 1;
const int AtEvent::AtEventError = 2;
const int AtEvent::AtEventReport = 3;

AtEvent::AtEvent()
    : status(AtEvent::AtEventUnknown), errCode(0)
{
    propName[0] = propValue[0] = 0;
}

AtEvent::~AtEvent()
{
}

AtEvent* AtEvent::decodeFromMessage(char *response)
{
    // return messages can be either of following:
    //
    // +CME ERROR: errcode
    // +@NAME: @VALUE
    // ^@NAME: @VALUE
    // OK
    // ERROR

    AtEvent *evt = new AtEvent();

    if (!strncmp(response, "OK", 2)) {
        evt->status = AtEvent::AtEventOk;
        //printf("OK\n");
    } else if (!strncmp(response, "ERROR", 5)) {
        evt->status = AtEvent::AtEventError;
        evt->errCode = -1;
    } else if (!strncmp(response, "+CME ERROR: ", 12)) {
        evt->status = AtEvent::AtEventError;
        evt->errCode = atoi(response + 12);
        //printf("+CME ERROR: %d\n", evt->errCode);
    } else {
        if (*response != '+' && *response != '^')
            goto out_err;

        char *p = strchr(++response, ':');
        if (!p)
            goto out_err;

        *p++ = 0;

        strncpy(evt->propName, response, MAX_PROPERTY_NAME);
        evt->propName[MAX_PROPERTY_NAME-1] = 0;

        while (*p && *p == ' ') p++;

        strncpy(evt->propValue, p, MAX_PROPERTY_VALUE);
        evt->propValue[MAX_PROPERTY_VALUE-1] = 0;

        evt->status = AtEvent::AtEventReport;

        //printf("+%s: %s\n", evt->propName, evt->propValue);
    }

    return evt;

out_err:
    Log::Error("Skip unknown response: %s", response);
    delete evt;
    return NULL;
}

