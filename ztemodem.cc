
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <errno.h>
#include "ztemodem.h"
#include "dispatchmanager.h"

ZteModem::ZteModem(Modem *_modem)
    : AtListener(_modem), AtProperties(), modem(_modem)
{
}

ZteModem::~ZteModem()
{
    hang_up();
    delete modem;
}

bool ZteModem::init_modem()
{
    int cimi = 0, cfun = 0;

    if (inited)
        return true;

    AtProperties::add("STATUS", "");
    AtProperties::add("ZICCID", "");
    AtProperties::add("CIMI", "");
    AtProperties::add("CFUN", "");
    AtProperties::add("CREG", "");
    AtProperties::add("CGREG", "");
    AtProperties::add("CEREG", "");
    AtProperties::add("MODE", "");
    AtProperties::add("ZGACT", "");

    // start listener
    AtListener::start();

    // disable echo
    if (at_raw_command("ATE0\r"))
        goto out_err;

    // set error code report mode
    if (at_set_command("CMEE", 1))
        goto out_err;

    // set CREQ notification format
    if (at_set_command("CREG", 1))
        goto out_err;

    // set CGREQ notification format
    if (at_set_command("CGREG", 1))
        goto out_err;

    // set CEREQ notification format
    if (at_set_command("CEREG", 1))
        goto out_err;

    // query network status
    if (at_query_command("CREG"))
        goto out_err;

    // query GPRS network status
    if (at_query_command("CGREG"))
        goto out_err;

    // query EPS network status
    if (at_query_command("CEREG"))
        goto out_err;

    // query data connection status
    if (at_query_command("ZGACT"))
       goto out_err;

    // query IMSI, can fail if no SIM card
    at_query_command("CIMI", &cimi);

    // query CFUN state
    if (at_query_command("CFUN", &cfun))
        goto out_err;

    // reset the modem if no SIM card
    if (cimi == 0) {
        if (at_set_command("CFUN", 0))
            goto out_err;
        cfun = 0;
    }

    // setup PDP context
    if (cfun == 0) {
        // PDP configuration
        if (at_set_command("CGDCONT", "1,\"IP\""))
            goto out_err;

        if (cimi != 0) {
            // Protocol stack open radio frequency, initialize card and
            // search network
            if (at_set_command("CFUN", 1))
                goto out_err;
        }
    }

    // query CFUN state one more time
    if (at_query_command("CFUN", &cfun))
        goto out_err;

    inited = true;

    return true;
out_err:
    AtListener::stop();
    AtProperties::clean();
    return false;
}

bool ZteModem::soft_init()
{
    int cimi = 0, cfun = 0;

    // query IMSI, can fail if no SIM card
    at_query_command("CIMI", &cimi);

    // query CFUN state
    if (at_query_command("CFUN", &cfun))
        goto out_err;

    // setup PDP context
    if (cfun == 0 && cimi != 0) {
        // Protocol stack open radio frequency, initialize card and
        // search network
        if (at_set_command("CFUN", 1))
            goto out_err;
    }

    // query CFUN state one more time
    if (at_query_command("CFUN", &cfun))
        goto out_err;

    return true;
out_err:
    return false;
}

int ZteModem::at_wait_command(int timeout)
{
    int status = -1;
    if (!AtProperties::wait("STATUS", timeout))
        return -ETIMEDOUT;
    AtProperties::get("STATUS", &status);
    return status;
}

int ZteModem::at_raw_command(const char* cmd)
{
    int r;
    at_mutex.lock();
    modem->send(cmd);
    r = at_wait_command();
    at_mutex.unlock();
    return r;
}

int ZteModem::at_set_command(const char* name, int val)
{
    char command[256];
    snprintf(command, sizeof(command), "AT+%s=%d\r", name, val);
    return at_raw_command(command);
}

int ZteModem::at_set_command(const char* name, const char* val)
{
    char command[256];
    snprintf(command, sizeof(command), "AT+%s=%s\r", name, val);
    return at_raw_command(command);
}

int ZteModem::at_query_command(const char* name, char* data, int len)
{
    char command[256];
    int status;
    snprintf(command, sizeof(command), "AT+%s?\r", name);
    status = at_raw_command(command);
    if (!status && data && len > 0) {
        AtProperties::get(name, data, len); 
    }
    return status;
}

int ZteModem::at_query_command(const char* name, int* value)
{
    char command[256];
    int status;
    if (!strncmp(name, "CIMI", 4))
        snprintf(command, sizeof(command), "AT+%s\r", name);
    else
        snprintf(command, sizeof(command), "AT+%s?\r", name);
    status = at_raw_command(command);
    if (!status && value) {
        AtProperties::get(name, value);
    }
    return status;
}

const char* ZteModem::network_status(int state)
{
    const char* status = "UNREGISTER";

    switch (state) {
    case 1:
        status = "REGISTERED";
        break;
    case 2:
        status = "SEARCHING";
        break;
    default:
        break;
    }

    return status;
}

const char* ZteModem::connection_status(int state)
{
    if (state)
        return "CONNECT";
    else
        return "DISCONNECT";
}

bool ZteModem::dial()
{
    char value[MAX_PROPERTY_NAME];
    int cid = 0, state = 0;

    if (!inited)
        return false;

    // is network registered?
    if (!AtProperties::get("CEREG", &state))
        return false;

    if (state != 1)
        return false;

    // is already connected?
    if (!AtProperties::get("ZGACT", value, sizeof(value)))
        return false;

    if (sscanf(value, "%d,%d", &cid, &state) != 2)
        return false;

    if (state)
        return true;

    // connect now
    snprintf(value, sizeof(value), "%d,%d", cid, 1);
    if (at_set_command("ZGACT", value))
        return false;

    return true;
}

bool ZteModem::disconnect()
{
    char value[MAX_PROPERTY_NAME];
    int cid = 0, state = 0;

    if (!inited)
        return false;

    // is network registered?
    if (!AtProperties::get("CEREG", &state))
        return false;

    if (state != 1)
        return false;

    // is connected?
    if (!AtProperties::get("ZGACT", value, sizeof(value)))
        return false;

    if (sscanf(value, "%d,%d", &cid, &state) != 2)
        return false;

    if (!state)
        return true;

    // issue disconnect command
    snprintf(value, sizeof(value), "%d,%d", cid, 0);
    if (at_set_command("ZGACT", value))
        return false;

    return true;
}

bool ZteModem::hang_up()
{
    if (!inited)
        return false;

    // disable the modem
    at_set_command("CFUN", 0);

    // stop the listener
    AtListener::stop();

    // clean up the properties
    AtProperties::clean();

    inited = false;

    return true;
}

bool ZteModem::reset()
{
    if (!inited)
        return false;

    // disable the modem is same as reset
    at_set_command("CFUN", 0);
    AtProperties::set("CFUN", 0);

    return true;
}

bool ZteModem::on_error()
{
    char message[32];
    snprintf(message, sizeof(message), "ACTION=remove");
    DispatchManager::Post(DispatchManager::modemmanager_id, message);
    return false;
}

bool ZteModem::on_event(AtEvent *evt)
{
    char buffer[32], message[256];
    int status = evt->getStatus();
    int state, cid, opertype;

    if (status == AtEvent::AtEventOk) {
        AtProperties::set("STATUS", 0);
    } else if (status == AtEvent::AtEventError) {
        AtProperties::set("STATUS", evt->getErrorcode());
    } else if (status == AtEvent::AtEventReport) {
        const char* propName = evt->getPropName();
        const char* propValue = evt->getPropValue();
        const char* p;

        message[0] = 0;

        if (!strncmp(propName, "CFUN", 4)) {
            // modem status: +CFUN: <state>
            state = atoi(propValue);
            snprintf(message, sizeof(message), "STATE=%d", state);
        } else if (!strncmp(propName, "CREG", 4)) {
            // network status: +CREG: <n>,<state> || +CREG: <state>
            p = strchr(propValue, ',');
            if (p) propValue = ++p;
            state = atoi(propValue);
            snprintf(message, sizeof(message), "NETWORK=%d", state);
        } else if (!strncmp(propName, "CGREG", 5)) {
            // EPS network status: +CEREG: <n>,<state> || +CEREG: <state>
            p = strchr(propValue, ',');
            if (p) propValue = ++p;
            state = atoi(propValue);
            snprintf(message, sizeof(message), "GPRS=%d", state);
        } else if (!strncmp(propName, "CEREG", 5)) {
            // GPRS network status: +CGREG: <n>,<state> || +CGREG: <state>
            p = strchr(propValue, ',');
            if (p) propValue = ++p;
            state = atoi(propValue);
            snprintf(message, sizeof(message), "EPS=%d", state);
        } else if (!strncmp(propName, "ZGACT", 5)) {
            // RNDIS connection status: +ZGACT: <cid>,<state>
            if (sscanf(propValue, "%d,%d", &cid, &state) != 2)
                goto out;
            snprintf(message, sizeof(message), "CONNECTION=%d", state);
        } else if (!strncmp(propName, "ZCONSTAT", 7)) {
            // RNDIS connection status: +ZCONSTAT: <state>,<cid>
            if (sscanf(propValue, "%d,%d", &state, &cid) != 2)
                goto out;
            // convert to ZGACT format
            snprintf(buffer, sizeof(buffer), "%d,%d", cid, state);
            propName = "ZGACT";
            propValue = buffer;
            snprintf(message, sizeof(message), "CONNECTION=%d", state);
        } else if (!strncmp(propName, "ZICCID", 6)) {
            // ZICCID is used for reporting USIM hotplug event
            // this event is reported when
            //  a) USIM removal
            //  b) CFUN=0
            // olny "removal" is interested now
            int cid = atoi(propValue);
            if (cid == 0) {
                AtProperties::set("CIMI", "");
                strcpy(message, "USIMSLOT=0");
            }
        } else if (!strncmp(propName, "ZPBIC", 5)) {
            // USIM status report: +ZPBIC: <state>,<opertype>
            // this event is reported when:
            //  a) USIM removal
            //  b) CFUN=0 when USIM is available
            // only "USIM initialization complete" is interested now
            sscanf(propValue, "%d,%d", &state, &opertype);
            if (state == 1 && opertype == 1)
                strcpy(message, "USIMSLOT=1");
        }

        AtProperties::set(propName, propValue);

        if (message[0] != 0)
            DispatchManager::Post(DispatchManager::modemmanager_id, message);
    }
out:
    return true;
}

ZteModem* ZteModem::fromDevice(const char *device)
{
    Modem* modem = new Modem(device);
    if (!modem->is_inited()) {
        delete modem;
        return NULL;
    }
    return new ZteModem(modem);
}
