
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __ATPROPERTIES_H
#define __ATPROPERTIES_H

#include <list>
#include "mutex.h"

#define MAX_PROPERTY_NAME       32
#define MAX_PROPERTY_VALUE      256

typedef struct property {
    char            name[MAX_PROPERTY_NAME];
    char            value[MAX_PROPERTY_VALUE];
    Mutex           mutex;
    //unsigned        futex; //for futex
} Property;

typedef std::list<Property*> PropertyCollection;

class AtProperties
{
private:
    PropertyCollection*     propertyContainers;

    Property*               find(const char* propName);
public:
                            AtProperties();
    virtual                 ~AtProperties();

    bool                    wait(const char* propName, int timeout = 100);
    bool                    set(const char* propName, const char* propValue);
    bool                    set(const char* propName, int value);
    bool                    get(const char* propName, char* propValue, int length);
    bool                    get(const char* propName, int* value);
    bool                    add(const char* propName, const char* propValue = "");
    void                    clean();
};

#endif
