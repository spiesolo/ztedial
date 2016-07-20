
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __MESSAGEDISPATCH_H
#define __MESSAGEDISPATCH_H

class MessageDispatch
{
private:
    static MessageDispatch* dispatcher;

    int                     mesgfd[2];
    bool                    inited;

    bool                    poll(int timeout);
public:
                            MessageDispatch();
    virtual                 ~MessageDispatch();

    bool                    ready() { return inited; }

    void                    post(const char* message);
    int                     read(char* message, int length, int timeout = -1); 
    int                     fd() { return mesgfd[0]; }

#if 0
    static void             Instantiate();
    static void             Dispose();
    static MessageDispatch* Instance();
    static void             Post(const char *message);
    static int              Get(char *message, int length, int timeout = -1);
#endif
};

#endif
