
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include "log.h"
#include "atlistener.h"

AtListener::AtListener(Modem* _modem)
    : Listener(_modem->getfd()), modem(_modem)
{
}

AtListener::~AtListener()
{
}

bool AtListener::on_data_available()
{
    bool ret = true;
    char response[256];
    int offset, n, count;

    offset = modem->read(buffer, sizeof(buffer));

    while (offset < (int)sizeof(buffer) && modem->select(100, true, false))
        offset += modem->read(buffer + offset, sizeof(buffer) - offset);

    n = 0;
    count = 0;
    while (n < offset) {
        for (; n < offset; n++) {
            if (buffer[n] == '\r')
                continue;
            if (buffer[n] == '\n') {
                if (count > 0) break;
                continue;
            }
            response[count++] = buffer[n];
            if (count > (int)(sizeof(response) - 1))
                break;
        }

        if (!count)
            continue;

        response[count] = 0;
        count = 0;

        Log::Trace("<== %s", response);

        AtEvent *evt = AtEvent::decodeFromMessage(response);
        if (evt) {
            ret = on_event(evt);
            delete evt;

            if (!ret)
                break;
        }
    }

    return ret;
}
