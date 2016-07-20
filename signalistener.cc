
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <signal.h>

#include "log.h"
#include "signalistener.h"
#include "dispatchmanager.h"

SignalListener::SignalListener()
{
    signals = new SignalContainers();
}

SignalListener::~SignalListener()
{
    clean();
    delete signals;
}

void SignalListener::clean()
{
    while (!signals->empty()) {
        int s = signals->front();
        signals->pop_front();
        signal(s, SIG_DFL);
    } 
}

void SignalListener::watch(int sig)
{
    SignalContainers::iterator it;

    for (it = signals->begin(); it != signals->end(); it++) {
        int s = *it;
        if (s == sig)
            return;
    }

    signals->push_back(sig);
    signal(sig, SignalListener::signal_handler);
}

// @static
void SignalListener::signal_handler(int sig)
{
    char message[64];
    Log::Info("signal recevied: %d", sig);
    if (sig == SIGCHLD) {
        snprintf(message, sizeof(message), "ACTION=waitpid");
    } else {
        snprintf(message, sizeof(message), "ACTION=exit");
    }
    DispatchManager::Post(DispatchManager::main_id, message);
}
