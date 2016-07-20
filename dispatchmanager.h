
// Copyright: (C) 2016 ESIMTEK ALL RIGHTS RESERVED
// author: Yonghua Zheng

#ifndef __DISPATCHCONTAINER_H
#define __DISPATCHCONTAINER_H

#include <map>
#include "messagedispatch.h"
#include "mutex.h"

typedef std::map<int, MessageDispatch*> MessageDispatchContainer;

class DispatchManager
{
private:
    MessageDispatchContainer*   container;
    MessageDispatch*            loop;
    Mutex                       mutex;

    static DispatchManager*     singleton;

                                DispatchManager();
public:
    virtual                     ~DispatchManager();

    bool                        registerr(int id, MessageDispatch* dispatch);
    bool                        unregister(int id);
    bool                        post(int id, const char* message);
    bool                        get(int id, char* message, int length);

    static void                 Instantiate();
    static void                 Dispose();
    static bool                 Register(int id, MessageDispatch* dispatch);
    static bool                 Unregister(int id);
    static bool                 Post(int id, const char* message);
    static bool                 Get(int id, char* message, int length);

    static const int            main_id;
    static const int            modemmanager_id;
};

#endif
