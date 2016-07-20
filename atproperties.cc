#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "atproperties.h"

AtProperties::AtProperties()
{
    propertyContainers = new PropertyCollection();
}

AtProperties::~AtProperties()
{
    clean();
    delete propertyContainers;
}

Property* AtProperties::find(const char* propName)
{
    PropertyCollection::iterator it;

    for (it = propertyContainers->begin(); it != propertyContainers->end(); it++) {
        Property* prop = *it;
        if (!strcmp(prop->name, propName))
            return prop;
    }

    return NULL;
}

bool AtProperties::wait(const char* propName, int timeout)
{
    Property* prop = find(propName);
    if (prop)
        return prop->mutex.wait(timeout);
    return false;
}

bool AtProperties::set(const char* propName, const char* propValue)
{
    Property* prop = find(propName);
    if (prop) {
        if (strncmp(propName, "STATUS", 6)) {
            if (!strcmp(prop->value, propValue))
                return true;
        }
        strncpy(prop->value, propValue, MAX_PROPERTY_VALUE);
        prop->value[MAX_PROPERTY_VALUE-1] = 0;
        prop->mutex.broadcast();
        return true;
    }
    return false;
}

bool AtProperties::set(const char* propName, int value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return set(propName, buffer);
}

bool AtProperties::get(const char* propName, char* propValue, int length)
{
    Property* prop = find(propName);

    if (propValue && length > 0)
        propValue[0] = 0;

    if (prop && propValue && length > 0) {
        strncpy(propValue, prop->value, length);
        propValue[length - 1] = 0;
        return true;
    }

    return false;
}

bool AtProperties::get(const char* propName, int* value)
{
    Property* prop = find(propName);
    if (prop) {
        if (value)
            *value = atoi(prop->value);
        return true;
    }
    return false;
}

bool AtProperties::add(const char* propName, const char* propValue)
{
    Property* prop = find(propName);
    if (!prop) {
        prop = new Property();
        strncpy(prop->name, propName, MAX_PROPERTY_NAME);
        prop->name[MAX_PROPERTY_NAME-1] = 0;
        strncpy(prop->value, propValue, MAX_PROPERTY_VALUE);
        prop->name[MAX_PROPERTY_VALUE-1] = 0;
        propertyContainers->push_back(prop);
        return true;
    }
    return false;
}

void AtProperties::clean()
{
    while (!propertyContainers->empty()) {
        Property* prop = propertyContainers->front();
        propertyContainers->pop_front();
        //prop->mutex.broadcast();
        delete prop;
    }
}
