
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/select.h>

#include "messagedispatch.h"

MessageDispatch* MessageDispatch::dispatcher = NULL;

MessageDispatch::MessageDispatch()
{
    if (pipe(mesgfd) < 0)
        inited = false;
    else {
        inited = true;

        fcntl(mesgfd[0], F_SETFL, O_NONBLOCK);
        fcntl(mesgfd[1], F_SETFL, O_NONBLOCK);

        fcntl(mesgfd[0], F_SETFD, O_CLOEXEC);
        fcntl(mesgfd[1], F_SETFD, O_CLOEXEC);
    }
}

MessageDispatch::~MessageDispatch()
{
    if (inited) {
        close(mesgfd[0]);
        close(mesgfd[1]);
    }
}

void MessageDispatch::post(const char* message)
{
    TEMP_FAILURE_RETRY(write(mesgfd[1], message, strlen(message)+1));
}

bool MessageDispatch::poll(int timeout)
{
    struct pollfd ufds[1];

    ufds[0].fd = mesgfd[0];
    ufds[0].events = POLLIN;
    ufds[0].revents = 0;

    int ret = TEMP_FAILURE_RETRY(::poll(ufds, 1, timeout));
    if (ret > 0 && ufds[0].revents & POLLIN)
        return true;

    return false;
}

int MessageDispatch::read(char* message, int length, int timeout)
{
    int offset = 0, len = length -1;
    unsigned char ch;

    if (poll(timeout)) {
        while (offset < len) {
           int n = TEMP_FAILURE_RETRY(::read(mesgfd[0], &ch, 1));
           if (n < 0) {
               if (errno == EAGAIN) break;
               return n;
           } else if (!n)
               break;

            if (ch == '\r')
                continue;

            if (ch == '\n')
                continue;

            if (message)
                message[offset++] = ch;

            if (ch == 0)
                break;

            if (offset == len)
                break;
        }

        if (message)
            message[offset] = 0;
    }

    return offset;
}

#if 0
// @static
MessageDispatch* MessageDispatch::Instance()
{
    if (!MessageDispatch::dispatcher)
        MessageDispatch::dispatcher = new MessageDispatch();
    if (MessageDispatch::dispatcher && MessageDispatch::dispatcher->ready())
        return dispatcher;
    return NULL;
}

// @static
void MessageDispatch::Post(const char* message)
{
    if (!MessageDispatch::dispatcher || !MessageDispatch::dispatcher->ready())
        return;
    MessageDispatch::dispatcher->post(message);
}

// @static
int MessageDispatch::Get(char* message, int length, int timeout)
{
    if (!MessageDispatch::dispatcher || !MessageDispatch::dispatcher->ready())
        return 0;
    return MessageDispatch::dispatcher->read(message, length, timeout);
}

// @static
void MessageDispatch::Instantiate()
{
    MessageDispatch::Instance();
}

// @static
void MessageDispatch::Dispose()
{
    if (MessageDispatch::dispatcher) {
        delete MessageDispatch::dispatcher;
        MessageDispatch::dispatcher = NULL;
    }
}
#endif
