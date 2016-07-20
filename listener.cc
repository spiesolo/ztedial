
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "listener.h"
#include "messagedispatch.h"

Listener::Listener(int _fd)
    : fd(_fd), idle_time(-1), started(false)
{
    ctrlfd[0] = ctrlfd[1] = -1;
}

Listener::~Listener()
{
    stop();
    close(fd);
}

void *Listener::thread_main(void *object)
{
    Listener* me = reinterpret_cast<Listener *>(object);
    me->run();
    pthread_exit(NULL);
    return NULL;
}

void Listener::setfd(int _fd)
{
    fd = _fd;
    if (started) {
        int msg = 1;
        write(ctrlfd[1], &msg, 1);
    }
}

void Listener::set_idle_time(int _time)
{
    idle_time = _time;
    if (started) {
        int msg = 1;
        write(ctrlfd[1], &msg, 1);
    }
}

void Listener::run()
{
    struct pollfd ufds[2];

    while (1) {
        int fd_count = 0;

        ufds[fd_count].fd = ctrlfd[0];
        ufds[fd_count].events = POLLIN;
        ufds[fd_count].revents = 0;
        fd_count++;

        if (fd > -1) {
            ufds[fd_count].fd = fd;
            ufds[fd_count].events = POLLIN;
            ufds[fd_count].revents = 0;
            fd_count++;
        }

        int n = poll(ufds, fd_count, idle_time);
        if (n < 0) {
            if (errno == EAGAIN || errno == EINTR)
                continue;
            break;
        } else if (!n) {
            if (!on_idle())
                return;
            continue;
        }

        int i;
        for (i = 0; i < fd_count; i++) {
            if (ufds[i].revents != 0) {
                if (ufds[i].fd == ctrlfd[0]) {
                    int msg;
                    if (read(ctrlfd[0], &msg, 1) < 0)
                        return;
                    if (msg == 0)
                        return;
                } else if (ufds[i].fd == fd) {
                    if (ufds[i].revents & POLLIN) {
                        if (!on_data_available())
                            return;
                    }
                    if (ufds[i].revents & (POLLERR | POLLHUP)) {
                        if (!on_error())
                            return;
                    }
                }
            }
        }
    }
}

bool Listener::start()
{
    if (started)
        return true;

    if (socketpair(AF_LOCAL, SOCK_STREAM, 0, ctrlfd)  < 0)
        return false;

    fcntl(ctrlfd[0], F_SETFD, O_CLOEXEC);
    fcntl(ctrlfd[1], F_SETFD, O_CLOEXEC);

    if (pthread_create(&thread, NULL, Listener::thread_main, this) < 0) {
        close(ctrlfd[0]);
        close(ctrlfd[1]);
        ctrlfd[0] = ctrlfd[1] = -1;
        return false;
    }

    started = true;

    return true;
}

bool Listener::stop()
{
    if (!started)
        return true;

    int msg = 0;
    write(ctrlfd[1], &msg, 1);

    pthread_join(thread, NULL);

    close(ctrlfd[0]);
    close(ctrlfd[1]);
    ctrlfd[0] = ctrlfd[1] = -1;

    started = false;

    return true;
}
