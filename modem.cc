
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "modem.h"

Modem::Modem(const char *device)
    : WvModem(device, 115200)
{
    char buffer[32];

    Log::Info("Modem device: %s", device);

    inited = false;

    if (WvModem::isok()) {
        // drop garbage data
        while (WvModem::select(100, true, false))
            WvModem::read(buffer, sizeof(buffer));

        // check if this is a modem by sending AT command
        send("AT\r");

        if (recv(buffer, sizeof(buffer), 100) <= 0)
            return;

        buffer[sizeof(buffer)-1] = 0;
        if (!strstr(buffer, "OK"))
            return;

        inited = true;
    } else
        Log::Error("Modem initialization failed");
}

Modem::~Modem()
{
}

int Modem::send(const char *msg)
{
    Log::Trace("==> %s", msg);
    return WvModem::print(msg);
}

int Modem::sendln(const char *msg)
{
    int count = WvModem::print(msg);
    count += WvModem::print("\r");
    return count;
}

int Modem::recv(char *buffer, int length, int timeout)
{
    int offset = 0;

    if (!WvModem::select(timeout, true, false))
        return 0;

    offset = WvModem::read(buffer, length);

    while (offset < length && WvModem::select(100, true, false))
        offset += WvModem::read(buffer + offset, length - offset);

    return offset;
}
