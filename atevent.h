
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ATEVENT_H
#define __ATEVENT_H

#include "atproperties.h"

class AtEvent
{
private:
    int             status;
    int             errCode;
    char            propName[MAX_PROPERTY_NAME];
    char            propValue[MAX_PROPERTY_VALUE];

                    AtEvent();
public:
    virtual         ~AtEvent();

    int             getStatus() { return status; }
    int             getErrorcode() { return errCode; }
    const char*     getPropName() { return propName; }
    const char*     getPropValue() { return propValue; }

    static const int AtEventUnknown;
    static const int AtEventOk;
    static const int AtEventError;
    static const int AtEventReport;

    static AtEvent* decodeFromMessage(char *response);
};


#endif
